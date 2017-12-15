/*****
*
*	Define structures and function prototypes for your sorter
*
*
*
******/

//Suggestion: define a struct that mirrors a record (row) of the data set


//Suggestion: prototype a mergesort function


//The movies structure will be the basis for our Linked List structure in the assigment. The structure includes 28 fields modeled after the categories documented in the CSV. There is also a movies* variable that will serve as a pointer to the next node in the Linked List.

#ifndef SORTER_H
#define SORTER_H
	typedef struct movies
	{
		char* color;
		char* director_name;
		long num_critic_for_reviews;
		long duration;
		long director_facebook_likes;
		long actor_3_facebook_likes;
		char* actor_2_name;
		long actor_1_facebook_likes;
		long gross;
		char* genres;
		char* actor_1_name;
		char* movie_title;
		long num_voted_users;
		long cast_total_facebook_likes;
		char* actor_3_name;
		long facenumber_in_poster;
		char* plot_keywords;
		char* movie_imdb_link;
		long num_user_for_reviews;
		char* language;
		char* country;
		char* content_rating;
		long budget;
		long title_year;
		long actor_2_facebook_likes;
		double imdb_score;
		double aspect_ratio;
		long movie_facebook_likes;
		int quotes;

		struct movies * next;
	}movie; /* Movie nodes with all the information needed */
	
	typedef struct _threadIds{
		pthread_t tid;
		
		struct _threadIds * next;
	}threadIds;
	
	
	//Struct containing arguments for sortFile
	typedef struct sortFileArg {
		char* fileName;
		char** argv;
		int sortingBy;
		char* path;
		int colLoc;
		int * runningThreadCount;
	}sortFileArgs;
	
	typedef struct traverseArg {
		char* path;
		char** argv;
		int sortingBy;
		int existsNewOutDir;
		char* outPath;
		int * totalThreads;
		int colLoc;
		int * runningThreadCount;
	}traverseDirectoryArgs;
	
	//sorter.c helper method prototypes
	
	//movie* freePtr(movie* node);
	//movie* createNode(char** catToked, int hasQuotes);
	//char* spaceTrim(char* string);
	int sortByCategory(char* sortColumnName);
	//void * sortFile(void * args);
	void * traverseDirectory(void * args);
	//void printNodes(movie * currPtr, FILE* outputFile);


#endif
