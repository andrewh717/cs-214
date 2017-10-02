// Authors: Andrew Hernandez & Malek Saadeh
// CS214: Systems Programming Fall 2017
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

/*****
*
*	Define structures and function prototypes for your sorter
*
*
*
******/

//Suggestion: define a struct that mirrors a record (row) of the data set
typedef struct record {
	char *color;
	char *director_name;
	char *actor_3_name;
	char *actor_2_name;
	char *actor_1_name;
	char *movie_title;
	char *plot_keywords;
	char *movie_imdb_link;
	char *language;
	char *country;
	char *content_rating;
	char *genres;
	int num_critic_for_reviews;
	int duration;
	int director_fb_likes;
	int actor_3_fb_likes;
	int actor_1_fb_likes;
	int gross;
	int num_voted_users;
	int cast_total_fb_likes;
	int facenumber_in_poster;
	int num_user_for_reviews;
	int budget;
	int title_year;
	int actor_2_fb_likes;
	double imdb_score;
	double aspect_ratio;
	int movie_fb_likes;
} record;

//Suggestion: prototype a mergesort function

