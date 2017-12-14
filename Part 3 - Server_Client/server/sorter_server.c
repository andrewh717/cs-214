#include "sorter_server.h"

#define MAX_NUM_THREAD 2000

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

int status = 1; 
int num_of_thread = 0; // Number of currently created threads
pthread_mutex_t numThreads_mutex = PTHREAD_MUTEX_INITIALIZER;

// Pointers to linked list that holds the CSV data
record *globalHead;
record *globalRear;
pthread_mutex_t records_mutex = PTHREAD_MUTEX_INITIALIZER;

int numRecords = 0; // Number of records stored in the linked list
pthread_mutex_t numRecords_mutex = PTHREAD_MUTEX_INITIALIZER;

int numStart = 0; // Number of ~STARTX~ requests received
pthread_mutex_t numStart_mutex = PTHREAD_MUTEX_INITIALIZER;

tid_type tid_pool[MAX_NUM_THREAD];
pthread_mutex_t tid_pool_mutex = PTHREAD_MUTEX_INITIALIZER;

// Pointers to linked list that stores TIDs of threads assigned to handle ~STARTX~ requests
tid_node *startHead;
tid_node *startRear;
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;


/********** Helper functions ***********/
void init_tid_pool() {
	int i;
	for (i = 0; i < MAX_NUM_THREAD; i++) {
		tid_pool[i].isFree = 1;
		tid_pool[i].socketfd = -1;
	}
}

// Returns the index of available TID in pool
int get_tid() {
	int i;
	for (i = 0; i < MAX_NUM_THREAD; i++) {
		if (tid_pool[i].isFree == 1) {
			return i;
		}
	}
	return -1;
}

void release_tid(int index) {
	tid_pool[index].isFree = 1;
}

void trim(char *str) {
	int i;
	int begin = 0;
	int end = strlen(str) - 1;
	while (isspace((unsigned char) str[begin])) {
		begin++;
	}
	while ((end >= begin) && isspace((unsigned char) str[end])) {
		end--;
	}
	// Shift all chars back to the start of the string array
	for (i = begin; i <= end; i++) {
		str[i - begin] = str[i];
	}
	str[i - begin] = '\0'; // Null terminate string
}

void processData(char *buffer) {
	//printf("I HAVE ENTERED PROCESSDATA\n");
	record *ptr = malloc(sizeof(record));
	if (globalHead == NULL) {
		// malloc record
		// add to global list?
		globalHead = malloc(sizeof(record));
		globalHead->line = malloc(sizeof(char **));
		globalHead->next = NULL;
		globalRear = globalHead;
		ptr = globalHead;
	} else {
		globalRear->next = malloc(sizeof(record));
		globalRear->next->line = malloc(sizeof(char **));
		globalRear->next->next = NULL;
		ptr = globalRear->next;
	}
	char *token, *end = buffer;
	int currentToken = 0;
	while (end != NULL) {
		if (end[0] != '\"') {
			token = strsep(&end, ",");	
		} else {
			end++;
			token = strsep(&end, "\"");
			end++;
		}
		if (currentToken > 0) {
			ptr->line = realloc(ptr->line, (currentToken + 1) * sizeof(char **));
		}
		trim(token);
		ptr->line[currentToken] = malloc(strlen(token) * sizeof(char) + 1);
		strcpy(ptr->line[currentToken], token);
		pthread_mutex_lock(&file_mutex);
		//fprintf(recordFile, "%s", ptr->line[currentToken]);
		pthread_mutex_unlock(&file_mutex);
		currentToken++;
	}
	globalRear = ptr;
	pthread_mutex_lock(&numRecords_mutex);
	numRecords++;
	pthread_mutex_unlock(&numRecords_mutex);
	//free(ptr);
}

record * freeRecord(record *ptr) {
	if (ptr == NULL) {
		return NULL;
	} 
	int i;
	for (i = 0; i < 28; i++) {
		free(ptr->line[i]);
	}
	free(ptr->line);
	free(freeRecord(ptr->next));
	return NULL;
}
/********** Helper functions end *********/



//////// Part 3: Service function ////////////

// Define your service function to serve requests
// Service thread should read in a client request and handle it
void * service(void *args) {
	int index = (intptr_t)args;
	int client_socket = tid_pool[index].socketfd;

	char recv_buf[500];
	char req_buf[9];
	const char post_req[9] = "~STARTX~";
	const char dump_req[9] = "~DUMPXY~";

	// Perform initial read to determine request type
	if(read(client_socket, req_buf, 9) < 0){
		perror("read");
		exit(EXIT_FAILURE);
	}

	// POST request received
	if(strcmp(req_buf, post_req) == 0) {
		pthread_mutex_lock(&start_mutex);
		if(startHead == NULL) {
			startHead = malloc(sizeof(tid_node));
			startHead->tid = pthread_self();
			//printf("TID saved: %u\n", (unsigned int)startHead->tid);
			startHead->next = NULL;
			startRear = startHead;
		} else {
			startRear->next = malloc(sizeof(tid_node));
			startRear->tid = pthread_self();
			//printf("TID saved: %u\n", (unsigned int)startRear->tid);
			startRear = startRear->next;
		}
		pthread_mutex_unlock(&start_mutex);
		
		int counter = 0;
		//printf("~START~ received!\n");
		sleep(2);
		while (read(client_socket, recv_buf, 500) > 0) {
			//printf("Counter: %d\n", counter);
			pthread_mutex_lock(&records_mutex);
			record *checkGlobalHead = malloc(sizeof(record));
			checkGlobalHead = globalHead;
			pthread_mutex_unlock(&records_mutex);
			if (counter == 0 && checkGlobalHead != NULL) {
				counter++;
				continue;
			}
			if (strcmp(recv_buf, "~END~") == 0) {
				//printf("%s\n", recv_buf);
				memset(recv_buf, 0, 500);
				break;
			}
			pthread_mutex_lock(&records_mutex);
			processData(recv_buf);
			memset(recv_buf, 0, 500);
			pthread_mutex_unlock(&records_mutex);
			counter++;
		}
	} 
	// DUMP request received
	else if (strcmp(req_buf, dump_req) == 0) {		
		// Prepare to send sorted CSV
		//printf("~DUMPX~ received!\n");
		char columnIndex_buf[3];
		int columnIndex;
		if (read(client_socket, columnIndex_buf, 3) < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		columnIndex = atoi(columnIndex_buf);
		
		pthread_mutex_lock(&start_mutex);
		tid_node *startPtr = malloc(sizeof(tid_node));
		startPtr = startHead;
		while (startPtr != NULL && startPtr->tid != 0) {
			//printf("About to join TID: %u\n",(unsigned int)startPtr->tid);
			pthread_join(startPtr->tid, NULL);
			startPtr = startPtr->next;
			//printf("On to the next one!\n");
		}
		
		startHead->tid = 0;
		startRear = startHead;
		startPtr = startHead->next;
		tid_node *tempStartPtr;
		
		while (startPtr != NULL) {
			tempStartPtr = startPtr;
			startPtr = startPtr->next;
			free(tempStartPtr);
		}
		startHead = NULL;
		startRear = NULL;
		pthread_mutex_unlock(&start_mutex);
		
		pthread_mutex_lock(&records_mutex);

		//sleep(30);
		record *ptr = malloc(sizeof(record));
		record *temp = malloc(sizeof(record));
		ptr = globalHead;
		//printf("%s\n", ptr->line[columnIndex]);
		merge(&ptr->next, columnIndex);
		
		int count;
		count = 0;
		int i;
		char send[500];
		while (ptr != NULL) {
			for (i = 0; i < 28; i++) {
				if (i != 27) {
					//printf("about to access token#%d in loop#%d\n", i, count);
					strcat(send, ptr->line[i]);
					strcat(send, ",");
				} else {
					//printf("about to access token#%d in loop#%d\n", i, count);
					strcat(send, ptr->line[i]);
					strcat(send, "\n");
				}
			}
			if (write(client_socket, send, 500) < 0) {
				perror("write");
				exit(EXIT_FAILURE);
			}
			memset(send, 0, 500);
			//printf("successfully write #%d\n", count);
			count++;
			if (ptr->next == NULL) {
				break;
			}
			ptr = ptr->next;
		}
		temp = globalHead->next;
		freeRecord(temp);
	
		globalHead = NULL;
		globalRear = NULL;
		
		pthread_mutex_unlock(&records_mutex);
		//printf("\nDump completed successfully.\n");
		//printf("Column Index sorted: %d\n", columnIndex);
	} else {
		printf("invalid request type\n");
	}
	
	close(client_socket);

	pthread_mutex_lock(&numThreads_mutex);
	num_of_thread--;
	pthread_mutex_unlock(&numThreads_mutex);
	
	if (strcmp(req_buf, dump_req) == 0) {
		pthread_exit(0);		
	}
	memset(req_buf, 0, 9);
	return NULL;
}

/////// Part 2: Main Function ///////////////

int main(int argc, char **argv) {
	if (argc != 3 || strcmp(argv[1], "-p") != 0) {
		perror("invalid parameters");
		exit(EXIT_FAILURE);
	}
	uint16_t port = (uint16_t) atoi(argv[2]);
		
	int server_sock, client_sock;
	socklen_t client_length;
	struct sockaddr_in server_address, client_address;
	
	globalHead = NULL;
	globalRear = NULL;
	
	startHead = NULL;
	startRear = NULL;

	// Check if socket() is successful or not
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);
	
	if (bind(server_sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		perror("bind");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	if (listen(server_sock, 0) < 0) {
		perror("listen");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	client_length = sizeof(client_address);
	
	printf("Received connections from: \n");

	init_tid_pool();
	
	// Wait for connects using infinite loop
	while (status) {
		client_sock = accept(server_sock, (struct sockaddr *) &client_address, &client_length);
		
		// Check if accept() is successful or not
		if (client_sock < 0) {
			perror("accept");
			close(server_sock);
			exit(EXIT_FAILURE);
		}

		// Get connection's IP address and print it
		struct sockaddr_in* ipV4Addr = (struct sockaddr_in*)&client_address;
		struct in_addr ipAddr = ipV4Addr->sin_addr;
		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ipAddr, ipStr, INET_ADDRSTRLEN);
		printf(" %s,", ipStr); 
		
		int i = get_tid();
		
		tid_pool[i].socketfd = client_sock;
		
		pthread_mutex_lock(&tid_pool_mutex);
		pthread_create(&tid_pool[i].tid, NULL, service, (void *) (intptr_t) i);
		pthread_mutex_unlock(&tid_pool_mutex);
		//printf("TID used in create: %u\n", (unsigned int) tid_pool[i].tid);
		pthread_mutex_lock(&numThreads_mutex);
		num_of_thread++;
		pthread_mutex_unlock(&numThreads_mutex);
		
		pthread_mutex_lock(&tid_pool_mutex);
		release_tid(i);
		pthread_mutex_unlock(&tid_pool_mutex);
	}
	close(server_sock);
	return 0;
}
