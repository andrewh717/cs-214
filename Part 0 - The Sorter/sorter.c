// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"sorter.h"
#include"mergesort.h"

extern int errno;

int main(int argc, char **argv) {
    int errnum;
    if (argc < 2) {
        errnum = errno;
        fprintf(stderr, "Error: %s\n", strerror(errnum));
    }
    return 0;
}