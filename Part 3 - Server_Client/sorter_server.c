#include "sorter_server.h"

/* Macros (define your macros below) */
#define MAX_NUM_THREAD 10

// Port numbers
#define PORT 9000

/********** Helper functions ***********/
int status = 1; // for server running status
int num_of_thread = 0; // currently created num of threads

/* Global thread id pool */
tid_type tid_pool[MAX_NUM_THREAD];
	
void init_tid_pool()
{
	int i;
	for(i = 0; i < MAX_NUM_THREAD; i++)
	{
		tid_pool[i].isFree = 1;
		tid_pool[i].socketfd = -1;
	}
}

/* return the index of available tid in pool */
int get_tid()
{
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

void release_tid(int index)
{
	tid_pool[index].isFree = 1;
}
/********** Helper functions end *********/



//////// Part 3: Service function ////////////

// Define your service function to serve requests
// Service thread should read in a client request and handle it
void * service(void *args) 
{
	// this is the socket for our server 
	// to talk to client
	int index = (int)args;
	int client_socket = tid_pool[index].socketfd;
	// define two buffers, receive and send
	char send_buf[256] = "Hello World!";
	char recv_buf[256];
	/* STEP 5: receive data */
	// use read system call to read data 
	read(client_socket, recv_buf, 256);
	// replace receive buffer with your buffer name
	printf("[r] Reading from client: %s\n", recv_buf);

	/* STEP 6: send data */
	// prepare your sending data
	// use write system call to send data
	write(client_socket, send_buf, 256);

	printf("[s] Data sent\n");

	/* STEP 7: close socket */
	close(client_socket);

	/* Need to add lock here */
	release_tid(index);
	num_of_thread--;
}

/////// Part 2: Main Function ///////////////

int main(int argc, char **argv)
{
	// optional: You can add args checking here
		
	// We need a server socket and a client socket
	int server_sock, client_sock;
	// Then we need a sockaddr_in struct to hold
	// socket information
	struct sockaddr_in address;

	/* STEP 1: create socket and setup sockaddr_in */
	// REMEMBER: ALWAYS CHECK FAILURE WHEN YOU DO I/O
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		// print error message with perror()
		perror("socket");
		// exit your server
		exit(EXIT_FAILURE);
	}

	// setup the sockaddr_in struct you defined above
	// sin_family = AF_INET
	// sin_addr.s_addr = INADDR_ANY
	// sin_port = htons(port number)
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	/* STEP 2: bind */
	if (bind(server_sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		// perror() report error message
		perror("bind");
		// close socket
		close(server_sock);
		// exit your program
		exit(EXIT_FAILURE);
	}

	/* STEP 3: Listen */
	// Now we have a binded socket, we can use this 
	// socket to listen on a port
	if (listen(server_sock, 0) < 0)
	{
		// perror() report error message
		perror("listen");
		// close socket
		close(server_sock);
		// exit your program
		exit(EXIT_FAILURE);
	}

	printf("Waiting for connections...\n");

	init_tid_pool();
	// we use a while loop to keep waiting for connections
	while (status)
	{
		/* STEP 4: create connection/accept connection request */
		// use client socket to accept client request
		// accept(int server_socket, NULL, NULL)
		// you can setup the second and third arguments other 
		// than NULL
		client_sock = accept(server_sock, NULL, NULL);
		// check if accept() is successful or not
		if (client_sock < 0)
		{
			// perror() report error message
			perror("accept");
			// close socket
			close(server_sock);
			// exit your program
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
			pthread_create(&tid_pool[i].tid, NULL, service, (void *)i);
			/* You need to add lock here */
			num_of_thread++;
			/* add lock above */
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
