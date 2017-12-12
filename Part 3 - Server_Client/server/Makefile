all: sorter_server

sorter_server: sorter_server.c
	gcc -g -Werror -Wall -fsanitize=address -pthread mergesort.c sorter_server.c -o sorter_server
	
clean:
	rm -rf sorter_server
