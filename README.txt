4/6/19
CPEG 450 Networks Programming Assignment 1
Eric Nahe

IMPORTANT: THIS CODE MUST BE RUN ON A UNIX BASED OPERATING SYSTEM. PLEASE ENSURE THAT YOUR OPERATING SYSTEM OF CHOICE HAS SUPPORT FOR sendfile(), AND LINUX SYSTEM CALLS, OR ELSE THIS PROGRAM WILL CRASH. 

-List of relevant files: 
	server.c: The file which contains all the server code for this assignment. This will create a server bound to the IP of the machine you run it on, and will send files from the server to the client, run on another machine.
	client.c: The file which contains all the client code for this assignment. This will create a client program which you will instruct to connect to the IP of the server you have created. You can then request files from the server. 
	Makefile: this contains all the rules for making BOTH THE CLIENT AND SERVER EXECUTABLES. YOU WILL NEED 2 COPIES OF THE MAKEFILE (ONE AT THE CLIENT SIDE, ONE AT THE SERVER SIDE) TO COMPILE BOTH PROGRAMS INTO THIER RESPECTIVE EXECUTABLES. 

Compilation instructions: 
	Firstly, you are going to want to copy the client.c file and the server.c file to 2 seperate, networked machines. Ensure that a copy of the Makefile is on both systems as well, then type either make client or make server to compile the respective client or server executable. Also, ensure that the machine that you have moved server.c to has text files in that same directory for the client to ask for and retrieve. These can be however long you want, but please keep the filenames under 99 characters. 

Running instructions: 
	Both client and server require no command line arguments to run. 
	Before running the client program, make note of the IP address of the machine containing server.c. The client will prompt you for this once you run it. 
	Please run the server executable before starting the client executable. 

Server running instructions: Before running the server executable, please make some text files and put them in the same directory as your server executable. To run the server program, simlply type ./server in the location of your server executable. No other input is required, and the server should close once the client has finished asking for files or if an error is raised. 

Client running instructions: To run the client program, simply type ./client in the location of your client executable. You will then be prompted to enter the IP address of the server you wish to connect to. Enter the correct IP, and then you should be prompted to enter the file name of the file you wish to copy from file to server. Enter this as well, and once the transfer completes, you will be asked if you want to transfer another file. Enter 1 to keep using the program, and 0 to stop. 


