// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"sortertest.h"

int checkType (char *str);

void lineSwap(char **str1, char **str2, int numColumns);

void mergeInt(int *left, int *right, int *arr, int sizeLeft, int sizeRight);

void mergeSortInt(int *arr, int size);

void mergeString(record *arr, int columnIndex, int low, int mid, int high);

void mergeSortString(record *arr, int columnIndex, int low, int high);

void mergeSort(record *arr, int columnIndex, int numRecords);
