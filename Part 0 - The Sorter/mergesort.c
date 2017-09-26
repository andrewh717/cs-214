#include"sorter.h"

// Recursive method of performing mergeSort on an array of ints
void mergeSortInt(int *arr) {
    int *left, *right;
    int size = sizeof(arr)/sizeof(a[0]);
    if (size < 2) {
        return;
    }
    int mid = size / 2;
    int sizeLeft = mid;
    int sizeRight = size - mid;
    left = (int*) malloc (sizeLeft * sizeof(int));
    right = (int*) malloc (sizeRight * sizeof(int));;
    for (int i = 0; i < mid; i++) {
        left[i] = arr[i];
    }
    for (int i = mid; i < size; i++) {
        right[i - mid] = arr[i];
    }
    mergeSortInt(left);
    mergeSortInt(right);
    mergeInt(left, right, arr);
    free(left);
    free(right);
}

// Merge left and right sides into one array
// Used in the mergeSort method
void mergeInt(int left[], int right[], int arr[]) {
    int sizeLeft = sizeof(left)/sizeof(left[0]);
    int sizeRight = sizeof(right)/sizeof(right[0]);
    int i = 0, j = 0, k = 0;
    while (i < sizeLeft && j < sizeRight) {
        if (left[i] <= right[j]) {
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

int main() {
    int a[] = {1,5,3,6,88,11,2};
    printf("Original:\n")
    for(int i = 0; i < (sizeof(a)/sizeof(a[0])); i++) {
        printf("%d", a[i]);
    }
    mergeSortInt(a);
    printf("\nSorted:\n");
    for(int i = 0; i < (sizeof(a)/sizeof(a[0])); i++) {
        printf("%d", a[i]);
    }
    return 0;
}