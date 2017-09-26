#include"sorter.h"

// Merge left and right sides into one array
// Used in the mergeSort method
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

// Recursive method of performing mergeSort on an array of ints
void mergeSortInt(int *arr, int size) {
    int *left, *right, i;
    if (size < 2) {
        return;
    }
    int mid = size / 2;
    int sizeLeft = mid;
    int sizeRight = size - mid;
    left = (int*) malloc (sizeLeft * sizeof(int));
    right = (int*) malloc (sizeRight * sizeof(int));;
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

int main() {
    int i;
	// Declare and initialze array of integers to be sorted
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
    return 0;
}
