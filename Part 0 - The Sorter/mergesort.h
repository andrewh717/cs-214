// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#ifndef MERGESORT_H_
#define MERGESORT_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void mergeInt(int *left, int *right, int *arr, int sizeLeft, int sizeRight);

void mergeSortInt(int *arr, int size);

void mergeString(char **arr, int low, int mid, int high);

void mergeSortString(char **arr, int low, int high);

#endif // MERGESORT_H_