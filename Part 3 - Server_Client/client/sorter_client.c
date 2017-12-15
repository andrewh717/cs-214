#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "sorter_client.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 9000
#define ADDR 127.0.0.1

pthread_mutex_t thread_ID_list = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t file_open_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t running_thread_count = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_thread_count = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t global_linkedlist = PTHREAD_MUTEX_INITIALIZER;

//threadIds * threadIdsHead = NULL;
//threadIds* threadIdsRear = NULL;

int sortByCategory(char* sortColumnName){
	if(strcmp(sortColumnName, "color") == 0) {
		return 0;
	}
	else if(strcmp(sortColumnName, "director_name") == 0) {
		return 1;
	}
	else if(strcmp(sortColumnName, "num_critic_for_reviews") == 0) {
		return 2;
	}
	else if(strcmp(sortColumnName, "duration") == 0) {
		return 3;
	}
	else if(strcmp(sortColumnName, "director_facebook_likes") == 0) {
		 return 4;
	}
	else if(strcmp(sortColumnName, "actor_3_facebook_likes") == 0) {
		return 5;
	}
	else if(strcmp(sortColumnName, "actor_2_name") == 0) {
		return 6;
	}
	else if(strcmp(sortColumnName, "actor_1_facebook_likes") == 0) {
		return 7;
	}
	else if(strcmp(sortColumnName, "gross") == 0) {
		return 8;
	}
	else if(strcmp(sortColumnName, "genres") == 0) {
		return 9;
	}
	else if(strcmp(sortColumnName, "actor_1_name") == 0) {
		return 10;
	}
	else if(strcmp(sortColumnName, "movie_title") == 0) {
		return 11;
	}
	else if(strcmp(sortColumnName, "num_voted_users") == 0) {
		return 12;
	}
	else if(strcmp(sortColumnName, "cast_total_facebook_likes") == 0) {
		return 13;
	}
	else if(strcmp(sortColumnName, "actor_3_name") == 0) {
		return 14;
	}
	else if(strcmp(sortColumnName, "facenumber_in_poster") == 0) {
		return 15;
	}
	else if(strcmp(sortColumnName, "plot_keywords") == 0) {
		return 16;
	}
	else if(strcmp(sortColumnName, "movie_imdb_link") == 0) {
		return 17;
	}
	else if(strcmp(sortColumnName, "num_user_for_reviews") == 0) {
		return 18;
	}
	else if(strcmp(sortColumnName, "language") == 0) {
		return 19;
	}
	else if(strcmp(sortColumnName, "country") == 0) {
		return 20;
	}
	else if(strcmp(sortColumnName, "content_rating") == 0) {
		return 21;
	}
	else if(strcmp(sortColumnName, "budget") == 0) {
		return 22;
	}
	else if(strcmp(sortColumnName, "title_year") == 0) {
		return 23;
	}
	else if(strcmp(sortColumnName, "actor_2_facebook_likes") == 0) {
		return 24;
	}
	else if(strcmp(sortColumnName, "imdb_score") == 0) {
		return 25;
	}
	else if(strcmp(sortColumnName, "aspect_ratio") == 0) {
		return 26;
	}
	else if(strcmp(sortColumnName, "movie_facebook_likes") == 0) {
		return 27;
	}
	else {
		printf("Fatal Error: Please input a proper category title\n");
		exit(0);
	}
}

void * traverseDirectory(void * args){
	//printf("ENTER TRAVERSE\n");
	
	char * path = NULL;
	path = strdup(((traverseDirectoryArgs*)args)->path);
	char** argv = ((traverseDirectoryArgs*)args)->argv;
	//int sortingBy = ((traverseDirectoryArgs*)args)->sortingBy;
	int existsNewOutDir = ((traverseDirectoryArgs*)args)->existsNewOutDir;
	char * outPath = NULL;;
	outPath = strdup(((traverseDirectoryArgs*)args)->outPath);
	int* totalThreads = ((traverseDirectoryArgs*)args)->totalThreads;
	int* runningThreadCount = ((traverseDirectoryArgs*)args)->runningThreadCount;
	int colLoc = ((traverseDirectoryArgs*)args)->colLoc;
	
	//printf("%u,", (unsigned int) pthread_self());
	
	/*pthread_mutex_lock(&thread_ID_list);
	if(threadIdsHead == NULL){
		threadIdsHead = (threadIds*)malloc(sizeof(threadIds));
		threadIdsHead->tid = pthread_self();
		threadIdsHead->next = NULL;
		
		threadIdsRear = threadIdsHead;
	}
	else{
		threadIdsRear->next = (threadIds*)malloc(sizeof(threadIds));
		threadIdsRear = threadIdsRear->next;
		threadIdsRear->tid = pthread_self();
		threadIdsHead->next = NULL;
	}
	pthread_mutex_unlock(&thread_ID_list);
	*/
	//printf("REAR TID %u\n", (unsigned int) threadIdsRear->tid);
	
	//printf("outPath in traverseDir = %s\n", outPath);
	//flag to see if we printed or not, defaul 0, will be set to current pid after pid printed, must be passed because same process can recursively call traverseDirectory()
	pthread_t tid;
	int err;
	//int pid = -1;
	DIR* directoryPointer;
	struct dirent* currentObject;
	struct stat buffer;
	
	//traverseDirectoryArgs args = malloc(sizeof(traverseDirectoryArgs);
	
	//printf("path to file = %s\n", path);
	
	if((directoryPointer = opendir(path)) == NULL){
		printf("Fatal Error: The directory <%s> does not exist.\n", path);
		//return;
		pthread_mutex_lock(&running_thread_count);
		(*runningThreadCount)--;
		pthread_mutex_unlock(&running_thread_count);
		pthread_exit(0);
	}
	
	//This will change the current working directory (cwd) to the given location, which will be the path we send the function.
	chdir(path);
	
	//hostname in argv[4]
	int socketFD;
	int portnum = atoi(argv[6]);
	
	struct sockaddr_in socketAddress;
	struct hostent * server = gethostbyname(argv[4]);
	
	char line[500];
	int charWrite;
	int fileNum = 0;
	memset(line, 0, sizeof(line));
	
	//We will be reading through everything in the current directory.
	while((currentObject = readdir(directoryPointer)) != NULL){
		
		lstat(currentObject->d_name, &buffer);
		
		//printf("CURRENTOBJECT NAME = %s\n", currentObject->d_name);
		
		if((strcmp(currentObject->d_name, "..") == 0) || (strcmp(currentObject->d_name, ".") == 0)){
					continue;
		}
		//If the current object being read is a directory...
		if(S_ISDIR(buffer.st_mode)){
			//Fork a new process
			fflush(0);
			//These are edge names that mess up the traversal (".." make the pointer go up a level)
			if((strcmp(currentObject->d_name, "..") == 0) || (strcmp(currentObject->d_name, ".") == 0)){
				continue;
			}
			//printf("looking at directory... %s\n", currentObject->d_name);
			//We create a char array located in the stack that will hold the path to the new directory we must traverse.
			char* newPath = NULL;
			newPath = getcwd(newPath, 1024);
			
			//This adds the "/" needed before we give another file direction
			strcat(newPath, "/");
			//This adds the new directory name to the new path
			strcat(newPath, currentObject->d_name);
			
			//Repeat process with new directory
			//printf("\nduplicate child prob gets here\n");
			
			pthread_mutex_lock(&total_thread_count);
			(*totalThreads)++;
			pthread_mutex_unlock(&total_thread_count);
			
			pthread_mutex_lock(&running_thread_count);
			(*runningThreadCount)++;
			pthread_mutex_unlock(&running_thread_count);
			
			
			//traverseDirectoryArgs args = malloc(sizeof(traverseDirectoryArgs));
			
			//What is i for this thread
			((traverseDirectoryArgs*)args)->path = strdup(newPath);
			//args->argv = argv;
			//args->sortingBy = sortingBy;
			((traverseDirectoryArgs*)args)->existsNewOutDir = existsNewOutDir;
			((traverseDirectoryArgs*)args)->outPath = strdup(outPath);
			((traverseDirectoryArgs*)args)->totalThreads = totalThreads;
			((traverseDirectoryArgs*)args)->runningThreadCount = runningThreadCount;
			((traverseDirectoryArgs*)args)->colLoc = colLoc;
			
			void * (*traverseFuncPointer)(void*) = traverseDirectory;
			
			err = pthread_create(&tid, NULL, traverseFuncPointer, (void*)args);
			if(err != 0) {
				printf("Fatal error: Thread did not create properly\n");
				//Should it be exit or return?
				pthread_mutex_lock(&running_thread_count);
				(*runningThreadCount)--;
				pthread_mutex_unlock(&running_thread_count);
				pthread_exit(0);
			}
			
		}
		//The currentObject is a file so...
		else{
			//printf("current file looked at = %s\n", currentObject->d_name);
			//Ignore an already sorted file
			if(strstr(currentObject->d_name, "-sorted")){
					continue;
			}
			//Ignore non-csv files
			else if((strstr(currentObject->d_name, ".csv")) == NULL){
					continue;
			}
			//The only remaining option is for the file to be considered for sorting. Thus, fork a new process.
			else{
				//IN THIS CONDITIONAL, REQUEST CONNECTION TO SERVER AND START SENDING LINES
				//Create method that to connects to server and sends records from valid file line by line
				
				
				memset(&socketAddress, 0, sizeof(socketAddress));
	
				socketAddress.sin_family = AF_INET;
	
				socketAddress.sin_port = htons(portnum);
	
				bcopy((char*)server->h_addr, (char*)&socketAddress.sin_addr.s_addr, server->h_length);
	
				socketFD = socket(AF_INET, SOCK_STREAM, 0);
	
				if(socketFD < 0){
					perror("Fatal Error: Socket Creation");
					exit(EXIT_FAILURE);
				}
	
				if(connect(socketFD, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) < 0){
					perror("Fatal Error: Socket Connection");
					close(socketFD);
					exit(EXIT_FAILURE);
				}
				
				FILE * currentFile = fopen(currentObject->d_name, "r");
			
				//Now, open the file and send line by line
				charWrite = write(socketFD, "~STARTX~", 9);
				fgets(line, 500, currentFile);
				charWrite = write(socketFD, line, 500);
				memset(line, 0, sizeof(line));
				int linesSent = 0;
				
				while(fgets(line, 500, currentFile)){
					//printf("Writing to server line: %s\n", line);
					linesSent++;
					charWrite = write(socketFD, line, 500);
					//printf("LinesSent = %d\n", linesSent);
					//printf(" ");
					if(charWrite <= 0){
						perror("Socket Write");
						break;
					}
					
					//printf("File num looking at = %d\n", fileNum);
					//printf("Num chars written = %d\n", charWrite);
					
					memset(line, 0, sizeof(line));
				}
				
				charWrite = write(socketFD, "~END~", 6);
				fclose(currentFile);
				fileNum++;
			}
		}
		
	}
	//Traverse back up to previous file path
	chdir("..");
	closedir(directoryPointer);
	
	pthread_mutex_lock(&running_thread_count);
	(*runningThreadCount)--;
	pthread_mutex_unlock(&running_thread_count);
	
	//printf("YAY IM EXITING FROM TRAVERSE DIRECT\n");
	
	pthread_exit(0);
	return NULL;
}

int main(int argc, char ** argv) {
	
	int portnum = -1;
	int socketFD;
	
	struct sockaddr_in socketAddress;
	
	struct hostent * server;
	
	char * execPath = NULL;
	execPath = getcwd(execPath, 1024);
	
	int * totalThreads = (int *)malloc(sizeof(int));
	*totalThreads = 0;
	
	int * runningThreadCount = (int *)malloc(sizeof(int));
	*runningThreadCount = 0;
	
	int existsNewOutDir = 0;
	int err;
	traverseDirectoryArgs* args = malloc(sizeof(traverseDirectoryArgs));
	//printf("\nargc = %d\n", argc);
	//int initPID = getpid();
	//printf("Initial PID: %d\n", initPID);
	//printf("\tTIDS of all child threads: ");
	
	//The input format will be incorrect if argc is 1, or an even number
	if((argc == 1) || ((argc % 2) == 0)){
		printf("Fatal Error: The format of the input is incorrect. Please use a combination of the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
		exit(0);
	}
	int hasCflag = 0;
	int dFlag = 0;
	int oFlag = 0;
	
	int sortingBy = -1;
	
	//Default for searchDir should be current directory, default for outputDir should be whatever searchDir is. Think Joe wanted to wait until after input checks to see if he has to do that.
	DIR* searchDir = NULL;
	DIR* outputDir = NULL;
	char* outPath = NULL;
	
	//storing where the name of the column heading is
	int colLoc = -1;
	
	//This starts on the first argument after ./sorter
	int i = 1;
	for(i = 1; i < argc; i++) {
		switch(i) {
			case 1:
				if(strcmp(argv[i], "-c") == 0) {
					hasCflag++;
					sortingBy = sortByCategory(argv[i+1]);
					colLoc = i+1;
					i++;
				}
				else {
					printf("Fatal Error: The format of the input is incorrect. Please use the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
				}
				break;
				
			case 2:
				//category name to sort by
				//Shouldn't be reached since we increment i again in previous section
				break;
				
			case 3:
				if(strcmp(argv[i], "-h") == 0) {
					//printf("hostname = %s\n", argv[i+1]);
					server = gethostbyname(argv[i+1]);
					if(server == NULL){
						printf("Fatal Error: Host does not exist.");
						exit(0);
					}
					i++;
				}
				else {
					printf("Fatal Error: The format of the input is incorrect. Please use the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
				}
				break;
				
			case 4:
				//hostname
				//Shouldn't be reached since we increment i again in previous section
				break;
				
			case 5:
				if(strcmp(argv[i], "-p") == 0) {
					//printf("portnum = %s\n", argv[i+1]);
					portnum = atoi(argv[i+1]);
					
					i++;
				}
				else {
					printf("Fatal Error: The format of the input is incorrect. Please use the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
				}
				break;
				
			case 6:
				// port #
				//Shouldn't be reached since we increment i again in previous section
				break;
				
			case 7:
				if(strcmp(argv[i], "-o") == 0) {
					if(oFlag != 0) {
						printf("Fatal Error: repeated flag. Please only use -o flag once.\n");
						exit(0);
					}
					oFlag++;
					if(argv[i+1] == NULL){
						printf("Fatal Error: The format of the input is incorrect. Please use a combination of the format: ./sorter.c -c <column heading> -o <outputdir>\n");
						exit(0);
					}
					else{
						outputDir = opendir(argv[i+1]);
						if(outputDir == NULL){
							printf("The file directory does not exist\n");
							exit(0);
						}
						existsNewOutDir = 1;
						if(strcmp(argv[i+1], "./") == 0){
							outPath = getcwd(outPath, 1024);
						}
						else if(strstr(argv[i+1], "../")){
							outPath = getcwd(outPath, 1024);
							outPath = strcat(outPath, "/");
							outPath = strcat(outPath, argv[i+1]);
							//printf("outPath = %s\n", outPath);
						}
						else{
							outPath = strdup(argv[i+1]);
						}
						//printf("outPath in main = %s\n", outPath);
					}
				}
				//The 7 output will be -d
				else if(strcmp(argv[i], "-d") == 0) {
					if(dFlag != 0) {
						printf("Fatal Error: repeated flag. Please only use -d flag once.\n");
						exit(0);
					}
					dFlag++;
					if(argv[i+1] == NULL){
						printf("Fatal Error: The format of the input is incorrect. Please use a combination of the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
					}
					else{
						searchDir = opendir(argv[i+1]);
						if(searchDir == NULL){
							printf("The file directory does not exist\n");
							exit(0);
						}
						fflush(0);
						
						pthread_mutex_lock(&total_thread_count);
						(*totalThreads)++;
						pthread_mutex_unlock(&total_thread_count);
						
						pthread_mutex_lock(&running_thread_count);
						(*runningThreadCount)++;
						pthread_mutex_unlock(&running_thread_count);
						
						//traverseDirectoryArgs args = malloc(sizeof(traverseDirectoryArgs));
						((traverseDirectoryArgs*) args)->path = strdup(argv[i+1]);
						((traverseDirectoryArgs*) args)->argv = argv;
						((traverseDirectoryArgs*) args)->sortingBy = sortingBy;
						((traverseDirectoryArgs*) args)->existsNewOutDir = existsNewOutDir;
						((traverseDirectoryArgs*) args)->outPath = getcwd(((traverseDirectoryArgs*) args)->outPath, 1024);
						((traverseDirectoryArgs*) args)->totalThreads = totalThreads;
						((traverseDirectoryArgs*) args)->runningThreadCount = runningThreadCount;
						((traverseDirectoryArgs*) args)->colLoc = colLoc;
						
						pthread_t tid;
						void * (*traverseFuncPointer)(void*) = traverseDirectory;
						
						err = pthread_create(&tid, NULL, traverseFuncPointer, (void*)args);
						
						if(err != 0) {
							printf("Fatal error: Thread did not create properly\n");
							exit(0);
						}
						
					}
				}
				else {
					printf("Fatal Error: The format of the input is incorrect. Please use the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
				}
				i++;
				break;
				
			case 8:
				// -o or -d parameter, and incremented to skip
				break;
				
			case 9:
				if(strcmp(argv[i], "-o") == 0) {
					if(oFlag != 0) {
						printf("Fatal Error: repeated flag. Please only use -o flag once.\n");
						exit(0);
					}
					if(argv[i+1] == NULL){
						printf("Fatal Error: The format of the input is incorrect. Please use a combination of the format: ./sorter.c -c <column heading> -o <outputdir>\n");
						exit(0);
					}
					else{
						outputDir = opendir(argv[i+1]);
						if(outputDir == NULL){
							printf("The file directory does not exist\n");
							exit(0);
						}
						existsNewOutDir = 1;
						if(strcmp(argv[i+1], "./") == 0){
							outPath = getcwd(outPath, 1024);
						}
						else if(strstr(argv[i+1], "../")){
							outPath = getcwd(outPath, 1024);
							outPath = strcat(outPath, "/");
							outPath = strcat(outPath, argv[i+1]);
							//printf("outPath = %s\n", outPath);
						}
						else{
							outPath = strdup(argv[i+1]);
						}
						//printf("outPath in main = %s\n", outPath);
					}
				}
				//The 4 output will be -d
				else if(strcmp(argv[i], "-d") == 0) {
					if(dFlag != 0) {
						printf("Fatal Error: repeated flag. Please only use -d flag once.\n");
						exit(0);
					}
					if(argv[i+1] == NULL){
						printf("Fatal Error: The format of the input is incorrect. Please use a combination of the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
					}
					else{
						searchDir = opendir(argv[i+1]);
						if(searchDir == NULL){
							printf("The file directory does not exist\n");
							exit(0);
						}
						fflush(0);
					
						pthread_mutex_lock(&total_thread_count);
						(*totalThreads)++;
						pthread_mutex_unlock(&total_thread_count);
						
						pthread_mutex_lock(&running_thread_count);
						(*runningThreadCount)++;
						pthread_mutex_unlock(&running_thread_count);
						
						traverseDirectoryArgs * args = malloc(sizeof(traverseDirectoryArgs));
						((traverseDirectoryArgs*)args)->path = strdup(argv[i+1]);
						((traverseDirectoryArgs*)args)->argv = argv;
						((traverseDirectoryArgs*)args)->sortingBy = sortingBy;
						((traverseDirectoryArgs*)args)->existsNewOutDir = existsNewOutDir;
						((traverseDirectoryArgs*)args)->outPath = getcwd(((traverseDirectoryArgs*)args)->outPath, 1024);
						((traverseDirectoryArgs*)args)->totalThreads = totalThreads;
						((traverseDirectoryArgs*)args)->runningThreadCount = runningThreadCount;
						((traverseDirectoryArgs*)args)->colLoc = colLoc;
						
						pthread_t tid;
						void * (*traverseFuncPointer)(void*) = traverseDirectory;
						
						err = pthread_create(&tid, NULL, traverseFuncPointer, (void*)args);
						
						if(err != 0) {
							printf("Fatal error: Thread did not create properly\n");
							exit(0);
						}
						
					}
				}
				else {
					printf("Fatal Error: The format of the input is incorrect. Please use the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
						exit(0);
				}
				i++;
				break;
				
			case 10:
				// -d or -o info, incremented to skip
				break;
				
			default:
				printf("Fatal Error: This should never happen. Something wrong with for loop.\n");
				exit(0);
		}
	}
	
	
	if(hasCflag == 0) {
		printf("Fatal Error: The format of the input is incorrect. Missing -c flag. Please use a combination of the format: ./sorter.c -c <column heading> <-d thisdir> <-o thatdir>\n");
		exit(0);
	}

	char* cwd = NULL;
	
	//If we were not given a specific directory to traverse, we will utilize the current directory
	if(searchDir == NULL){
		cwd = getcwd(cwd, 1024);
		if(cwd == NULL) {
			printf("Fatal error: Something wrong with default directory. Please fix me.\n");
			exit(0);
		}
		//printf("CWD = %s\n", cwd);
		searchDir = opendir(cwd);
		if(searchDir == NULL){
			printf("Fatal error: Something wrong with default directory. Please fix me.\n");
			exit(0);
		}
		
		fflush(0);
		
		pthread_mutex_lock(&total_thread_count);
		(*totalThreads)++;
		pthread_mutex_unlock(&total_thread_count);
		
		pthread_mutex_lock(&running_thread_count);
		(*runningThreadCount)++;
		pthread_mutex_unlock(&running_thread_count);
		
		((traverseDirectoryArgs*)args)->path = strdup(cwd);
		((traverseDirectoryArgs*)args)->argv = argv;
		((traverseDirectoryArgs*)args)->sortingBy = sortingBy;
		((traverseDirectoryArgs*)args)->existsNewOutDir = existsNewOutDir;
		((traverseDirectoryArgs*)args)->outPath = getcwd(((traverseDirectoryArgs*)args)->outPath, 1024);
		((traverseDirectoryArgs*)args)->totalThreads = totalThreads;
		((traverseDirectoryArgs*)args)->runningThreadCount = runningThreadCount;
		((traverseDirectoryArgs*)args)->colLoc = colLoc;
		
		pthread_t tid;
		void * (*traverseFuncPointer)(void*) = traverseDirectory;
						
		err = pthread_create(&tid, NULL, traverseFuncPointer, (void*)args);
		
		if(err != 0) {
			printf("Fatal error: Thread did not create properly\n");
			exit(0);
		}
		
	}
	
	fflush(0);

	//NOTE: fflush(0) is necessary before forking in order to clear the I/O buffer to repeat incredibly repetitive output.
	
	//printf("END OF MAIN THREADIDSHEAD TID = %u\n", (unsigned int) threadIdsHead->tid);
	
	
	while((*runningThreadCount) > 0){
		//printf("runningThreadCount = %d\n", *runningThreadCount);
		sleep(1);
	}
	
	
	if(outPath == NULL){
		outPath = execPath;
	}
	
	//Changes current working directory to the output directory where we will put the final file
	chdir(outPath);
	
	char line[500];
	memset(line, 0, sizeof(line));
	
	//Request connection to server to write to all-sorted file
	//printf("server = %s\n", argv[4]);
	//printf("portnum = %d\n", portnum);
	
	memset(&socketAddress, 0, sizeof(socketAddress));
	
	socketAddress.sin_family = AF_INET;
	
	socketAddress.sin_port = htons(portnum);
	
	bcopy((char*)server->h_addr, (char*)&socketAddress.sin_addr.s_addr, server->h_length);
	
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	
	if(socketFD < 0){
		perror("Fatal Error: Socket Creation");
		exit(EXIT_FAILURE);
	}
	
	if(connect(socketFD, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) < 0){
		perror("Fatal Error: Socket Connection");
		close(socketFD);
		exit(EXIT_FAILURE);
	}
	
	//printf("\noutput directory = %s\n", outPath);
	char* finalFilePath = (char*)malloc(sizeof(char) * (22 + strlen(outPath) + strlen(argv[colLoc])));
	
	memcpy(finalFilePath, outPath, strlen(outPath));
	strcat(finalFilePath, "/AllFiles-sorted-");
	strcat(finalFilePath, argv[colLoc]);
	strcat(finalFilePath, ".csv");
	
	//printf("\nend file name = %s\n", finalFilePath);
	
	FILE* finalFile = fopen(finalFilePath, "w");
	int charsRead;
	char sortBy[3];
	//Here, print to file using data from server.
	
	//Repeat this until end of file
	charsRead = write(socketFD, "~DUMPXY~", 9);
	snprintf(sortBy, 3, "%d", sortingBy);
	
	charsRead = write(socketFD, sortBy, 3);
	while(1){
		memset(line, 0, sizeof(line));
		charsRead = read(socketFD, line, 500);
		if(charsRead < 0){
			perror("Socket Read");
			memset(line, 0, sizeof(line));
			break;
		}
		else if(charsRead == 0){
			memset(line, 0, sizeof(line));
			break;
		}
		printf("Num chars received = %d\n", charsRead);
		printf("Line received = %s\n", line);
	
		fprintf(finalFile, line);
		
		
	}
	
	close(socketFD);
	fclose(finalFile);
	
	//printf("\n\tTotal number of threads: %d\n", *totalThreads);
	return 0;
}
