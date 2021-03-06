// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"sorter.h"

int checkType (char *str);

void lineSwap(char **str1, char **str2, int numColumns);

void mergeRecord(record *arr, int columnIndex, int low, int mid, int high, int type);

void mergeSortRecord(record *arr, int columnIndex, int low, int high, int type);

void mergeSort(record *arr, int columnIndex, int numRecords);

void addQuotes(record *arr, int numRecords);
