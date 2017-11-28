// Authors: Andrew Hernandez & Michael Belmont
// CS214: Systems Programming Fall 2017
#ifndef SORTER_H_
#define SORTER_H_

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>

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

void sort(FILE *fp, char *sortParam, char *fileName, char *outputDir);

#endif // SORTER_H_
