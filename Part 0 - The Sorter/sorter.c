// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"sorter.h"
#include"mergesort.h"

int main(int argc, char **argv) {
    printf("argc = %d\n", argc);
	printf("argv[argc] = %s\n", argv[argc]);
	if (argc != 3) {
        printf("Error: Need additional parameters\nDo not forget to use -c and specify column to sort on\n");
		exit(EXIT_FAILURE);
    }
	if (strcmp(argv[1], "-c") != 0) {
		printf("Error: Incorrect first parameter given\nPlease use -c as the first parameter");
		exit(EXIT_FAILURE);
	}
	char *line = NULL;
	size_t len = 0;
	ssize_t nread; // getline must return a variable of ssize_t containing the number of characters read
	int count = 0; // Number of lines read by getline
	int numRecords = 0;
	record *arr = (record)malloc(sizeof(record));
	while ((nread = getline(&line, &len, stdin) != -1)) {
		if (count == 0) {
			count++;
			continue;
		}
		numRecords++;
		arr = (record)realloc(numRecords * sizeof(record));
		char *token, *end = line;
		int tokenCount = 1;
		while ((token = strsep(&end, ",") != NULL) {
			int i;
			printf("Token %d: %s", tokenCount, token);
			for (i = 0; i < 27; i++) {
				switch (i) {
					case 0 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
						break;
					case 1 :
						arr[numRecords - 1]->director_name = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->director_name, token);
						break;
                    			case 2 :
						arr[numRecords - 1]->num_critic_for_reviews = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->num_critic_for_reviews, token);
                        			break;
                    			case 3 :
						arr[numRecords - 1]->duration = (int)malloc(strlen(token) * sizeof(int));
						arr[numRecords - 1]->duration = atoi(token);
                       				break;
                    case 4 :
						arr[numRecords - 1]->director_fb_likes = (int)malloc(sizeof(int));
						arr[numRecords - 1]->director_fb_likes = atoi(token);
                        break;
                    case 5 :
						arr[numRecords - 1]->actor_3_fb_likes = (int)malloc(sizeof(int));
						arr[numRecords - 1]->actor_3_fb_likes = atoi(token);
                        break;
                    case 6 :
						arr[numRecords - 1]->actor_2_name = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->actor_2_name, token);
                        break;
                    case 7 :
						arr[numRecords - 1]->actor_1_fb_likes = (int)malloc(sizeof(int));
						arr[numRecords - 1]->actor_1_fb_likes = atoi(token);
                        break;
                    case 8 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 9 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
						break;
					case 10 :
                        arr[numRecords - 1]->director_name = (char *)malloc(strlen(token) * sizeof(char *) + 1);
                        strcpy(arr[numRecords - 1]->director_name, token);
						break;
                    case 11 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 12 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 13 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 14 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 16 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 17 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 18 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 19 :
                        arr[numRecords - 1]->director_name = (char *)malloc(strlen(token) * sizeof(char *) + 1);
                        strcpy(arr[numRecords - 1]->director_name, token);
						break;
                    case 20 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 21 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 22 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 23 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 24 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 25 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
                    case 26 :
						arr[numRecords - 1]->color = (char *)malloc(strlen(token) * sizeof(char *) + 1);
						strcpy(arr[numRecords - 1]->color, token);
                        break;
					default :
						break;
				} 
			}
			tokenCount++;
		}
		count++;
	}
    exit(EXIT_SUCCESS);
}

