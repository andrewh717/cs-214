#include "sorter_server.h"

/* Macros (define your macros below) */
#define MAX_NUM_THREAD 2000

// Port numbers
#define PORT 9697

/********** Global variables/structures ***********/

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

int status = 1; // for server running status
int num_of_thread = 0; // currently created num of threads

pthread_mutex_t records_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numRecords_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tid_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numThreads_mutex = PTHREAD_MUTEX_INITIALIZER;

record *globalHead;
record *globalRear;

int numRecords = 0;

int numStart = 0;
pthread_mutex_t numStart_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Global thread id pool */
tid_type tid_pool[MAX_NUM_THREAD];

tid_node *startHead;
tid_node *startRear;
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;


/********** Helper functions ***********/
void init_tid_pool() {
	int i;
	for(i = 0; i < MAX_NUM_THREAD; i++)
	{
		tid_pool[i].isFree = 1;
		tid_pool[i].socketfd = -1;
	}
}

/* return the index of available tid in pool */
int get_tid() {
	int i;
	for (i = 0; i < MAX_NUM_THREAD; i++)
	{
		if (tid_pool[i].isFree == 1)
		{
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
	if(globalHead == NULL) {
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
	while(end != NULL) {
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
	// this is the socket for our server 
	// to talk to client
	//printf("i have entered the service method\n");
	int index = (intptr_t)args;
	int client_socket = tid_pool[index].socketfd;
	// define two buffers, receive and send
	// char send_buf[256] = "Hello World!";
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
			printf("TID saved: %u\n", (unsigned int)startHead->tid);
			startHead->next = NULL;
			startRear = startHead;
		} else {
			startRear->next = malloc(sizeof(tid_node));
			startRear->tid = pthread_self();
			printf("TID saved: %u\n", (unsigned int)startRear->tid);
			startRear = startRear->next;
		}
		pthread_mutex_unlock(&start_mutex);
		// Prepare to read in CSV contents
		//int receivingCSV = 1;
		int counter = 0;
		printf("~START~ received!\n");
		sleep(2);
		//while(receivingCSV == 1) {
		while (read(client_socket, recv_buf, 500) > 0) {
			//printf("in the loop, about to read!\n");
			///if(read(client_socket, recv_buf, 500) < 0) {
				//perror("read");
				//exit(EXIT_FAILURE);
			//}
			//printf("Counter: %d -- Received: %s\n", counter, recv_buf);
			if(strcmp(recv_buf, "~END~") == 0){
				printf("%s\n", recv_buf);
				memset(recv_buf, 0, 500);
				break;
			}
			pthread_mutex_lock(&records_mutex);
			processData(recv_buf);
			memset(recv_buf, 0, 500);
			//printf("I have processed data!\n");
			pthread_mutex_unlock(&records_mutex);
			counter++;
		}
	} 
	// DUMP request received
	else if(strcmp(req_buf, dump_req) == 0) {		
		// Prepare to send sorted CSV
		printf("~DUMPX~ received!\n");
		
		pthread_mutex_lock(&start_mutex);
		tid_node *startPtr = malloc(sizeof(tid_node));
		startPtr = startHead;
		while (startPtr != NULL && startPtr->tid != 0) {
			printf("About to join TID: %u\n",(unsigned int)startPtr->tid);
			pthread_join(startPtr->tid, NULL);
			startPtr = startPtr->next;
			printf("On to the next one!\n");
		}
		startHead->tid = 0;
		startRear = startHead;
		startPtr = startHead->next;
		tid_node *tempStartPtr;
		printf("i am before freeing startlist\n");
		while (startPtr != NULL) {
			tempStartPtr = startPtr;
			startPtr = startPtr->next;
			free(tempStartPtr);
		}
		startHead = NULL;
		startRear = NULL;
		pthread_mutex_unlock(&start_mutex);
		printf("got past freeing startlist\n");
		//  NEED TO GET COLUMNINDEX TO SORT ON
		pthread_mutex_lock(&records_mutex);

		//sleep(30);
		record *ptr = malloc(sizeof(record));
		record *temp = malloc(sizeof(record));
		ptr = globalHead;
		int count;
		count = 0;
		int i;
		char send[500];
		printf("i am right before the write loop\n");
		while(ptr != NULL){
			//printf("loop #%d\n", count);
			for(i = 0; i < 28; i++){
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
			printf("i am right after the for loop\n");
			//printf("about to write #%d\n", count);
			if(write(client_socket, send, 500) < 0){
				perror("write");
				exit(EXIT_FAILURE);
			}
			memset(send, 0, 500);
			//printf("successfully write #%d\n", count);
			count++;
			printf("i am before ptr = ptr->next\n");
			//temp = ptr;
			if (ptr->next == NULL) {
				break;
			}
			ptr = ptr->next;
			printf("ptr has been incremented\n");
			//if (temp != globalHead) {
			//	freeRecord(temp);
			//}
			//printf("i am after free stuff\n");
			//freeRecord(globalRear);
			//printf("loop completed #%d\n", count);
		}
		printf("i have exited the write while loop\n");
		
		temp = globalHead;
		freeRecord(temp);
		
		
		globalHead = malloc(sizeof(record));
		globalRear = malloc(sizeof(record));
		
		globalHead = NULL;
		globalRear = NULL;
		
		pthread_mutex_unlock(&records_mutex);
		printf("Dump completed successfully.\n");
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
	memset(req_buf, 0, 8);
	return NULL;
}

/////// Part 2: Main Function ///////////////

int main(int argc, char **argv) {
	// optional: You can add args checking here
		
	int server_sock, client_sock;
	socklen_t client_length;
	struct sockaddr_in server_address, client_address;
	globalHead = NULL;
	globalRear = NULL;
	
	startHead = NULL;
	startRear = NULL;

	/* STEP 1: create socket and setup sockaddr_in */
	// REMEMBER: ALWAYS CHECK FAILURE WHEN YOU DO I/O
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT);
	
	/* STEP 2: bind */
	if (bind(server_sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		perror("bind");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	/* STEP 3: Listen */
	if (listen(server_sock, 0) < 0) {
		perror("listen");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	client_length = sizeof(client_address);
	
	printf("Waiting for connections...\n");

	init_tid_pool();
	//FILE *recordFile;
	//recordFile = fopen("all-records.csv", "w");
	// Wait for connects using infinite loop
	while (status) {
		/* STEP 4: accept connection request */
		// use client socket to accept client request
		// accept(int server_socket, NULL, NULL)
		// you can setup the second and third arguments other 
		// than NULL
		client_sock = accept(server_sock, (struct sockaddr *) &client_address, &client_length);
		// check if accept() is successful or not
		if (client_sock < 0) {
			perror("accept");
			close(server_sock);
			exit(EXIT_FAILURE);
		}

		// replace client socket below with your client socket variable name
		printf("[+] Connect to client %d\n", client_sock); 
		
		// the last argument is tricky not a good way to do, just demo
		// replace client socket with your client socket variable name
		//if (num_of_thread < MAX_NUM_THREAD) {
			int i = get_tid();
			// replace client socket here
			tid_pool[i].socketfd = client_sock;
			//printf("about to create thread\n");
			pthread_mutex_lock(&tid_pool_mutex);
			pthread_create(&tid_pool[i].tid, NULL, service, (void *) (intptr_t) i);
			pthread_mutex_unlock(&tid_pool_mutex);
			printf("TID used in create: %u\n", (unsigned int) tid_pool[i].tid);
			pthread_mutex_lock(&numThreads_mutex);
			num_of_thread++;
			pthread_mutex_unlock(&numThreads_mutex);
			
			pthread_mutex_lock(&tid_pool_mutex);
			//pthread_join(tid_pool[i].tid, NULL);
			release_tid(i);
			pthread_mutex_unlock(&tid_pool_mutex);
		//}

		/* what if num_of_thread >= MAX_NUM_THREAD? 
		 * You need to think about a way to solve this
		 * problem
		 * CV/semaphore ... or other better solution
		 * */
		/*else {
			// just dirty code for demo ;)
			while (num_of_thread >= MAX_NUM_THREAD) {
				sleep(1);
			}
		}*/
		/*record *ptr = malloc(sizeof(record));
		ptr = globalHead;
		while(ptr != NULL){
			int i;
			for(i = 0; i < 28; i++){
				if (i != 27) {
					fprintf(recordFile, "%s,", ptr->line[i]);
				} else {
					fprintf(recordFile, "%s\n", ptr->line[i]);
				}
			}
			ptr = ptr->next;
		}*/
	}
	
	/* clean up */
	close(server_sock);
	return 0;
}