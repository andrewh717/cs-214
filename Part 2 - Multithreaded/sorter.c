// Authors: Andrew Hernandez & Michael Belmont
// CS214: Systems Programming Fall 2017
#include"sorter.h"
#include"mergesort.h"

int processCounter;
char *column;
char *outputDir;

// Trims the leading and trailing space of tokens
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

// Checks array of strings for duplicates
// Returns 1 if duplicates are found, 0 otherwise
int checkForDuplicates(char **str, int numStrings) {
	int i, j;
	for (i = 0; i < numStrings; i++) {
		for (j = i + 1; j < numStrings; j++) {
			if (strcmp(str[i], str[j]) == 0) {
				printf("Error: Duplicate column names found!\nCSV column names should be unique\n");
				return 1;
			}
		} 
	}
	return 0;
}

// Gets the index of the column which is being sorted on
int getColumnIndex(record *arr, int numColumns, char *columnName) {
	int i;
	for (i = 0; i < numColumns; i++) {
		if (strcmp(arr[0].line[i], columnName) == 0) {
			return i;
		}
	}
	return -1;
}

// Sorts a CSV given the file pointer, sort parameter, and the name of the file
// Since filename can't be extracted from file pointer, it needs to be passed as its own argument
// Assume that the file has already been checked prior to calling sort() to make sure it is a CSV
void sort(FILE *fp, char *fileName, char *outputDir){
	int i, j, numRecords, numColumns, currentToken;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread; // getline must return a variable of ssize_t containing the number of characters read
	numRecords = 0;
	record *arr = malloc(sizeof(record));
	while ((nread = getline(&line, &len, fp) != -1)) {
		
		if (strstr(line, ",") == NULL) {
			break;
		}
		arr = realloc(arr, (numRecords + 1) * sizeof(record));
		// Check if the given sort parameter is actually a column in the CSV
		if (numRecords == 1) {
			numColumns = currentToken; // The number of columns is the same as the number of tokens in a line
			arr[0].numColumns = numColumns; // Setting the number of columns for the row containing column names
			if (checkForDuplicates(arr[0].line, numColumns) == 1) {
				exit(EXIT_FAILURE);
			}
			int paramFound = 0;
			for (i = 0; i < numColumns; i++) {
				// The sort parameter only needs to be found once
				// If it is found, we proceed with the rest of the program
				if (strcmp(arr[0].line[i], column) == 0) {
					paramFound = 1;
				}
			}
			// If the sort parameter is not found, exit the program
			if (paramFound == 0) {
				//printf("Error: Sort parameter does not exist in the given file\n");
				exit(EXIT_FAILURE);
			}
		}
		char *token, *end = line;
		currentToken = 0;
		arr[numRecords].line = malloc(sizeof(char **));
		while (end != NULL) {
			if (end[0] != '\"') {
				token = strsep(&end, ",");	
			} else {
				end++;
				token = strsep(&end, "\"");
				end++;
			}
			if (currentToken > 0) {
				arr[numRecords].line = realloc(arr[numRecords].line, (currentToken + 1) * sizeof(char **));
			}
			trim(token);
			arr[numRecords].line[currentToken] = malloc(strlen(token) * sizeof(char) + 1);
			strcpy(arr[numRecords].line[currentToken], token);
			currentToken++;
		}
		arr[numRecords].numColumns = numColumns;
		numRecords++;
	}
	
	int columnIndex = getColumnIndex(arr, numColumns, column);
	if (columnIndex < 0) {
		exit(EXIT_FAILURE);
	} else {
		// This is where the actual sorting happens
		mergeSort(arr, columnIndex, numRecords - 1);
	}
	
	// Prints out all the records of the new CSV (including the column names)
	// Output file is created using fopen
	// Output file needs to be named as such:
	// 'movie_metadata.csv' sorted on the field 'movie_title' will result in a file named "movie_metadata-sorted-movie_title.csv".
	if (opendir(outputDir) == NULL) {
		mkdir(outputDir, S_IRWXU);
	}
	FILE *sortedFile;
	char *base, *extension;
	// Grab the extension (Note: it should always be ".csv")
	extension = strrchr(fileName, '.');
	const char *substring = strstr(fileName, extension);
	size_t baseLength = substring - fileName;
	// Get the base of the filename
	base = malloc(len + 1);
	memcpy(base, fileName, baseLength);
	base[baseLength] = '\0';
	if (strcmp(outputDir, "") != 0) {	
		base = strrchr(base, '/');	
	}
	int totalLength = strlen(outputDir) + strlen(base) + strlen("-sorted-") + strlen(column) + strlen(extension) + 1;
	char sortedFileName[totalLength];
	// Create the sorted file name (aka <csv_name>-sorted-<sort_parameter>.csv)
	snprintf(sortedFileName, totalLength + 1, "%s/%s-sorted-%s%s", outputDir, base, column, extension);
	sortedFile = fopen(sortedFileName, "w");
	for (i = 0; i < numRecords; i++) { // i < numRecords
		for (j = 0; j < numColumns; j++) {
			if (j != (numColumns - 1)) {
				fprintf(sortedFile, "%s,", arr[i].line[j]);}else{fprintf(sortedFile, "%s", arr[i].line[j]);
			}
		}
		fprintf(sortedFile, "\n");
	}
}


void countProcesses(char* directory) {
	DIR *dir;
	dir = opendir(directory);
	
	struct dirent *dp;
	
	if(dir == NULL) { 
		return; 
	}
	
	while ((dp = readdir(dir)) != NULL) {
		char pathway[1024];
		snprintf(pathway, sizeof(pathway), "%s/%s", directory, dp->d_name);
		
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }

		if(dp->d_type == 4) {
            processCounter++;
            countProcesses(pathway);

        }
		
		else if(dp->d_type == DT_REG) {
            char *name = dp->d_name;
            int length = strlen(name);
            /* Check to make sure it's a .csv file that isn't sorted*/
            if(name[length - 3] == 'c' && name[length - 2] == 's' && name[length - 1] == 'v' && !(strstr(dp->d_name, "-sorted-"))) {
                    processCounter++;
            }
        }
	}
}

void navigateDir(char *dirName) {
	DIR *dir;
	struct dirent *item;
	int counter = 0;
	dir = opendir(dirName);
	
	pthread_t tid1 = 0;
	pthread_t tid2 = 0;
	
	if(dir == NULL) { 
		return; 
	}
	
	while((item = readdir (dir)) != NULL) {
		if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0) {
			continue;
		}
		
		if(item->d_type == 4) {
			tid1 = pthread_create(&tid1, NULL, );
			if(tid == 0) {
				printf("%d, ", gettid()); fflush(stdout);
				char pathway[1024];
				snprintf(pathway, sizeof(pathway), "%s/%s", dirName, item->d_name);
				navigateDir(pathway);				
				exit(0);
			}
		} else {
			int length = strlen(item->d_name);
			if(item->d_name[length - 1] == 'v' && item->d_name[length - 2] == 's' && item->d_name[length - 3] == 'c' && !(strstr(item->d_name, "sorted"))) {
				counter++;
				//tid2 = pthread_create();
				if(pid2 == 0) {
					printf("%d, ", getpid()); fflush(stdout);
					char pathway[1024];
					snprintf(pathway, sizeof(pathway), "%s/%s", dirName, item->d_name);
					FILE *file = fopen(pathway, "r");
					sort(file, pathway, outputDir);
					exit(0);
				}				
			}
		}
	}
	
	if (pid1 != 0) {	
		wait(0); 
	}
	
	if (pid2 != 0) {
		while(counter > 0) { 
			wait(0); 
			counter--;
		}
	}
	return;
}

/* There can be 3-7 parameters
   Need to account for:
   ./sorter -c  movie_title 
   ./sorter -c  movie_title -d thisdir
   ./sorter -c  movie_title -o thatdir 
   ./sorter -c  movie_title -d thisdir -o thatdir 
*/
int main(int argc, char **argv) {
	processCounter = 1;
	int i = getpid();
	printf("\n\nInitial PID: %d\n", i); 
	printf("TIDs of all child threads: "); fflush(stdout);
	
	column = (char*)malloc(sizeof(char*)*128);
	char* directory = (char*)malloc(sizeof(char*)*128);
	outputDir = (char*)malloc(sizeof(char*)*128);
	
	if (argc < 3) {
        printf("Error: Need additional parameters\nDo not forget to use -c and specify column to sort on\n");
		exit(EXIT_FAILURE);
	}
	
	if (argc > 7) {
		printf("Error: Too many parameters\nExample parameters: -c <columnName> -d <directoryName> -o <outputDirectoryName>\nNote: -d <directoryName> and -o <outputDirectoryName> are optional\n");
		exit(EXIT_FAILURE);
	}
	
	if (argc == 3) {
    	strcpy(column, argv[2]);
    	strcpy(directory, ".");
    	strcpy(outputDir, ".");
    } else if (argc == 5 && strcmp(argv[3], "-d") == 0) {
    	strcpy(column, argv[2]);
    	strcpy(directory, argv[4]);
    	strcpy(outputDir, "");
    } else if (argc == 5 && strcmp(argv[3], "-o") == 0) {
    	strcpy(column, argv[2]);
    	strcpy(directory, ".");
    	strcpy(outputDir, argv[4]);
    } else if (argc == 7) {
    	strcpy(column, argv[2]);
    	strcpy(directory, argv[4]);
    	strcpy(outputDir, argv[6]);
    }
	
	// The -c flag must always be present
	if (argc >= 3 && strcmp(argv[1], "-c") != 0) {
		printf("Error: Incorrect first parameter given\nPlease use -c as the first parameter");
		exit(EXIT_FAILURE);
	}
	// Since -d and -o are optional, either can be the second flag
	if (argc > 3 && argv[3] != NULL && (strcmp(argv[3], "-d") != 0 && strcmp(argv[3], "-o") != 0)) {
		printf("Error: Incorrect flag given\nPlease use -d or -o as the second flag");
		exit(EXIT_FAILURE);
	}
	// If there are 3 flags, the third one is -o
	if (argc > 5 && argv[5] != NULL && strcmp(argv[5], "-o") != 0) {
		printf("Error: Incorrect flag given\nPlease use -o as the third flag");
		exit(EXIT_FAILURE);
	}
	// Checks to make sure -c is specified
	if (argc >= 3) {
		if(argv[1][0] != '-' && argv[1][1] != 'c') {
			return -1;
		}
	}
	
	// Variable to track if directory is specified
	int nav = 0;
	int out = 0;
	// Sets nav & out and verifies valid input
	if (argc == 5) {
		if(argv[3][1] != 'd' && argv[3][1] != 'o') {
			return -1;
		}
		if (argv[3][1] == 'd') {
			nav = 1;
		}
		if (argv[3][1] == 'o') {
			out = 1;
		}
	}
	
	if (argc == 7) {
		if(argv[3][1] != 'd' || argv[5][1] != 'o') {
			printf("Error: Invalid input. \nPlease try again.\n");
			exit(EXIT_FAILURE);
		}
		nav = 1;
		out = 1;
	}
	
	countProcesses(directory);
	
	if(nav == 1 && out == 0) {
		strcpy(outputDir, "");
		navigateDir(directory);
	} else if(nav == 1 && out == 1)	{
		navigateDir(directory);
	} else if(nav == 0 && out == 1)	{
		navigateDir(".");
	} else {
		strcpy(outputDir, "");
		navigateDir(".");
	}
	
	printf("\nTotal number of threads: %d\n\n", processCounter);
	return 0;
}
