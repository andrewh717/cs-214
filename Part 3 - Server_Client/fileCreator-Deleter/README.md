# fileCreator-Deleter
This code will create a number of copies of a csv file given an integer input and a path to the file to copy. Each copy will be named "testfile%d.csv" where the %d will be replaced by the the appropriate number associated to the number of copies already made.

Input syntax: ./fileCreator <# of copies to create> <path to file to copy>

Ex: To create 2 copies of a file...

Input
./fileCreator 2 ./fileToCopy

Output:
testfile0.csv
testfile1.csv
