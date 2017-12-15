// CS214: Systems Programming Fall 2017
#ifndef SORTER_SERVER_H_
#define SORTER_SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pthread.h>
#include<unistd.h>
#include<stdint.h>

/* Socket Stuff */
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

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

typedef struct tid_node {
	pthread_t tid;
	struct tid_node *next;
}tid_node;

void trim(char *str);

int checkForDuplicates(char **str, int numStrings);

int getColumnIndex(record *arr, int numColumns, char *columnName);

int get_tid();

record * freeRecord(record *ptr);

//Prototypes for the functions we're using in mergesort.c - DOCUMENTATION IS IN THAT FILE
void merge(record ** front, int sortingBy);
record * sorted(record * first, record * second, int sortingBy);
void split(record * source, record ** first, record ** second);

#endif // SORTER_SERVER_H_
