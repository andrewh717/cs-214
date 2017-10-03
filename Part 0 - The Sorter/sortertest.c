// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"sortertest.h"
#include"mergesort.h"

void trim(char *str) {
	int i;
	int begin = 0;
	int end = strlen(str) - 1;

	while (isspace((unsigned char) str[begin]))
		begin++;

	while ((end >= begin) && isspace((unsigned char) str[end]))
		end--;

	// Shift all characters back to the start of the string array.
	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];

	str[i - begin] = '\0'; // Null terminate string.
	
	// Attempt at removing the weird A at the end of strings
	/*if(isprint((int)str[i - begin - 1]) == 0) {
		for (i = begin; i <= end; i++)
			str[i - begin -1] = str[i];
	}*/
}

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
	int i, numRecords;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread; // getline must return a variable of ssize_t containing the number of characters read
	numRecords = 0;
	record *arr = malloc(sizeof(record));
	while ((nread = getline(&line, &len, stdin) != -1)) {
		arr = realloc(arr, (numRecords + 1) * sizeof(record));
		char *token, *end = line;	
		char *temp = strstr(line, "\"");
		int currentToken = 0;
		arr[numRecords].line = malloc(sizeof(char **));
		while (end != NULL) {
			int containsQuote = 0;
			if ((temp != NULL) && (currentToken == 11)) {
				containsQuote = 1;
			}
			if (containsQuote != 1) {
				token = strsep(&end, ",");	
			} else if (currentToken == 11) {
				end++;
				token = strsep(&end, "\"");
				end++;
			}
			if (currentToken > 0){
				arr[numRecords].line = realloc(arr[numRecords].line, (currentToken + 1) * sizeof(char **));
			}
			trim(token);
			arr[numRecords].line[currentToken] = malloc(strlen(token) * sizeof(char) + 1);
			strcpy(arr[numRecords].line[currentToken], token);
			currentToken++;
		}
		numRecords++;
	}
	//char *sortBy;
	int j;
	for(i = 0; i < numRecords; i++) {
		for(j = 0; j < 28; j++) {
			if(j != 27){printf("%s,", arr[i].line[j]);}else{printf("%s", arr[i].line[j]);}
		}
		printf("\n");
		// Need to do the actual sorting of records... Not sure exactly how to.
		// Might need to adapt the code of mergesort.c
		// I wrote methods to sort arrays of ints and arrays of strings,
		// so basically what we need to do is adapt it so that we reorder our array of records
		// in the correct sorted order.
	}
	exit(EXIT_SUCCESS);
}


