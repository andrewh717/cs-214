// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"mergesort.h"

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
void mergeString(char **arr, int low, int mid, int high) {
	int i, j, k;
	int sizeLeft = mid - low + 1;
	int sizeRight = high - mid;
	char **left = (char **)malloc(sizeLeft * sizeof(char *));
	char **right = (char **)malloc(sizeRight * sizeof(char *));
	for (i = 0; i < sizeLeft; i++) {
		left[i] = (char *)malloc(strlen(arr[low + i]) + 1);
		strcpy(left[i], arr[low + i]);
	}
	for (j = 0; j < sizeRight; j++) {
		right[j] = (char *)malloc(strlen(arr[mid + 1 + j]) + 1);
		strcpy(right[j], arr[mid + 1 + j]);
	}
	i = 0; // Initial index of left array
	j = 0; // Initial index of right array
	k = low; // Initial index of merged array
    while (i < sizeLeft && j < sizeRight) {
        if (strcmp(left[i], right[j]) < 0) {
            strcpy(arr[k++], left[i++]);
        } else {
            strcpy(arr[k++], right[j++]);
        }
    }
    while (i < sizeLeft) {
        strcpy(arr[k++], left[i++]);
    }
    while (j < sizeRight) {
        strcpy(arr[k++], right[j++]);
    }
}

void mergeSortString(char **arr, int low, int high) {
    if (low < high) {
    	int mid = low + (high - low) / 2;
    	mergeSortString(arr, low, mid);
    	mergeSortString(arr, mid + 1, high);
    	mergeString(arr, low, mid, high);
	}
}

// For testing purposes
int main() {
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
}
