#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv){
	int numFiles = atoi(argv[1]);
	int i;
	char c;
	FILE** filePointers;
	FILE* copyFile;
	DIR* dirPointer = opendir(".");
	struct dirent* currentObject;
	
	if(dirPointer == NULL){
		printf("Directory pointer failed to locate path.\n");
		return 0;
	}
	
	while((currentObject = readdir(dirPointer)) != NULL){
		if(strstr(currentObject->d_name, "-sorted") != NULL){
			if((remove(currentObject->d_name) != 0)){
				printf("Error deleting file.\n");
			}
		}
	}
	closedir(dirPointer);
	free(currentObject);
	
	
	copyFile = fopen(argv[2], "r");
	
	if(copyFile == NULL){
		printf("Error opening copy file.\n");
		return 0;
	}
	
	filePointers = (FILE**) malloc(sizeof(FILE*) * numFiles);
	
	for(i = 0; i < numFiles; i++){
		char fileName[20];
		sprintf(fileName, "testfile%d.csv", i);
		filePointers[i] = fopen(fileName, "w");
		
		if(filePointers[i] == NULL){
			printf("Error opening file to write to.\n");
			return 0;
		}
		
		c = fgetc(copyFile);
		
		while(c != EOF){
			fputc(c, filePointers[i]);
			c = fgetc(copyFile);
		}
		rewind(copyFile);
	}

	return 0;
}
