// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#ifndef SORTER_H_
#define SORTER_H_

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
/*****
*
*	Define structures and function prototypes for your sorter
*
*
*
******/

typedef struct record {
	char **line;
	int numColumns;
} record;

void trim(char *str);

int checkForDuplicates(char **str, int numStrings);

int getColumnIndex(record *arr, int numColumns, char *columnName);

#endif // SORTER_H_
