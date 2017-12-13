// Authors: Andrew Hernandez & Michael Belmont
// CS214: Systems Programming Fall 2017
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"sorter_server.h"

//Prototypes for the functions we're using in mergesort.c - DOCUMENTATION IS IN THAT FILE
void merge(record ** front, int sortingBy);
record * sorted(record * first, record * second, int sortingBy);
void split(record * source, record ** first, record ** second);
