// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"mergesort.h"

// Method to swap records by swapping their line members
void lineSwap(char **str1, char **str2, int numColumns) {
	int i;
	for (i = 0; i < numColumns; i++) { 
		if (strlen(str1[i]) > strlen(str2[i])) {
			char *temp = malloc(strlen(str1[i]) * sizeof(char) + 1);
			strcpy(temp, str1[i]);
			strcpy(str1[i], str2[i]);
			str2[i] = temp;
		} else {
			char *temp = malloc(strlen(str2[i]) * sizeof(char) + 1);
			strcpy(temp, str2[i]);
			strcpy(str2[i], str1[i]);
			str1[i] = malloc(strlen(temp) * sizeof(char) + 1);
			strcpy(str1[i], temp);
		}
	}
}

// Mergesort for array of records
void mergeRecord(record *arr, int columnIndex, int low, int mid, int high, int type) {
	int i, j, k;
	int sizeLeft = mid - low + 1;
	int sizeRight = high - mid;
	record *left, *right;
	left = malloc(sizeLeft * sizeof(record));
	right = malloc(sizeRight * sizeof(record));
	for (i = 0; i < sizeLeft; i++) {
		left[i].line = malloc(arr[0].numColumns * sizeof(char **));
		for (k = 0; k < arr[0].numColumns; k++) {
			left[i].line[k] = malloc(strlen(arr[low + i].line[k]) * sizeof(char) + 1);
			strcpy(left[i].line[k], arr[low + i].line[k]);
		}
	}
	for (j = 0; j < sizeRight; j++) {
		right[j].line = malloc(arr[0].numColumns * sizeof(char **));
		for (k = 0; k < arr[0].numColumns; k++) {
			right[j].line[k] = malloc(strlen(arr[mid + 1 + j].line[k]) * sizeof(char) + 1);
			strcpy(right[j].line[k], arr[mid + 1 + j].line[k]);
		}
	}
	i = 0; // Initial index of left array
	j = 0; // Initial index of right array
	k = low; // Initial index of merged array
	
    while (i < sizeLeft && j < sizeRight) {
        if (type == 0 && (strcmp(left[i].line[columnIndex], right[j].line[columnIndex]) < 0)) {
			lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
			k++;
        } else if ((type == 1 || 2) && ((strcmp(left[i].line[columnIndex], "") == 0) && (strcmp(right[j].line[columnIndex], "") != 0))) {
			lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
			k++;
			// For some reason, the numeric types that are empty strings are sorted in reverse order of how they appear in the original CSV
		} else if ((type == 1 || 2) && ((strcmp(left[i].line[columnIndex], "") != 0) && (strcmp(right[j].line[columnIndex], "") == 0))) {
			lineSwap(arr[k].line, right[j++].line, arr[k].numColumns);
			k++;
		} else if ((type == 1 || 2) && ((strcmp(left[i].line[columnIndex], "") == 0) && (strcmp(right[j].line[columnIndex], "") == 0))) {
			lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
			k++;
		} else if (type == 1 && (atoi(left[i].line[columnIndex]) < atoi(right[j].line[columnIndex]))) {
			lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
			k++;
        } else if (type == 2 && (atof(left[i].line[columnIndex]) < atof(right[j].line[columnIndex]))) {
			lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
			k++;
        } else {
        	lineSwap(arr[k].line, right[j++].line, arr[k].numColumns);
        	k++;
        }
    }
    while (i < sizeLeft) {
        lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
		k++;
    }
    while (j < sizeRight) {
        lineSwap(arr[k].line, right[j++].line, arr[k].numColumns);
        k++;
    }
}

void mergeSortRecord(record *arr, int columnIndex, int low, int high, int type) {
    if (low < high) {
    	int mid = low + (high - low) / 2;
    	mergeSortRecord(arr, columnIndex, low, mid, type);
    	mergeSortRecord(arr, columnIndex, mid + 1, high, type);
    	mergeRecord(arr, columnIndex, low, mid, high, type);
	}
}

int checkType (char *str) {
	int i, isInt;
	isInt = 0;
	for (i = 0; i < strlen(str); i++) {
		if (isdigit(str[i]) != 0) {
			continue;
		} else {
			isInt = 1;
			break;
		}
	}
	// A string is a floating point number if the first and last characters are digits, 
	// and the string contains a period (aka decimal point)
	char *temp = strstr(str, ".");
	if ((isdigit(str[0]) != 0) && (isdigit(str[strlen(str)-1]) != 0) && (temp != NULL)) {
		return 2; // Treat the string as a double
	}
	if (isInt == 0) {
		return 1; // Treat the string as an int
	}
	return 0; // If it's not an int or a double, treat it as a string
}

void addQuotes (record *arr, int numRecords) {
	int i, j;
	for (i = 0; i < numRecords; i++) {
		for (j = 0; j < arr[0].numColumns; j++) {
			if (strstr(arr[i].line[j], ",") != NULL) {
				// Allocate enough room
				char *temp = malloc(strlen(arr[i].line[j]) * sizeof(char) + 1);
				strcpy(temp, arr[i].line[j]); 
				arr[i].line[j] = realloc(arr[i].line[j], strlen(arr[i].line[j]) * sizeof(char) + 3);
				snprintf(arr[i].line[j], strlen(arr[i].line[j]) + 3, "\"%s\"", temp);	
			}
		}
	}
}

void mergeSort(record *arr, int columnIndex, int numRecords) {
	int type;
	type = checkType(arr[1].line[columnIndex]);
	// If type == 0, the token is a string
	// If type == 1, the token is an int
	// If type == 2, the token is a double
	mergeSortRecord(arr, columnIndex, 1, numRecords, type);
	addQuotes(arr, numRecords);
	return;
}
