// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"mergesort.h"

void lineSwap(char **str1, char **str2, int numColumns) {
	int i;
	for (i = 0; i < numColumns; i++) { 
		if (strlen(str1[i]) > strlen(str2[i])) {
			char *temp = malloc(strlen(str1[i]) * sizeof(char) + 1);
			strcpy(temp, str1[i]);
			strcpy(str1[i], str2[i]);
			//strcpy(str2[i], temp);
			str2[i] = temp;
			// NEED TO SUCCESSFULLY SWAP THE STRINGS
		} else {
			char *temp = malloc(strlen(str2[i]) * sizeof(char) + 1);
			strcpy(temp, str2[i]);
			strcpy(str2[i], str1[i]);
			//strcpy(str1[i], temp);
			str1[i] = malloc(strlen(temp) * sizeof(char) + 1);
			strcpy(str1[i], temp);
		}
	}
}

// Mergesort for array of integers
void mergeInt(int *left, int *right, int *arr, int sizeLeft, int sizeRight) {
    int i = 0, j = 0, k = 0;
    while (i < sizeLeft && j < sizeRight) {
        if (left[i] < right[j]) {
            // Doing the increments in the array-reference makes this more compact
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }
    while (i < sizeLeft) {
        arr[k++] = left[i++];
    }
    while (j < sizeRight) {
        arr[k++] = right[j++];
    }
}

void mergeSortInt(int *arr, int size) {
    int *left, *right, i;
    if (size < 2) {
        return;
    }
    int mid = size / 2;
    int sizeLeft = mid;
    int sizeRight = size - mid;
    left = (int*) malloc(sizeLeft * sizeof(int));
    right = (int*) malloc(sizeRight * sizeof(int));;
    for (i = 0; i < mid; i++) {
        left[i] = arr[i];
    }
    for (i = mid; i < size; i++) {
        right[i - mid] = arr[i];
    }
    mergeSortInt(left, sizeLeft);
    mergeSortInt(right, sizeRight);
    mergeInt(left, right, arr, sizeLeft, sizeRight);
    free(left);
    free(right);
}

// Mergesort for array of strings
void mergeString(record *arr, int columnIndex, int low, int mid, int high) {
	int i, j, k;
	int sizeLeft = mid - low + 1;
	int sizeRight = high - mid;
	record *left, *right;
	left = malloc(sizeLeft * sizeof(record));
	//left.numColumns = arr.numColumns;
	right = malloc(sizeRight * sizeof(record));
	//right.numColumns = arr.numColumns;
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
        if (strcmp(left[i].line[columnIndex], right[j].line[columnIndex]) < 0) {
			lineSwap(arr[k].line, left[i++].line, arr[k].numColumns);
			k++;
            //strcpy(arr[k++], left[i++]);
            //free(left[i]) and then i++; (would have to remove the post-increment above)
        } else {
        	lineSwap(arr[k].line, right[j++].line, arr[k].numColumns);
        	k++;
            //strcpy(arr[k++], right[j++]);
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

void mergeSortString(record *arr, int columnIndex, int low, int high) {
    if (low < high) {
    	int mid = low + (high - low) / 2;
    	mergeSortString(arr, columnIndex, low, mid);
    	mergeSortString(arr, columnIndex, mid + 1, high);
    	mergeString(arr, columnIndex, low, mid, high);
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

void mergeSort(record *arr, int columnIndex, int numRecords) {
	int type;
	type = checkType(arr[1].line[columnIndex]);
	switch (type) {
		case 0 : // The token is a string
			mergeSortString(arr, columnIndex, 1, numRecords);
			break;
		case 1 : // The token is an int
			//mergeSortInt();
			break;
		case 2 : // The token is a double
			//mergeSortDouble();
			break;
		default :
			break;
	}
	return;
}

// For testing purposes
/*int main() {
    int i;
	// Declare and initialize array of integers to be sorted
    int a[] = {1,5,3,6,88,11,2};
	// Need to get the size of the array now because it needs to be done in scope of the array declaration
	int size = sizeof(a)/sizeof(a[0]);
	// Create a pointer to the array of integers
	int *ptr;
	ptr = a;
    printf("Original:\n");
    for(i = 0; i < (size); i++) {
        printf("%d ", a[i]);
    }
	// Pass the pointer and the size of the array
    mergeSortInt(ptr, size);
    printf("\nSorted:\n");
    for(i = 0; i < (size); i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
    
    // Testing mergeSortString
    int numStrings;
    printf("\nPlease enter the number of strings to be sorted:\n");
    scanf("%d", &numStrings);
    char **strings = (char **) malloc(numStrings * sizeof(char *));
    printf("Enter the strings:\n");
    for(i = 0; i < numStrings; i++){
        strings[i] = malloc(sizeof(char) * 256);
        scanf("%s", strings[i]);
    }
    mergeSortString(strings, 0, numStrings - 1);
    printf("\nHere is the sorted array of strings:\n");
    for(i = 0; i < numStrings; i++){
        if(i == (numStrings - 1)){
            printf("%s", strings[i]);
        } else {
            printf("%s, ", strings[i]);
        }        
    }
 
    return 0;
} */
