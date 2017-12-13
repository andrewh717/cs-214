// CS214: Systems Programming Fall 2017
#include"sorter_server.h"

/*The main method for the merge function. This goes through the 4 steps of mergesort and uses helper functions to complete its purpose*/
//We pass a pointer to the poitner of the linked list, which we can de-reference to change the actual list, and an int representing the category that we are sorting by
void merge(record ** frontAddress, int sortingBy)
{
	record * front = *frontAddress;
	record * first = NULL;
	record * second = NULL;

	//List does not need to be sorted if 1 or 0 element(s)
	if((front == NULL) || (front->next == NULL))
	{
		return;
	}

	//Need a method that splits the list of data points into halves
	split(front, &first, &second);

	//Recursively sort each half-list
	merge(&first, sortingBy);
	merge(&second, sortingBy);

	//Lastly merge them together
	*frontAddress = sorted(first, second, sortingBy);

	return;
}

record * sorted(record * first, record * second, int sortingBy)
{
	//The pointer to the front of our sorted list once we are done merging the two given list-halves	
	record * result = NULL;
	
	/*Checking to see if one of the lists is empty*/
	if(first == NULL)
	{
		return second;
	}
	else if(second == NULL)
	{
		return first;
	}
	
	//If isNum is 0, then we are sorting by string. If isNum is 1, we are sorting by numbers
	int isNum = 0;
	
	//We are wasting a tiny amount of space to create a variable for the case of longs/doubles or strings
	char * fStrVal = "";
	char * sStrVal = "";
	double fNumVal = 0;
	double sNumVal = 0;

	switch (sortingBy) {
		case 0: case 1: case 6: case 9: case 10: case 11: case 14: case 16: case 17: case 19: case 20: case 21:
			fStrVal = first->line[sortingBy];
			sStrVal = second->line[sortingBy];
			//printf("fStrVal: %s ---- sStrVal: %s\n", fStrVal, sStrVal);
			break;
		case 2: case 3: case 4: case 5: case 7: case 8: case 12: case 13: case 15: case 18: case 22: case 23: case 24: case 25: case 26: case 27:
			isNum = 1;
			fNumVal = atof(first->line[sortingBy]);
			sNumVal = atof(second->line[sortingBy]);
			//printf("fNumVal: %f ---- sNumVal: %f\n", fNumVal, sNumVal);
			break;
		default:
			printf("Fatal Error: Something went wrong with selecting the category to sort by.\n");
			exit(EXIT_FAILURE);
	}
	
	//printf("%s and %s\n", fStrVal, sStrVal);
	
	/*We're going to take the lexicographically first value and store it as the current node. We are going to call this method recursively on both lists minus what we just added so we can keep going in order until one list is null*/
	
	//STILL HAVE TO ACCOUNT FOR INSTANCES OF NULL
	if(isNum == 0)
	{
		if (strcmp(fStrVal, "-1") == 0) //The first value is a NULL value
		{
			result = first;
			result->next = sorted(first->next, second, sortingBy);
		}
		else if (strcmp(sStrVal, "-1") == 0) //The second value is a NUll value
		{
			result = second;
			result->next = sorted(first, second->next, sortingBy);
		}
		else //Neither of the values are NULL
		{
			if(strcmp(fStrVal, sStrVal) <= 0)
			{
				result = first;
				result->next = sorted(first->next, second, sortingBy);
			}
			else
			{
				result = second;
				result->next = sorted(first, second->next, sortingBy);
			}
		}
	}
	else if(isNum == 1)
	{
		if (fNumVal == -1) //The first value is NULL
		{
			result = first;
			result->next = sorted(first->next, second, sortingBy);
		}
		else if (sNumVal == -1) //The second value is NULL
		{
			result = second;
			result->next = sorted(first, second->next, sortingBy);
		}
		else //Neither value is NULL
		{
			if(fNumVal <= sNumVal)
			{
				result = first;
				result->next = sorted(first->next, second, sortingBy);
			}
			else
			{
				result = second;
				result->next = sorted(first, second->next, sortingBy);
			}
		}
	}
	//Returning the merged and sorted result
	return result;
}

/*This function takes a linked list and returns pointers to the two halves of the source list. If there is an odd amount of items in the list, the extra node goes into the front half*/

//WE THINIK THIS SHOULD SAVE ANY UPDATES TO FIRST AND SECOND TO THE ORIGINAL ADDRESS BUT IF THIS DOESN'T WORK PROPERLY, WE KNOW WHERE TO LOOK
void split(record * front, record ** first, record ** second)
{
	//Using a fast and slow iterating pair of pointers to find the midpoint of the linked list
	record * fast;
	record * slow;
	//If there are only one or no items in the list so far to sort
	if(front == NULL || front->next == NULL)
	{
		*first = front;
		*second = NULL;
	}
	else //If there are more than just one or no items in the list
	{
		slow = front;
		fast = front->next;
		//Iterating the fast pointer by two nodes each cycle while only iterating slow by one node
		//We keep iterating until fast reaches the end, at which point, slow should be at the halfway point
		while(fast != NULL)
		{
			fast = fast->next;
			if(fast != NULL)
			{
				slow = slow->next;
				fast = fast->next;
			}
		}
		*first = front;
		*second = slow->next;
		slow->next = NULL;
	}
	return;
}

