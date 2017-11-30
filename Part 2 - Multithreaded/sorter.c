// Authors: Andrew Hernandez & Michael Belmont
// CS214: Systems Programming Fall 2017
#include"sorter.h"

#include"mergesort.h"

LL *csvlist;
int processCounter;
char *sortParam;
char *output;
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

// ADJUSTED FOR PROJECT 2
void* sort(void *fpv)
{
	FILE *fp = (FILE*)fpv;
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
				if (strcmp(arr[0].line[i], sortParam) == 0) {
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
	
	int columnIndex = getColumnIndex(arr, numColumns, sortParam);
	if (columnIndex < 0) 
	{
		exit(EXIT_FAILURE);
	} 
	else 
	{
		// This is where the actual sorting happens
		mergeSort(arr, columnIndex, numRecords - 1);
	}
	
	csvfile newCSV;
	record newTable[numRecords];
	newCSV.table = newTable;
	
	for(i = 0; i < numRecords; i++) 
	{ // i < numRecords
		for(j = 0; j < numColumns; j++) 
		{
			if(j != (numColumns - 1))
			{
				newCSV.table[i].line[j] = strcat(arr[i].line[j], ",");
			}
			else
			{
				newCSV.table[i].line[j] = arr[i].line[j];
			}
		}
	}
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	pthread_mutex_lock(&mutex);
	
	LL *newNode;
	(*newNode).next = NULL;
	(*newNode).data = newCSV;
	
	if (csvlist == NULL) csvlist = newNode;
	else {
	LL *node = csvlist;
	while((*node).next != NULL)
		node = (*node).next;
	
	(*node).next = newNode;
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}


void countProcesses(char* directory)
{
	DIR *dir;
	dir = opendir(directory);
	
	struct dirent *dp;
	
	if(dir == NULL)
		return;
	
	while ((dp = readdir(dir)) != NULL)
	{
		char pathway[1024];
		snprintf(pathway, sizeof(pathway), "%s/%s", directory, dp->d_name);
		
		if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
            continue;
        }

		if(dp->d_type == 4){
            processCounter++;
            countProcesses(pathway);

        }
		
		else if(dp->d_type == DT_REG)
		{

            char *name = dp->d_name;
            int length = strlen(name);
            /* .csv file*/
            if(name[length - 3] == 'c' && name[length - 2] == 's' && name[length - 1] == 'v' && !(strstr(dp->d_name, "sorted"))){
         
                    processCounter++;
                
            }
        }
	}
}

// CURRENTLY WORKING ON
void* navigateDir(void *dName) {
	char *dirName = (char*)dName;
	
	DIR *dir;
	struct dirent *item;
	int counter = 0;
	dir = opendir(dirName);
	
	if(dir == NULL) { return NULL; }
	
	while((item = readdir (dir)) != NULL) 
	{
		if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
		{
			//printf("LEL");
			continue;
		}
		
		if(item->d_type == 4)
		{
			char pathway[1024];
			snprintf(pathway, sizeof(pathway), "%s/%s", dirName, item->d_name);
			pthread_t tid1;
			pthread_create(&tid1, NULL, navigateDir, pathway); 
			
		}
		
		else 
		{
			int length = strlen(item->d_name);
			
			if(item->d_name[length - 1] == 'v' && item->d_name[length - 2] == 's' && item->d_name[length - 3] == 'c')
			{
				counter++;
				char pathway[1024];
				snprintf(pathway, sizeof(pathway), "%s/%s", dirName, item->d_name);
				FILE *file = fopen(pathway, "r");
				pthread_t tid2;
				pthread_create(&tid2, NULL, sort, file);
			}
		}
	}
	return NULL;
}

// ALREADY ADJUSTED FOR PROJECT 2
int main(int argc, char **argv) {

	processCounter = 1;

	int i = getpid();
	printf("\n\nInitial PID: %d\n", i); 
	printf("PIDS of all child processes: "); fflush(stdout);
	
	char* column = (char*)malloc(sizeof(char*)*128);
	char* directory = (char*)malloc(sizeof(char*)*128);
	output = (char*)malloc(sizeof(char*)*128);
	int colCheck = 0;
	int dirCheck = 0;
	int outCheck = 0;
	
	if (argc < 3) {
        printf("Error: Need additional parameters\nDo not forget to use -c and specify column to sort on\n");
		exit(EXIT_FAILURE);
	}
	if (argc > 7) {
		printf("Error: Too many parameters\nExample parameters: -c <columnName> -d <directoryName> -o <outputDirectoryName>\nNote: -d <directoryName> and -o <outputDirectoryName> are optional\n");
		exit(EXIT_FAILURE);
	}
	
	// INPUT FLAGS IN ANY ORDER
	
	if (argc == 3) 
	{
		switch(argv[1][1])
		{
			case 'c':
				strcpy(column, argv[2]);
				colCheck = 1;
				break;
			case 'd':
				strcpy(directory, argv[2]);
				dirCheck = 1;
				break;
			case 'o':
				strcpy(output, argv[2]);
				outCheck = 1;
				break;
			default:
				printf("Error: Incorrect input.\n");
				exit(EXIT_FAILURE);
		}
    }
	
    else if (argc == 5) 
	{
    	switch(argv[1][1])
		{
			case 'c':
				strcpy(column, argv[2]);
				colCheck = 1;
				break;
			case 'd':
				strcpy(directory, argv[2]);
				dirCheck = 1;
				break;
			case 'o':
				strcpy(output, argv[2]);
				outCheck = 1;
				break;
			default:
				printf("Error: Incorrect input.\n");
				exit(EXIT_FAILURE);
		}
		
		switch(argv[3][1])
		{
			case 'c':
				strcpy(column, argv[4]);
				colCheck = 1;
				break;
			case 'd':
				strcpy(directory, argv[4]);
				dirCheck = 1;
				break;
			case 'o':
				strcpy(output, argv[4]);
				outCheck = 1;
				break;
			default:
				printf("Error: Incorrect input.\n");
				exit(EXIT_FAILURE);
		}
    }
	
    else if (argc == 7) 
	{
    	switch(argv[1][1])
		{
			case 'c':
				strcpy(column, argv[2]);
				colCheck = 1;
				break;
			case 'd':
				strcpy(directory, argv[2]);
				dirCheck = 1;
				break;
			case 'o':
				strcpy(output, argv[2]);
				outCheck = 1;
				break;
			default:
				printf("Error: Incorrect input.\n");
				exit(EXIT_FAILURE);
		}
		
		switch(argv[3][1])
		{
			case 'c':
				strcpy(column, argv[4]);
				colCheck = 1;
				break;
			case 'd':
				strcpy(directory, argv[4]);
				dirCheck = 1;
				break;
			case 'o':
				strcpy(output, argv[4]);
				outCheck = 1;
				break;
			default:
				printf("Error: Incorrect input.\n");
				exit(EXIT_FAILURE);
		}
		
		switch(argv[5][1])
		{
			case 'c':
				strcpy(column, argv[6]);
				colCheck = 1;
				break;
			case 'd':
				strcpy(directory, argv[6]);
				dirCheck = 1;
				break;
			case 'o':
				strcpy(output, argv[6]);
				outCheck = 1;
				break;
			default:
				printf("Error: Incorrect input.\n");
				exit(EXIT_FAILURE);
		}
    }
	
	// THE -c FLAG MUST BE PRESENT
	
	if (colCheck == 1)
	{
		printf("Error: The -c flag must always be present.\n");
		exit(EXIT_FAILURE);
	}
	
	countProcesses(directory);
	
	csvlist = NULL;
	
	if(dirCheck == 1)
	{
		navigateDir(directory);
	}
	else
	{
		navigateDir(".");
	}
	
	if(outCheck == 0) {
		strcpy(output, "");
	}
		
	
	printf("\nNumber of threads: %d\n\n", processCounter);
	return 0;
}

