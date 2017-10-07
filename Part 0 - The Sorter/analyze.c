// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"sorter.h"
#include"mergesort.h"

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

// this method will average out the earnings for two sets:
// movies with less than 4 genres
// movies with 4 or more genres
void genreMine (record *arr, int numRecords){
	int numColumns = arr[0].numColumns;
	int less= 0; 
	int more= 0;

	double lessMoney= 0;
	double moreMoney= 0;

	int tempGen= 0;
	double tempGross= 0;

	int i, k, j, n; //counters

	for (i= 0; i< numRecords; i++){
			tempGen= 0;
			tempGross= 0;
			n= k-1;
		for (k= 0; k< numColumns; k++){
			
			if (strstr(arr[i].line[k], "|") != NULL){ //FOUND GENRE @@@@@
				
				char *tempG= malloc(strlen(arr[i].line[k]) * sizeof(char) + 1);
				strcpy(tempG, arr[i].line[k]);	
				
				for (j= 0; j< strlen(tempG); j++){
					if (tempG[j]== '|'){
						tempGen++;
						}
					}
					
					if (tempGen <4){	//1-3 genres
						less++;
						tempGross= strtod((arr[i].line[n]), NULL);
						lessMoney= tempGross+ lessMoney;
						}

					else{				// >3 genres
						more++;
						tempGross= strtod((arr[i].line[n]), NULL);
						moreMoney= tempGross+ moreMoney;
					}
				}
			}
		}
		lessMoney= lessMoney/less;
		moreMoney= moreMoney/more;
		printf("Average dollar amount made for movies with less than 4 genres \n");
		printf("%f", lessMoney);
		printf("\n");
		printf("Average dollar amount made for movies with 4 or more genres \n");
		printf("%f", moreMoney);
	}	
//genreMine ends


int main(int argc, char **argv) {
	//printf("argc = %d\n", argc);
	//printf("argv[argc] = %s\n", argv[argc]);
	if (argc != 3) {
        printf("Error: Need additional parameters\nDo not forget to use -c and specify column to sort on\n");
		exit(EXIT_FAILURE);
	}
	if (strcmp(argv[1], "-c") != 0) {
		printf("Error: Incorrect first parameter given\nPlease use -c as the first parameter");
		exit(EXIT_FAILURE);
	}
	int i, numRecords, numColumns, currentToken;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread; // getline must return a variable of ssize_t containing the number of characters read
	numRecords = 0;
	record *arr = malloc(sizeof(record));
	while ((nread = getline(&line, &len, stdin) != -1)) {
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
				if (strcmp(arr[0].line[i], argv[2]) == 0) {
					paramFound = 1;
				}
			}
			// If the sort parameter is not found, exit the program
			if (paramFound == 0) {
				printf("Error: Sort parameter does not exist in the given file\n");
				exit(EXIT_FAILURE);
			}
		}
		char *token, *end = line;	
		//char *temp = strstr(line, "\"");
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
	
	int columnIndex = getColumnIndex(arr, numColumns, argv[2]);
	if (columnIndex < 0) {
		exit(EXIT_FAILURE);
	} else {
		mergeSort(arr, columnIndex, numRecords - 1);
	}
	
	genreMine(arr, numRecords);

	exit(EXIT_SUCCESS);
}


