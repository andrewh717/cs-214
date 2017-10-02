// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include"sorter.h"
#include"mergesort.h"

size_t trim(char *out, size_t len, const char *str)
{
  if(len == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
  {
    *out = 0;
    return 1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < len-1 ? (end - str) : len-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size - 1] = 0;
  return out_size;
}

int main(int argc, char **argv) {
    //printf("argc = %d\n", argc);
	//printf("argv[argc] = %s\n", argv[argc]);
	if (argc != 3) {
        printf("Error: Need additional parameters\nDo not forget to use -c and specify column to sort on\n");
		exit(EXIT_FAILURE);
    }
	if (strcmp(argv[1], "-c") != 0) {
		printf("Error: Incorrect first parameter given\nPlease use -c as the first parameter");
		exit(EXIT_FAILURE);
	}
	int i, count, numRecords;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread; // getline must return a variable of ssize_t containing the number of characters read
	count = 0; // Number of lines read by getline
	numRecords = 0;
	record *arr = malloc(sizeof(record));
	while ((nread = getline(&line, &len, stdin) != -1)) {
		if (count == 0) {
			count++;
			continue;
		}
		numRecords++;
		arr = realloc(arr, numRecords * sizeof(record));
		char *token, *end = line;
		//int containsQuote = 0;
		int currentToken = 0;
		//int tokenIsNull = 0;
		char *temp = strstr(line, "\"");
		while (end != NULL) {
			int containsQuote = 0;
			if((temp != NULL) && (currentToken == 11)) {
				containsQuote = 1;
				//if(i == 1) {
				//printf("%s -- ", end);
				//printf("quote found and currentToken = %d\n", currentToken);
				//printf("line #%d\n", count);
				//exit(EXIT_SUCCESS);
				//}
			}
			if(containsQuote != 1) {
				token = strsep(&end, ",");	
			}
			switch (currentToken) {
				case 0 :
					trim(token, strlen(token), (const char *)token);
					arr[numRecords - 1].color = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].color, token);
					break;
				case 1 :
					arr[numRecords - 1].director_name = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].director_name, token);
					break;
                case 2 :
					//arr[numRecords - 1].num_critic_for_reviews = (int)malloc(sizeof(int));
					arr[numRecords - 1].num_critic_for_reviews = atoi(token);
                 	break;
                case 3 :
					//arr[numRecords - 1].duration = malloc(strlen(token) * sizeof(int));
					arr[numRecords - 1].duration = atoi(token);
                   	break;
                case 4 :
					//arr[numRecords - 1].director_fb_likes = malloc(sizeof(int));
					arr[numRecords - 1].director_fb_likes = atoi(token);
                    break;
                case 5 :
					//arr[numRecords - 1].actor_3_fb_likes = malloc(sizeof(int));
					arr[numRecords - 1].actor_3_fb_likes = atoi(token);
                    break;
                case 6 :
					arr[numRecords - 1].actor_2_name = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].actor_2_name, token);
                    break;
                case 7 :
					//arr[numRecords - 1].actor_1_fb_likes = malloc(sizeof(int));
					arr[numRecords - 1].actor_1_fb_likes = atoi(token);
                    break;
                case 8 :
					//arr[numRecords - 1].gross = malloc(sizeof(int));
					arr[numRecords - 1].gross = atoi(token);
                    break;
                case 9 :
					arr[numRecords - 1].genres = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].genres, token);
					break;
				case 10 :
                    arr[numRecords - 1].actor_1_name = malloc(strlen(token) * sizeof(char *) + 1);
                    strcpy(arr[numRecords - 1].actor_1_name, token);
					//printf("found case 10");
					//exit(EXIT_SUCCESS);
					break;
                case 11 :
					if (containsQuote != 0) {
						//printf("found case 11 and containsQuote, line: %s\n", end);
						//exit(EXIT_SUCCESS);
						end++;
						token = strsep(&end, "\"");
						end++;
						//printf("quote found!");
						//exit(EXIT_SUCCESS);
					}
					//printf("found case 11");
					//exit(EXIT_SUCCESS);
					trim(token, strlen(token), (const char *)token);
					arr[numRecords - 1].movie_title = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].movie_title, token);
                    break;
                case 12 :
					//arr[numRecords - 1].num_voted_users = malloc(sizeof(int));
					arr[numRecords - 1].num_voted_users = atoi(token);
                    break;
                case 13 :
					//arr[numRecords - 1].cast_total_fb_likes = malloc(sizeof(int));
					arr[numRecords - 1].cast_total_fb_likes = atoi(token);
                    break;
            	case 14 :
					arr[numRecords - 1].actor_3_name = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].actor_3_name, token);
                    break;
                case 15 :
					//arr[numRecords - 1].facenumber_in_poster = malloc(sizeof(int));
					arr[numRecords - 1].facenumber_in_poster = atoi(token);
                    break;
                case 16 :
					arr[numRecords - 1].plot_keywords = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].plot_keywords, token);
                    break;
                case 17 :
					arr[numRecords - 1].movie_imdb_link = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].movie_imdb_link, token);
                    break;
                case 18 :
                    //arr[numRecords - 1].num_user_for_reviews = malloc(sizeof(int));
                    arr[numRecords - 1].num_user_for_reviews = atoi(token);
					break;
                case 19 :
					arr[numRecords - 1].language = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].language, token);
                    break;
                case 20 :
					arr[numRecords - 1].country = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].country, token);
                    break;
                case 21 :
					arr[numRecords - 1].content_rating = malloc(strlen(token) * sizeof(char *) + 1);
					strcpy(arr[numRecords - 1].content_rating, token);
                    break;
                case 22 :
					//arr[numRecords - 1].budget = malloc(sizeof(int));
					arr[numRecords - 1].budget = atoi(token);
                    break;
                case 23 :
					//arr[numRecords - 1].title_year = malloc(strlen(sizeof(int));
					arr[numRecords - 1].title_year = atoi(token);
                    break;
                case 24 :
					//arr[numRecords - 1].actor_2_fb_likes = malloc(sizeof(int));
					arr[numRecords - 1].actor_2_fb_likes = atoi(token);
                    break;
                case 25 :
					//arr[numRecords - 1].imdb_score = malloc(sizeof(double));
					arr[numRecords - 1].imdb_score = atof(token);
                    break;
				case 26 :
					//arr[numRecords - 1].aspect_ratio = malloc(sizeof(double));
					arr[numRecords - 1].aspect_ratio = atof(token);
				case 27 :
					//arr[numRecords - 1].movie_fb_likes = malloc(sizeof(int));
					arr[numRecords - 1].movie_fb_likes = atoi(token);
				default :
					break;
				}
			currentToken++;
			//printf("I am correctly incrementing currentToken!\n");
		}
		count++;
	}
	char *sortBy;
	switch(argv[2]) {
		case "color" :
			sortBy = "string";
			break;
		case "director_name" :
			sortBy = "string";
			break;
		case "num_critic_for_reviews" :
			sortBy = "int";
			break;
		case "duration" :
			sortBy = "int";
			break;
		case "director_facebook_likes" :
			sortBy = "int";
			break;
		case "actor_3_facebook_likes" :
			sortBy = "int";
			break;
		case "actor_2_name" :
			sortBy = "string";
			break;
		case "actor_1_facebook_likes" :
			sortBy = "int";
			break;
		case "gross" :
			sortBy = "int";
			break;
		case "genres" :
			sortBy = "string";
			break;
		case "actor_1_name" :
			sortBy = "string";
			break;
		case "movie_title" :
			sortBy = "string";
			break;
		case "num_voted_users" :
			sortBy = "int";
			break;
		case "cast_total_facebook_likes" :
			sortBy = "int";
			break;
		case "actor_3_name" :
			sortBy = "string";
			break;
		case "facenumber_in_poster" :
			sortBy = "int";
			break;
		case "plot_keywords" :
			sortBy = "string";
			break;
		case "movie_imdb_link" :
			sortBy = "string";
			break;
		case "num_user_for_reviews" :
			sortBy = "int";
			break;
		case "language" :
			sortBy = "string";
			break;
		case "country" :
			sortBy = "string";
			break;
		case "content_rating" :
			sortBy = "string";
			break;
		case "budget" :
			sortBy = "int";
			break;
		case "title_year" :
			sortBy = "int";
			break;
		case "actor_2_facebook_likes" :
			sortBy = "int";
			break;
		case "imdb_score" :
			sortBy = "double";
			break;
		case "aspect_ratio" :
			sortBy = "double";
			break;
		case "movie_facebook_likes" :
			sortBy = "int";
			break;
		default :
			printf("Error: Incorrect column name given\n");
			exit(EXIT_FAILURE);
	}
	for(i = 0; i < numRecords; i++) {
		//printf("%s, %d\n", arr[i].genres, arr[i].num_voted_users);
		// Need to do the actual sorting of records... Not sure exactly how to.
		// Might need to adapt the code of mergesort.c
		// I wrote methods to sort arrays of ints and arrays of strings,
		// so basically what we need to do is adapt it so that we reorder our array of records
		// in the correct sorted order.
	}
    exit(EXIT_SUCCESS);
}


