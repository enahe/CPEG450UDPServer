
//server.c: Contains all the code necessary to connect to a client and send it text files located in the same directory as its executable. 



#include<string.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<stdio.h>
#include<net/if_arp.h>
#include<sys/sendfile.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h> 

unsigned int checkSum(char * message) { //compute checksum
	unsigned int xor = 0;
	for(int i = 0; i < sizeof(message)/sizeof(message[0]); i++) {
		xor = xor ^ message[i];
	}
        printf("Checksum = %d\n", xor);
        return xor;
}

int main() {
   int serverSocket, bindStatus;
   char  fileName[100];
   char  fileSize[256]; 
   char buffer[BUFSIZ];
   char continueSize[BUFSIZ]; // string to determine if the user wants more files 
   struct stat fileStatus;
   struct sockaddr_in serverAddress;
   int filePointer;
   serverSocket = socket (AF_INET, SOCK_DGRAM, 0); //create socket for server
   ssize_t length;
   off_t offset;
   int remainingData; 
   int sentData = 0;
   int continueSend = 1; 
   FILE *file; 
   char windowSize[100]; //size of the window, in a char array
   int windowLength; //size of the window as an int
   char prob[100]; //char array for probabilty of error. 
   double probDouble; //probability of error as a double
   char checkSumOriginal[100]; //check sum as a char array, used to send to client
   int checkSumInt; 
   unsigned int check; //check sum as an unsigned int, calculated here.
   int chunkSize; 
   char *data;
   int seqNum; 
   int windowEnd; 
   char seqNumChar[100]; //used to hold seq number to send to client
   char seqNumServer[32]; //used to hold seq number recieved from client
   int  seqNumClient; //int representation of sequence number recieved from client
   
   srand(time(0)); 
   if (serverSocket < 0) {
	printf("Client socket is not created. Please try again\n");
        exit(EXIT_FAILURE);
   }
   else {
        printf("Client socket has been created successfully\n");
   }
   
   memset(&serverAddress, 0, sizeof(serverAddress));
   socklen_t serverLength = sizeof(serverAddress);
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
   serverAddress.sin_port = htons(2500); //intialize data for server
   bindStatus = bind(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t *)sizeof(serverAddress)); //bind the data to the socket
   if (bindStatus == 0) {
	printf("Bound successfully!\n");
   }
   else {
        printf("Bind failure!\n");
        exit(EXIT_FAILURE);
   }
   printf("Hello. Enter the probablility of error you would like to use.\n");
   scanf("%s", prob);
   probDouble = atof(prob); 
   printf("Working with a probability of %lf\n", probDouble);
while (1) {
    if (recvfrom(serverSocket, windowSize, sizeof(windowSize), 0, (struct sockaddr*)&serverAddress, &serverLength) != -1) { //recieve the size of the window from the client
       printf("Recieved window size \n");
   }
   else {
       printf("Windows Size not recieved properly. Error %d\n", errno);
       exit(EXIT_FAILURE);
   }
   windowLength = atoi(windowSize); 
   printf("Working with a window length of %d\n", windowLength);
   if (recvfrom(serverSocket, fileName, sizeof(fileName), 0, (struct sockaddr*)&serverAddress, &serverLength) != -1) { //recieve the filenames the client wants
       printf("Recieved file name\n");
   }
   else {
       printf("File name not recieved properly. Error %d\n", errno);
       exit(EXIT_FAILURE);
   }
     filePointer = open(fileName, O_RDONLY); //try and open the file
    if (filePointer == -1) {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    if (fstat(filePointer, &fileStatus) < 0) { //get the files status
        printf("Error fstating\n"); 
        exit(EXIT_FAILURE);
    }
    sprintf(fileSize, "%d", fileStatus.st_size);
    length = sendto(serverSocket, fileSize, BUFSIZ, 0, (struct sockaddr*)&serverAddress, serverLength); //send the files size to the client
    if (length < 0) {
        printf("Error on sending filesize");
        exit(EXIT_FAILURE);
    }
    
    printf("The server sent a file size of %d\n", fileStatus.st_size);
    offset = 0;
    remainingData = fileStatus.st_size; 
    chunkSize = fileStatus.st_size / windowLength / 2;
    data = (char*) malloc(chunkSize * sizeof(char));
    printf("Transmitting in chunks of %d\n", chunkSize); 
    file = fopen(fileName, "r");
    seqNum = 0;
    memset(data, 0, sizeof(data));
    while (remainingData > 0) { //while there is still data to be sent, send it to the client 
         windowEnd = windowLength + seqNum;
         if (windowEnd > windowLength * 2) {
		windowEnd = windowLength * 2;
         }
         for (int i = seqNum; i  < windowEnd; i++) {
		rewind(file);
                fseek(file, i * chunkSize, SEEK_SET); // move the file to the correct place for reading
                sentData = fread(data, 1, sizeof(data), file);
                if (((double)rand() / (double)RAND_MAX) <= probDouble) {
                printf("Sending bad data in sequence %d to client.\n", i);
                checkSumInt = checkSum(data);
                	for (int i = 0; i < sizeof(data); i++) {
				data[i]= data[i] >>= 1;
                        }
                	if (sendto(serverSocket, data, sizeof(data), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) {
                		printf("Successfully sent data to client");
                	}
                	else {
  			 	printf("Data was not sent to client. Error: %d\n", errno); 
      			 	exit(EXIT_FAILURE);
                	}
			sprintf(checkSumOriginal, "%d", checkSumInt);
                      	if (sendto(serverSocket, checkSumOriginal, sizeof(checkSumOriginal), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) {
                		printf("Successfully sent checksum %d to client\n", checkSumInt);
                	}	
                	else {
  			 	printf("Checksum was not sent to client. Error: %d\n", errno); 
      			 	exit(EXIT_FAILURE);
                	}
                		sprintf(seqNumChar, "%d", i);
	 	      	if (sendto(serverSocket, seqNumChar, sizeof(seqNumChar), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) {
                		printf("Successfully sent sequence number %d to client\n", i);
                	}
                	else {
  			 	printf("Data was not sent to client. Error: %d\n", errno); 
      			 	exit(EXIT_FAILURE);
                	}
		}
                else {
			checkSumInt = checkSum(data);
				
 			if (sendto(serverSocket, data, sizeof(data), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) {
                		printf("Successfully sent data to client");
                	}
                	else {
  			 	printf("Data was not sent to client. Error: %d\n", errno); 
      			 	exit(EXIT_FAILURE);
                	}
			sprintf(checkSumOriginal, "%d", checkSumInt);
                      	if (sendto(serverSocket, checkSumOriginal, sizeof(checkSumOriginal), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) {
                		printf("Successfully sent checksum %d to client\n", checkSumInt);
                	}	
                	else {
  			 	printf("Checksum was not sent to client. Error: %d\n", errno); 
      			 		exit(EXIT_FAILURE);
			}
                	sprintf(seqNumChar, "%d", i);
	 	      	if (sendto(serverSocket, seqNumChar, sizeof(seqNumChar), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) {
                		printf("Successfully sent sequence number %d to client\n", i);
                	}
                	else {
  			 	printf("Data was not sent to client. Error: %d\n", errno); 
      			 	exit(EXIT_FAILURE);
                	}
                        remainingData -= sentData;
                  }      
                }
                
	 	fprintf(stdout, "Server sent %d bytes. Offset = %ld, Remaining Data = %d\n", sentData, offset, remainingData);
		if (recvfrom(serverSocket, seqNumServer, sizeof(seqNumServer), 0, (struct sockaddr*)&serverAddress, &serverLength) != -1) { //recieve the sequence number from the client
		seqNumClient = atoi(seqNumServer);
       		printf("Recieved sequence number %d from client\n", seqNumClient);   
                seqNum = seqNumClient; 
   		}
   		else {
       		printf("File name not recieved properly. Error %d\n", errno);
       		exit(EXIT_FAILURE);
   		}
		
    }
    fclose(file); 
}
    close(serverSocket);
    return 0; 
}
