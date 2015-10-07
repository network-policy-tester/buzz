/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "sbuFlowTags.h"

const int maxMboxId = 100;
const int maxTag = 100;
const int maxMboxState = 10;
const int queryTypeSize = 1;//in terms of number of digits
const int mboxIdSize = 2;//in terms of number of digits
const int tagSize = 2;//in terms of number of digits
const int stateSize = 1;//in terms of number of digits
const int fourTupleSize = 50;//in terms of number of digits (i.e., chars)
const int configReplySize = 50;//in terms of number of digits (i.e., chars)
const int noOfConfigItems = 1;//in terms of number of digits (i.e., int)
const int queryCodeSize = 1;//in terms of number of digits

void dostuff(int, char*[][maxTag], int[][maxTag][maxMboxState], int *configTable);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 5) {
		printf("%s\n","usage: ./server port ctrlInTagTable ctrlOutTagTable flowTagsMbConfig");
		printf("%s\n","usage: ./server 0 ctrlInTagTable ctrlOutTagTable flowTagsMbConfig");
	        exit(1);
     	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0){
		perror("ERROR opening socket");
	}
	
	// blocking socket
	// http://stackoverflow.com/questions/12773509/read-is-not-blocking-in-socket-programming
	// int iMode;
	// iMode = 0;
	// ioctl(sockfd, FIONBIO, &iMode);  

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(FLOWTAG_CONTROLLER_PT);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
              error("ERROR on binding");
	}
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	//initializing In and Out tag tables
	int i;
	int j;
	int k;

	FILE *ctrlInTagFile;
	char *ctrlInTagFileName = argv[2];
	ctrlInTagFile = fopen(ctrlInTagFileName, "r");

	if (!ctrlInTagFile){
		printf("file cannot be opened %s\n", argv[2]);
		return 1;
	}

	char* ctrlInTagTable[maxMboxId][maxTag];
	
	sbuFiveTuple *tmp;
	
	for (i = 0; i < maxMboxId; i++){
		for (j = 0; j < maxTag; j++){
			ctrlInTagTable[i][j] = malloc(sizeof(sbuFiveTuple));
			
			tmp = (sbuFiveTuple *)(ctrlInTagTable[i][j]);
			
			fscanf(ctrlInTagFile,"%s", tmp->srcIP );
			fscanf(ctrlInTagFile,"%s", tmp->dstIP );
			fscanf(ctrlInTagFile,"%d", &(tmp->srcPT) );
			fscanf(ctrlInTagFile,"%d", &(tmp->dstPT) );
			
			//fprintf(stdout, "(1:%s 2:%s 3:%d 4:%d)\n", tmp->srcIP, tmp->dstIP, tmp->srcPT, tmp->dstPT);
			//ctrlInTagTable[i][j] = "130.245.144.189,29.214.117.11,10000,4005";
		}
	}
	
	FILE *ctrlOutTagFile;
	char *ctrlOutTagFileName = argv[3];
    ctrlOutTagFile = fopen(ctrlOutTagFileName, "r");

	if (!ctrlOutTagFile){
		printf("file cannot be opened %s\n", argv[3]);
		return 1;
	}

	int ctrlOutTagTable[maxMboxId][maxTag][maxMboxState];

	for (i = 0; i < maxMboxId; i++){
		for (j = 0; j < maxTag; j++){
			for (k = 0; k < maxMboxState; k++){
				fscanf(ctrlOutTagFile,"%d", &ctrlOutTagTable[i][j][k]);
				// hack for testing: LUIS
				ctrlOutTagTable[i][j][k] = 4;
			}
		}
	}
	
	FILE *configInfoFile;
        char *configInfoFileName = argv[4];
        configInfoFile = fopen(configInfoFileName, "r");

        if (!configInfoFile){
                printf("file cannot be opened\n");
                return 1;
        }

	int *configTable;
	configTable = malloc(noOfConfigItems);

	for (i=0; i<noOfConfigItems; i++){
		//fscanf(configInfoFile,"%d", configTable[i]);
		//configTable[i] = "tos";
		// Luis: needs to be improved
		configTable[i] = FLOWTAG_TAGFIELD_TOS;
	}
	
	while (1) {
        	newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) 
             error("ERROR on accept");

         pid = fork();

        if (pid < 0){
             error("ERROR on fork");
		}

        if (pid == 0)  {
             
			 close(sockfd);
             dostuff(newsockfd,ctrlInTagTable,ctrlOutTagTable,configTable);
             exit(0);
			 
        } else {
			close(newsockfd);
		}
		
     } /* end of while */

     close(sockfd);

     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock, char* ctrlInTagTable[][maxTag], int ctrlOutTagTable[][maxTag][maxMboxState], int *configTable)
{
	int n;
	unsigned char *buffer;
	
	fd_set rset;
	int tempmax;
	int result;
	
	sbuFlowTags *reply;
	sbuFlowTags *request;
	
	reply = malloc(sizeof(sbuFlowTags));
	buffer = malloc(sizeof(sbuFlowTags));
	

	while(1){
	 
		n = recv(sock,buffer,sizeof(sbuFlowTags),0);
		
		if ( n > 0){
			 
			request = (sbuFlowTags *)buffer;
			
			int mboxId;													//Computing middlebox id

			mboxId = request->mbid;
			mboxId = mboxId - 1;
			
			fprintf(stdout, "Input: Type: %d \t MbID: %d \t Tag: %d \t State: %d \n", request->type, request->mbid, request->tag, request->state);
			
			reply->mbid = request->mbid;
			reply->type = request->type;
			
			if (request->type == FLOWTAG_QUERY){
				reply->state = configTable[mboxId]; 
				
			}else if (request->type == FLOWTAG_INTAG){
				
				//ctrlInTagTable[mboxId][request->tag]
				//memcpy(reply->data, ctrlInTagTable[mboxId][request->tag], 44);
				printf("reply is a 4-tuple\n");

			} else if (request->type == FLOWTAG_OUTTAG){//outTag query
			
				int nextTag = 0;
				nextTag = ctrlOutTagTable[mboxId][request->tag][request->state];

				reply->tag = nextTag;
				
			} 
			
			fprintf(stdout, "Reply: Type: %d \t MbID: %d \t Tag: %d \t State: %d \n", reply->type, reply->mbid, reply->tag, reply->state);
			
			if(write(sock, reply, sizeof(sbuFlowTags)) < 0){
				fprintf(stderr, "ERROR reading writing to socket");
			}
			 
		}else if (n == 0) {
			fprintf(stderr, "Socket shutdown");
			close(sock);
			return;
			
		}else  if (n < 0) {
			fprintf(stderr, "ERROR reading from socket");	
		}
	}	
}
