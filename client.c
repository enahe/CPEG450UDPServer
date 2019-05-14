
//client.c: contains all the code necessary to transfer files from client to server. Creates a socket for the client, connects it to the server, and asks the server for files to transfer. 



#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

int checkSum(char * message) { //compute checksum
	char xor = 0;
	for(int i = 0; i < sizeof(message)/sizeof(message[0]); i++) {
		xor = xor ^ message[i];
	}
        printf("Checksum = %d\n", xor);
        return xor;
}

int main () {
   int clientSocket;
   int connected;
   ssize_t length; //length of file to be transfered. 
   struct sockaddr_in serverAddress;
   char buffer[BUFSIZ];
   char *data;
   char serverIP[25]; // string of server's ip address
   char fileName[100]; // string of filename to copy
   int fileSize; 
   int continueSend = 1;
   FILE *recievedFile;
   int remainingData = 0;
   struct hostent *hostIP;
   char windowSize[10];
   int windowLength; //holds length of window obtained from user
   int chunkSize; //holds the size of each chunk that is going to be sent in a window
   int seqNum; //sequence number for cumulative ack
   int *windowCorrect; //is going to be the same size as the size of the window +1 , and will let us know which chunks of data have been recieved properly vs which ones need to be corrected 
   char checkSumChar[1]; //char array for current checkSumArray; 
   unsigned int checkSumInt; //checksum int 
   char seqNumChar[1]; //char array for sequence number
   int seqNumServer; //int for sequence number 
   int windowEnd; //int for when the current window ends

   printf("Hello. Enter the window size you would like to use.\n");
   scanf("%s", windowSize);
   windowLength = atoi(windowSize); 
   printf("Working with a window length of %d\n", windowLength);
   printf("Hello. Please enter the ip address of the server you would like to connect to.\n");
   scanf("%s", serverIP);
   hostIP = gethostbyname(serverIP); //get the ip address of the server 
   clientSocket = socket(AF_INET, SOCK_DGRAM, 0); //create client socket
   if (clientSocket != -1) {
       printf("Client socket has been created\n");
   }
   else {
       printf("Client socket has not been created. Try again\n"); 
       exit(EXIT_FAILURE);
   }
   memset(&serverAddress, 0, sizeof(serverAddress)); //0 out serverAddress 
   socklen_t serverLength = sizeof(serverAddress);
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_port = htons(2500);
   serverAddress.sin_addr= *((struct in_addr*)hostIP->h_addr); //create the server's address
    if (sendto(clientSocket, windowSize, sizeof(windowSize), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) { //send the window size to the server
       printf("Successfully sent window size to server.\n");
   }
   else {
       printf("Window size was not sent to server. Error: %d\n", errno); 
       exit(EXIT_FAILURE);
   }
   printf("Please enter the file name you would like to retrieve from the server. Please also keep the file name under 99 characters.\n");
   scanf("%s", fileName); 
   if (sendto(clientSocket, fileName, sizeof(fileName), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) { //send the filename that you would like to copy to server
       printf("Successfully sent filename to server. Awating response.\n");
   }
   else {
       printf("File name was not sent to server. Error: %d\n", errno); 
       exit(EXIT_FAILURE);
   }
   if (recvfrom(clientSocket, buffer, BUFSIZ, 0, (struct sockaddr*)&serverAddress, &serverLength) != -1) { //get size of file from server
       printf("Successfully recieved file size back from server. Beginning transmission\n");
   }
   else {
       printf("Could not recieve size of file from server. Please try again.\n");
       exit(EXIT_FAILURE);
   }
   fileSize = atoi(buffer);
   printf("The client recieved a file size of %d\n", fileSize);
   chunkSize = fileSize / windowLength / 2; 
   data = (char*)malloc(chunkSize * sizeof(char));

   printf("Transmitting in chunks of %d\n", chunkSize);

   
   recievedFile = fopen(fileName, "w"); //open an empty file on the client with the same name as the one on the server
   if (recievedFile == NULL) {
       printf("Failed to open the file requested. Please try again\n");
       exit(EXIT_FAILURE);
   }
   remainingData = fileSize; 
   seqNum = 0; 
   windowCorrect = (int *)malloc(sizeof(int) * ((windowLength * 2) + 1)); 
   for (int i = 0; i < (windowLength *2) + 1; i++) {
	windowCorrect[i] = 0; //set all elements of windowCorrect to 0 to show that nothing has been written yet 
   }
   printf("The client recieved a file size of %d\n", remainingData);
   memset(data, 0, sizeof(data));
   while ((remainingData > 0)) { //while the server continues to send more data
	windowEnd = windowLength + seqNum; // the end of the window 
        if (windowEnd > windowLength *2) {
		windowEnd = windowLength *2;
        }
	for (int i = seqNum; i < windowEnd; i++) {
                rewind(recievedFile); 
                if (windowCorrect[i] = 0) { 
			length = read(clientSocket, data, sizeof(data));
                         
			if (recvfrom(clientSocket, checkSumChar, 1, 0, (struct sockaddr*)&serverAddress, &serverLength) != -1) { //get size of file from server
       				printf("Successfully recieved checksum from sever\n");
   			}
   			else {
      				 printf("Could not recieve checksum from server. Please try again.\n");
       				 exit(EXIT_FAILURE); 
   			}
        	        if (recvfrom(clientSocket, seqNumChar, 1, 0, (struct sockaddr*)&serverAddress, &serverLength) != -1) { //get size of file from server
       				printf("Successfully recieved sequence number from srever\n");
   			}
   			else {
      			 	printf("Could not recieve sequence number from server. Please try again.\n");
       exit(EXIT_FAILURE); 
   }  
                        seqNumServer = atoi(checkSumChar); 
                        checkSumInt= atoi(checkSumChar);
                        if (checkSumInt == checkSum(data)) {
				fseek(recievedFile, seqNumServer * chunkSize, SEEK_SET); // set the file's offset
				fprintf(recievedFile, "%s", buffer); //copy the text from the buffer of the server to the text file on this machine
                		
                		windowCorrect[i] = 1; 
        			remainingData -= length; //reduce the amount of data left to recieve
        		fprintf(stdout, "Recieved ld bytes, only %d left to go\n", length, remainingData);
			}
		
	}
       }
        int tempSeqNum = 0;
	for (int i = seqNum; i < windowEnd; i++) {
            if (windowCorrect[i] == 0) {
           	char * seqNumServer = malloc(32); 
                sprintf(seqNumServer, "%d", i); 
                if (sendto(clientSocket, seqNumServer, sizeof(seqNumServer), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) { //send the filename that you would like to copy to server
       		printf("Successfully sent sequence number to server where error was recieved in message.\n");
                tempSeqNum = i;
                break; 
   		}
   		else {
       printf("Sequence number was not sent to server. Error: %d\n", errno); 
       exit(EXIT_FAILURE);
   		}
            }
        }
        if (tempSeqNum == 0) {
             seqNum += windowLength; 
             char * seqNumServerComplete = malloc(32);
             sprintf(seqNumServerComplete, "%d", seqNum); 
		if (sendto(clientSocket, seqNumServerComplete, sizeof(seqNumServerComplete), 0, (struct sockaddr*)&serverAddress, serverLength) != -1) { //send the sequence number acknowledgement back to serverto server
       		printf("Successfully sent sequence number to server where no errors where recieved in message.\n");
   		}
   		else {
       printf("Sequence number was not sent to server. Error: %d\n", errno); 
       exit(EXIT_FAILURE);
   		}
            }
         else { 
              seqNum += tempSeqNum; 
         }
             
        

}
       memset(buffer, 0, sizeof(buffer)); 
       fclose(recievedFile); // close the file
      close(clientSocket); //once the user is done, close the socket
      return 0; 
   
  

}

