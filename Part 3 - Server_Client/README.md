CS214 Project 3: Basic Data Sorter - server/client

IMPORTANT: Byzantine Error: Our implementation truncates data without the usage of print statements before writing to the final file. Also, sometimes the final output will be incorrect after multiple successful runs on the same parameters/data from client, and then the one right after that will be correct again. We are attempting to correct this issue (as of 23:11 12/13/17), but are unsure if it will be possible as we have no idea why this is happening. Please have mercy on our grade as we recognize that the client should not be printing to STDOUT, thank you!

This project involves creating a multithreaded client and server.

team 1: !failing members: Joseph Redling-Pace and Xiaopeng (Jake) Zhou
team 2: TSMTSMTSM members: Andrew Hernandez and Michael Belmont

Instructions for use: Compile both the server and the client with makefiles provided. 
	-Server Instructions
		To run the server and listen for connections, please type: 
		
			./sorter_server -p <port_num>
			
	-Client Instructions
		To utilize the client Makefile, please do "make clean" then "make".
		For the client parameters, we assumed that they would only appear in the order given in the Assignment Page. The parameters within [] are optional, but if both are used, must remain in that order. Therefore, please use this convention:
			
			./sorter_client -c <column_name> -h <host_name> -p <port_num> [-d <directory_to_search_path> -o <directory_to_output_path>]
			
Assumptions:
	-Only one client will be attempting to connect to the server at a time (unless extra credit is attempted).
	-The parameters for the client input will be in the same order as given in the Assignment description.
	-The number of files will not exceed the iLab given limit.
	-

Protocol for TCP handshake:
	-The server starts up, creates a port to connect to, and listens for connections.
	-The client will traverse through a given directory. When a valid CSV is found (i.e. not sorted, proper category header, etc.), a thread is spawned and attempts to connect to the server. The server also spawns a thread to handle the connection request, and appends it to a linked list of thread IDs.
	-After connection, the client sends a signal to the server to start reading in records of the CSV (represented by "~STARTX~"). Each of these lines are appended to a global record linked list in the server.
	-When the CSV is exhausted of records, a finished signal is sent to the server (represented by "~END~"). This will also close that connection.
	-Once the directory traversal has finished, the client sends a dump request to the server (represented by "~DUMPXY~" - Yes, it could have been "~DUMPED~" but that isn't as interested.)
	-After the dump request is received, the server will join all of the thread IDs in the thread ID linked list. Once that is completed, the entirety of the record linked list will be merge sorted.
	-Next, the client will open a new file to write to called "AllFiles-sorted-<column_name>.csv". The server will begin writing out the record linked list to the client.
	-When writing the linked list out to the client finishes, the connection closes with the client and the server continues to listen for requests.
	
Output: 
	-Client:
		The client successfully outputs the sorted data into an "AllFiles-sorted-<column_name>.csv" file. Nothing is printed to STDOUT from client.
	-Server:
		The server prints the IP addresses of all of the connected clients. The "Received connections from: <ipaddress>,..." message will appear when the server starts up. Any IP addresses will appear on the next line after "Received connections from: ".
		NOTE: The IP address is printed each time a request is made. This results in repeated IP addresses if multiple requests are made from the same client, which is intended. This way, you can check how many files are being read in to the server.
		
Testing Procedure:
	-To test our implementation, we first began by testing the transfer/sorting of one CSV at a time of varying sizes in the same directory as the executable.
		-The implementation works for files with less than ~1200 records.
	-Next, we tested the -d and -o flags with the single file trial.
	-Afterwards, we tested multiple files at a time of the same size in the same directory as the executable. Then, we tested the -d and -o flags once again.
		-Still, we faced an issue with files with a number of records greater than ~1200.
	-Finally, we tested a single file in each of multiple sub-directories, and afterwards, the -d and -o flags again.
		
Problems/Errors Faced:
	-Our implementation fails on files that have large amounts of lines in them (#lines > 1200). The error resulted is a heap-buffer-overflow. Upon inspecting our code, we cannot find what is causing this issue (as of 12/13/17 22:00).
		-However, it works flawlessly on large amounts of files with a medium amount of lines. For example, the program failed to work on a single file with 1200 lines, but worked on sorting 1000 copies of a 300 line CSV (resulting in a final AllSorted CSV of 300,000 lines).
	-We experienced an issue with race conditions, which we attempted to bandage fix by including sleep() calls. HOWEVER, we believe we have fixed the issue with the implementation of the thread ID linked list and joining of those thread IDs before dumping.
	-Running two clients on the same machine at the same exact time yielded arbitrary behavior and an eventual segfault.
	-If you run the client with the same exact parameters multiple times, then sometimes the output to the file is garbled. This is resolved by just deleting the "AllFiles-sorted-<column_name>.csv" between trials.
	-Our implementation occassionally experiences what we believe is packet loss. Some trials result in 300/300 lines being successfully printed to the final file, whereas other result in, for example, 284/300 lines being printed.
	
Interesting Observations:
	-Our code worked completely perfectly when the server and client existed on the same iLab machine. Errors only occurred when having to communicate between iLab machines.

This README describes the version of our code as of 22:00 12/13/17. If changes are made, we will attempt to edit the README, if time permits.
