// Authors: Andrew Hernandez & Michael Belmont
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

// Sorts a CSV given the file pointer, sort parameter, and the name of the file
// Since filename can't be extracted from file pointer, it needs to be passed as its own argument
// Assume that the file has already been checked prior to calling sort() to make sure it is a CSV
void sort(FILE *fp, char *sortParam, char *fileName){
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
				printf("Error: Sort parameter does not exist in the given file\n");
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
	int totalLength = strlen(base) + strlen("-sorted-") + strlen(sortParam) + strlen(extension) + 1;
	char sortedFileName[totalLength];
	// Create the sorted file name (aka <csv_name>-sorted-<sort_parameter>.csv)
	snprintf(sortedFileName, totalLength, "%s-sorted-%s%s", base, sortParam, extension);
	sortedFile = fopen(sortedFileName, "w");
	for(i = 0; i < numRecords; i++) { // i < numRecords
		for(j = 0; j < numColumns; j++) {
			if(j != (numColumns - 1)){
				fprintf(sortedFile, "%s,", arr[i].line[j]);}else{fprintf(sortedFile, "%s", arr[i].line[j]);
			}
		}
		fprintf(sortedFile, "\n");
	}	
}

/*
 We need to account for:
   ./sorter -c  movie_title 
   ./sorter -c  movie_title -d thisdir
   ./sorter -c  movie_title -o thatdir 
   ./sorter -c  movie_title -d thisdir -o thatdir 
*/
int main(int argc, char **argv) {
	//printf("argc = %d\n", argc);
	//printf("argv[argc] = %s\n", argv[argc]);
	if (argc < 3) {
        printf("Error: Need additional parameters\nDo not forget to use -c and specify column to sort on\n");
		exit(EXIT_FAILURE);
	}
	if (argc > 7) {
		printf("Error: Too many parameters\nExample parameters: -c <columnName> -d <directoryName> -o <outputDirectoryName>\nNote: -d <directoryName> and -o <outputDirectoryName> are optional\n");
		exit(EXIT_FAILURE);
	}
	// The -c flag must always be present
	if (strcmp(argv[1], "-c") != 0) {
		printf("Error: Incorrect first parameter given\nPlease use -c as the first parameter");
		exit(EXIT_FAILURE);
	}
	// I COMMENTED ALL THIS ERROR CHECKING OUT CUZ I THINK I DID IT WRONG
	// MIGHT WANNA LOOK AT IT LATER - Andrew
	// Since -d and -o are optional, either can be the second flag
	/*if (argv[3] != NULL && (strcmp(argv[3], "-d") != 0 || strcmp(argv[3], "-o") != 0)) {
		printf("Error: Incorrect flag given\nPlease use -d or -o as the second flag");
		exit(EXIT_FAILURE);
	}*/
	// If there are 3 flags, the third one is -o
	/*if (argv[5] != NULL && strcmp(argv[5], "-o") != 0) {
		printf("Error: Incorrect flag given\nPlease use -o as the third flag");
		exit(EXIT_FAILURE);
	}*/
	// Call sort()
	FILE *fp;
	char *fileName;
	fileName = "movie_metadata.csv";
	fp = fopen(fileName, "r");
	sort(fp, argv[2], fileName);
	fclose(fp);
	exit(EXIT_SUCCESS);
}


