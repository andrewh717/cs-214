// Authors: Andrew Hernandez & Michael Belmont
// CS214: Systems Programming Fall 2017
#ifndef SORTER_SERVER_H_
#define SORTER_SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pthread.h>
#include<unistd.h>

/* Socket Stuff */
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct record {
	char **line;
	struct record *next;
} record;


typedef struct csvfile {
	record *table;
	int numRows;
} csvfile;

typedef struct LL {
	struct LL *next;
	csvfile data;
} LL;

typedef struct tid_type
{
	pthread_t tid;
	int isFree;
	int socketfd;
}tid_type;

void trim(char *str);

int checkForDuplicates(char **str, int numStrings);

int getColumnIndex(record *arr, int numColumns, char *columnName);

int get_tid();

#endif // SORTER_SERVER_H_
