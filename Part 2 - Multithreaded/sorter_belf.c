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
	
	for (i = 0; i < numColumns; i++) 
	{
		printf("its the if\n");
		if (strcmp(arr[0].line[i], columnName) == 0) 
		{
			return i;
		}
	}
	return -1;
}

// ADJUSTED FOR PROJECT 2
void* sort(void *path) {
	char *pathway = (char*)path;
	FILE *fp = fopen(pathway, "r");
	int i, numRecords, numColumns, currentToken, columnIndex;
	columnIndex = 0;
	numColumns = 0;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread; // getline must return a variable of ssize_t containing the number of characters read
	numRecords = 0;
	record *arr = malloc(sizeof(record));
	while ((nread = getline(&line, &len, fp) != -1)) {
		//printf("The value of numRecords: %d", numRecords);
		if (strstr(line, ",") == NULL) {
			break;
		}

		arr = realloc(arr, (numRecords + 1) * sizeof(record));
		
		//printf("haha xd\n");
		// Check if the given sort parameter is actually a column in the CSV
		if (numRecords == 1) 
		{
			numColumns = currentToken; // The number of columns is the same as the number of tokens in a line
			
			arr[0].numColumns = numColumns; // Setting the number of columns for the row containing column names
			if (checkForDuplicates(arr[0].line, numColumns) == 1) 
			{
				exit(EXIT_FAILURE);
			}
			
			int paramFound = 0;
			for (i = 0; i < numColumns; i++) 
			{
				// The sort parameter only needs to be found once
				// If it is found, we proceed with the rest of the program
				
				if (strcmp(arr[0].line[i], sortParam) == 0) 
				{
					columnIndex = i;
					paramFound = 1;
					break;
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
		
		while (end != NULL) 
		{
			if (end[0] != '\"') 
			{
				token = strsep(&end, ",");	
			} 
			
			else 
			{
				end++;
				token = strsep(&end, "\"");
				end++;
			}
			
			if (currentToken > 0) 
			{
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
	
	printf("I am before the if\n");
	
	if (columnIndex < 0) 
	{
		exit(EXIT_FAILURE);
	} 
	else 
	{
		// This is where the actual sorting happens
		mergeSort(arr, columnIndex, numRecords - 1);
	}
	printf("lol\n");
	csvfile newCSV;
	newCSV.table = arr;
	newCSV.numRows = numRecords;
	printf("Num Records: %d\n", numRecords);
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	pthread_mutex_lock(&mutex);
	printf("still ALIVE alive\n");
	LL *newNode = malloc(sizeof(newNode));
	newNode->next = NULL;
	newNode->data = newCSV;
	printf("WE INNIT\n");
	if (csvlist == NULL) csvlist = newNode;
	else {
	LL *node = csvlist;
	while((*node).next != NULL)
		node = (*node).next;
	
	(*node).next = newNode;
	}
	pthread_mutex_unlock(&mutex);
	printf("I'm about to exit SORT\n");
	pthread_exit(0);
}


void countProcesses(char* directory) {
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
	
	pthread_t tid2;
	
	DIR *dir;
	struct dirent *item;
	int counter = 0;
	dir = opendir(dirName);
	
	if(dir == NULL) { return NULL; }
	
	while((item = readdir (dir)) != NULL) 
	{
		if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
		{
			continue;
		}
		
		if(item->d_type == 4)
		{
			char pathway[1024];
			snprintf(pathway, sizeof(pathway), "%s/%s", dirName, item->d_name);
			pthread_t tid1;
			printf("%d\n", tid1);
			
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
					
				pthread_create(&tid2, NULL, sort, pathway);
			}
		}
	}
	
	pthread_join(tid2, NULL);
	printf("I cant believe im here\n");
}


csvfile finalSort() {
	if(csvlist == NULL)
	{
		exit(0);
	}
	else if ((*csvlist).next == NULL)
	{
		return (*csvlist).data;
	}
	else
	{
		int counter = 0;
		LL *current = csvlist;
		while(current != NULL)
		{
			counter = counter + 1;
			current = (*current).next;
		}
		
		int arraySizes[counter];
		int totalSize = 0;
		counter = 0;
		
		current = csvlist;
		while(current != NULL)
		{
			arraySizes[counter] = (*current).data.numRows;
			totalSize = totalSize + arraySizes[counter];
			current = (*current).next;
			counter++;
		}
		
		current = csvlist;
		record *array = (record*)malloc(sizeof(record)*counter);
		
		counter = 0;
		int sizeCounter = 0;
		while(current != NULL)
		{
			memcpy(array + sizeCounter, (*current).data.table, arraySizes[counter] * sizeof(record));
			sizeCounter = sizeCounter + arraySizes[counter];
			current = (*current).next;
			counter++;
		}
		
		int columnIndex = getColumnIndex(array, (*csvlist).data.table[0].numColumns, sortParam);
		if (columnIndex < 0) 
		{
			exit(EXIT_FAILURE);
		} 
		else 
		{
			// This is where the actual sorting happens
			mergeSort(array, columnIndex, sizeCounter - 1);
		}
		
		csvfile csv;
		csv.table = array;
		csv.numRows = totalSize;
		
		return csv;
	}
}


void makeCSV(record *arr, int numRecords, int numColumns) {
	printf("pls not my butthole\n");
	if (opendir(output) == NULL) 
	{
		mkdir(output, S_IRWXU);
	}
	
	FILE *sortedFile;
	
	int flag = 0;
	if (output[strlen(output) - 1] != '/')
		flag = 1;
	char* s = (char*)malloc((16 + 4)*sizeof(char) + strlen(sortParam)*sizeof(char) + flag*sizeof(char));
	s = "";
	if(flag)
		strcat(s, "/");
	strcat(s, "AllFiles-sorted-");
	strcat(s, sortParam);
	strcat(s, ".csv");
	
	int totalLength = strlen(output) + strlen(s) + flag + 1;
	char sortedFileName[totalLength];
	
	snprintf(sortedFileName, totalLength + 1, "%s%s", output, s);
	sortedFile = fopen(sortedFileName, "w");
	
	int i, j;
	for(i = 0; i < numRecords; i++) 
	{ 
		for(j = 0; j < numColumns; j++) 
		{
			if(j != (numColumns - 1))
			{
				fprintf(sortedFile, "%s,", arr[i].line[j]);
			}
			else
			{
				fprintf(sortedFile, "%s", arr[i].line[j]);
			}
		}
		fprintf(sortedFile, "\n");
	}

}

// ALREADY ADJUSTED FOR PROJECT 2
int main(int argc, char **argv) {
	
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
	
	if (colCheck == 0)
	{
		printf("Error: The -c flag must always be present.\n");
		exit(EXIT_FAILURE);
	}
	
	sortParam = column;
	
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
	
	if(outCheck == 0) 
	{
		strcpy(output, ".");
	}

	csvfile sortedRecords = finalSort();
	printf("nigga we almost there\n");
	//makeCSV(sortedRecords.table, sortedRecords.numRows, csvlist->data.table[0].numColumns);
	/////////////////
	printf("pls not my butthole\n");
	if (opendir(output) == NULL) 
	{
		mkdir(output, S_IRWXU);
	}
	
	FILE *sortedFile;
	
	int flag = 0;
	
	if (output[strlen(output) - 1] != '/') {
		flag = 1;
	}
	
	if(flag == 1) {
		strcat(output, "/");
	}
	
	int totalLength = strlen(output) + strlen("AllFiles-sorted-") + strlen(sortParam) + strlen(".csv") + 1;
	char sortedFileName[totalLength];
	
	snprintf(sortedFileName, totalLength + 1, "%s%s%s%s", output, "AllFiles-sorted-", sortParam, ".csv");
	sortedFile = fopen(sortedFileName, "w");
	
	int i, j;
	for(i = 0; i < sortedRecords.numRows; i++) 
	{ 
		for(j = 0; j < csvlist->data.table[0].numColumns; j++) 
		{
			if(j != (csvlist->data.table[0].numColumns - 1))
			{
				fprintf(sortedFile, "%s,", sortedRecords.table[i].line[j]);
			}
			else
			{
				fprintf(sortedFile, "%s", sortedRecords.table[i].line[j]);
			}
		}
		fprintf(sortedFile, "\n");
	}
	////////////////////////////////////////////////////////////
	printf("%s", (*csvlist).data.table[0].line[0]);	
	printf("\nNumber of threads: %d\n\n", processCounter);
	return 0;
}