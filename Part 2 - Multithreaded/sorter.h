// Authors: Andrew Hernandez & Michael Belmont
// CS214: Systems Programming Fall 2017
#ifndef SORTER_H_
#define SORTER_H_

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pthread.h>
#include<unistd.h>


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

typedef struct csvfile {
	record *table;
	int numRows;
} csvfile;

typedef struct LL {
	struct LL *next;
	csvfile data;
} LL;

void trim(char *str);

int checkForDuplicates(char **str, int numStrings);

int getColumnIndex(record *arr, int numColumns, char *columnName);

#endif // SORTER_H_
