#include "sorter_server.h"

/* Macros (define your macros below) */
#define MAX_NUM_THREAD 1024

// Port numbers
#define PORT 9000

/********** Global variables/structures ***********/
int status = 1; // for server running status
int num_of_thread = 0; // currently created num of threads

pthread_mutex_t records_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numRecords_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tid_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numThreads_mutex = PTHREAD_MUTEX_INITIALIZER;

record *globalHead;
record *globalRear;

int numRecords = 0;

/* Global thread id pool */
tid_type tid_pool[MAX_NUM_THREAD];


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
		printf("Token received: %s\n", token);
		currentToken++;
	}
	globalRear = ptr;
	pthread_mutex_lock(&numRecords_mutex);
	numRecords++;
	pthread_mutex_unlock(&numRecords_mutex);
	free(ptr);
}
/********** Helper functions end *********/



//////// Part 3: Service function ////////////

// Define your service function to serve requests
// Service thread should read in a client request and handle it
void * service(void *args) 
{
	// this is the socket for our server 
	// to talk to client
	int index = (intptr_t)args;
	int client_socket = tid_pool[index].socketfd;
	// define two buffers, receive and send
	// char send_buf[256] = "Hello World!";
	char recv_buf[500];
	char req_buf[7];
	const char post_req[7] = "~POST~";
	const char dump_req[7] = "~DUMP~";

	// Perform initial read to determine request type
	if(read(client_socket, req_buf, 7) < 0){
		perror("read");
		exit(EXIT_FAILURE);
	}

	// POST request received
	if(strcmp(req_buf, post_req) == 0) {
		// Prepare to read in CSV contents
		int receivingCSV = 1;
		while(receivingCSV == 1) {
			read(client_socket, recv_buf, 500);
			if(strcmp(recv_buf, "~END~") == 0){
				break;
			}
			pthread_mutex_lock(&records_mutex);
			processData(recv_buf);
			pthread_mutex_unlock(&records_mutex);
		}
	} 
	// DUMP request received
	else if(strcmp(req_buf, dump_req) == 0) {
		// Prepare to send sorted CSV
		exit(EXIT_FAILURE);
	}
	// Received CSV, now add it to list of stored CSVs
	// Sort list of stored CSVs

	/* STEP 6: send data */
	// use write system call to send data
	char send_buf[256] = "Hello client! - from server";
	write(client_socket, send_buf, 256);

	printf("[s] Data sent\n");

	/* STEP 7: close socket */
	close(client_socket);

	pthread_mutex_lock(&tid_pool_mutex);
	release_tid(index);
	pthread_mutex_unlock(&tid_pool_mutex);

	pthread_mutex_lock(&numThreads_mutex);
	num_of_thread--;
	pthread_mutex_unlock(&numThreads_mutex);

	pthread_exit(0);
	return NULL;
}

/////// Part 2: Main Function ///////////////

int main(int argc, char **argv) {
	// optional: You can add args checking here
		
	int server_sock, client_sock;
	struct sockaddr_in address;
	globalHead = NULL;
	globalRear = NULL;

	/* STEP 1: create socket and setup sockaddr_in */
	// REMEMBER: ALWAYS CHECK FAILURE WHEN YOU DO I/O
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	/* STEP 2: bind */
	if (bind(server_sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("bind");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	/* STEP 3: Listen */
	if (listen(server_sock, 0) < 0)
	{
		perror("listen");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	printf("Waiting for connections...\n");

	init_tid_pool();
	
	// Wait for connects using infinite loop
	while (status)
	{
		/* STEP 4: accept connection request */
		// use client socket to accept client request
		// accept(int server_socket, NULL, NULL)
		// you can setup the second and third arguments other 
		// than NULL
		client_sock = accept(server_sock, NULL, NULL);
		// check if accept() is successful or not
		if (client_sock < 0)
		{
			perror("accept");
			close(server_sock);
			exit(EXIT_FAILURE);
		}

		// replace client socket below with your client socket variable name
		printf("[+] Connect to client %d\n", client_sock); 

		// the last argument is tricky not a good way to do, just demo
		// replace client socket with your client socket variable name
		if (num_of_thread < MAX_NUM_THREAD)
		{
			int i = get_tid();
			// replace client socket here
			tid_pool[i].socketfd = client_sock;
			pthread_create(&tid_pool[i].tid, NULL, service, (void *) (intptr_t) i);
			
			pthread_mutex_lock(&numThreads_mutex);
			num_of_thread++;
			pthread_mutex_unlock(&numThreads_mutex);
			
			pthread_detach(tid_pool[i].tid);
		}

		/* what if num_of_thread >= MAX_NUM_THREAD? 
		 * You need to think about a way to solve this
		 * problem
		 * CV/semaphore ... or other better solution
		 * */
		else
		{
			// just dirty code for demo ;)
			while (num_of_thread >= MAX_NUM_THREAD)
			{
				sleep(1);
			}
		}
	}

	/* clean up */
	close(server_sock);
	return 0;
}
