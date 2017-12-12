all: sorter_client

sorter_client:  
	gcc -g -Werror -Wall -fsanitize=address -o sorter_client sorter_client.c mergesort.c -pthread
	
clean:
	rm -rf sorter_client mergesort.o
