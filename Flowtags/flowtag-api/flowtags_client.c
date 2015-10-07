#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#include "sbuFlowTags.h"
#include "flowtags_client.h"

#define DEBUG_sbuFLOWTAGS 1
int flowtag_mbid;
int flowtag_tagfield;
int controller_fd;
 
void flowtags_set_mbid(int mbid){
	flowtag_mbid = mbid;
}

void flowtags_connect(char *controller){
	
    struct sockaddr_in serv_addr;
	struct hostent *server;
	
	if(flowtag_mbid > 0){										// only connect if the mbid is given
		
		if( controller_fd <= 0){								// only connect if there is no previous connection

			controller_fd = socket(AF_INET, SOCK_STREAM, 0);
			
			if (controller_fd < 0) 
				error("FLOWTAGS API: ERROR opening socket");
			
			if (controller == NULL){
				server = gethostbyname(FLOWTAG_CONTROLLER_IP);
			}else{
				server = gethostbyname(controller);
			}
			
			bzero((char *) &serv_addr, sizeof(serv_addr));
			
			serv_addr.sin_family = AF_INET;
			
			bcopy(  (char *)server->h_addr, 
					(char *)&serv_addr.sin_addr.s_addr,
					 server->h_length
				 );
			
			serv_addr.sin_port = htons(FLOWTAG_CONTROLLER_PT);
			
			if (connect(controller_fd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
				
				controller_fd = 0;
				fprintf(stdout, "FLOWTAGS API: Failed to connect\n");
			}
			
		}
	}
}

int flowtags_is_connected(){
	
	if (controller_fd > 0){
		return 1;
	}
	
	return 0;
}

void flowtags_disconnect(){

	if( controller_fd > 0 ){
	
		close(controller_fd);
    controller_fd = -1;
	
	}
}

int flowtags_get_tagfield(){
	
	sbuFlowTags *request;
	sbuFlowTagsWrapper *reply;

#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: flowtags_get_tagfield\n");
#endif

	if ( flowtag_tagfield > FLOWTAG_TAGFIELD_NOTDEFINED) {
		return flowtag_tagfield;
	}else{
	
		request = (sbuFlowTags *)malloc(sizeof(sbuFlowTags));

		request->type = htonl(FLOWTAG_QUERY);
		request->mbid = htonl(flowtag_mbid);
		
		reply = get_reply_FlowTags(request, NULL);
		
		flowtag_tagfield = (reply->flowtag_header)->state;
		
		free(reply->flowtag_header);
		free(reply->five_tuple);
		free(reply);
		
		free(request);
	}
	
	return flowtag_tagfield;
}

int flowtags_get_action(int tag, int state){

	sbuFlowTags *request;
	sbuFlowTagsWrapper *reply;

	int output;

#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: flowtags_get_action\n");
#endif

	request = (sbuFlowTags *)malloc(sizeof(sbuFlowTags));

	request->type = htonl(FLOWTAG_OUTTAG);
	request->mbid = htonl(flowtag_mbid);
	request->tag = htonl(tag);
	request->state = htonl(state);
	
	reply = get_reply_FlowTags(request, NULL);
	
	output = ((sbuFlowTags *)(reply->flowtag_header))->tag;
	
	free(reply->flowtag_header);
	free(reply->five_tuple);
	free(reply);
	
	free(request);
	
	return output;
}

// if request->state == 0 don't overwrite the tuple
sbuFiveTuple *flowtags_get_fivetuple(int tag){
	
	sbuFlowTags *request;
	sbuFlowTagsWrapper *reply;
	
	sbuFiveTuple *output;

#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: flowtags_get_fivetuple\n");
#endif

	request = (sbuFlowTags *)malloc(sizeof(sbuFlowTags));

	request->type = htonl(FLOWTAG_INTAG);
	request->mbid = htonl(flowtag_mbid);
	request->tag = htonl(tag);
	request->state = 0;
	
	reply = get_reply_FlowTags(request, NULL);
	
	output = reply->five_tuple;
	
	free(request);
	
	return output;
}

sbuFlowTagsWrapper *get_reply_FlowTags(sbuFlowTags *request, sbuFiveTuple *fivetuple){
 
	sbuFlowTagsWrapper *reply;
	 
	int n;
	int totalcnt;
	unsigned char *buffer;
	
	unsigned char *temp_buffer;
	
#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: get_reply_FlowTags\n");
#endif
	
	if(fivetuple == NULL){

		n = write(controller_fd, request, sizeof(sbuFlowTags));

		if (n < sizeof(sbuFlowTags)){
			perror("FLOWTAGS API: ERROR writing to socket");
			exit(0);
		}
	}else{
		temp_buffer = malloc(sizeof(sbuFlowTags) + sizeof(sbuFiveTuple));
		
		memcpy(temp_buffer, request, sizeof(sbuFlowTags));
		memcpy(temp_buffer + sizeof(sbuFlowTags), fivetuple, sizeof(sbuFiveTuple));

		n = write(controller_fd, temp_buffer, sizeof(sbuFlowTags) + sizeof(sbuFiveTuple));

		if (n < (sizeof(sbuFlowTags) + sizeof(sbuFiveTuple))){
			perror("FLOWTAGS API: ERROR writing to socket");
			exit(0);
		}
		
	}
	
	if(request->type != htonl(FLOWTAG_INTAG)){
		buffer = malloc(sizeof(sbuFlowTags));
		
		totalcnt = 0;
		while(totalcnt < sizeof(sbuFlowTags))
		{
			n = read(controller_fd, &buffer[totalcnt], sizeof(sbuFlowTags) - totalcnt);
			
			if (n < 0){
				perror("FLOWTAGS API: ERROR reading from socket");
				exit(0);
			}
			totalcnt += n;
		}
		
	}else{
	
#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: get_reply_FlowTags: waiting for %d\n", ((int)(sizeof(sbuFlowTags) +  sizeof(sbuFiveTuple))) );
#endif

		buffer = malloc(sizeof(sbuFlowTags) +  sizeof(sbuFiveTuple));
		
		totalcnt = 0;
		while(totalcnt < (sizeof(sbuFlowTags) +  sizeof(sbuFiveTuple)))
		{
			n = read(controller_fd, &buffer[totalcnt],sizeof(sbuFlowTags) +  sizeof(sbuFiveTuple) - totalcnt);
			
			if (n < 0){
				perror("FLOWTAGS API: ERROR reading from socket");
				exit(0);
			}
			
			totalcnt += n;
		}
	}
#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: get_reply_FlowTags: reply of len %d\n", totalcnt);
#endif
	reply = getFlowTags(buffer);

	return reply;
}

sbuFlowTagsWrapper *getFlowTags(unsigned char *packet){

	sbuFlowTagsWrapper *output;

	sbuFlowTags *incoming;
	sbuFiveTuple *five_tuple;
	
#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: getFlowTags\n");
#endif

	incoming = (sbuFlowTags *)packet;
	output = (sbuFlowTagsWrapper *)malloc(sizeof(sbuFlowTagsWrapper));
	
	output->flowtag_header = (sbuFlowTags *)malloc(sizeof(sbuFlowTags));
	
	((sbuFlowTags *)(output->flowtag_header))->type = ntohl(incoming->type);
	((sbuFlowTags *)(output->flowtag_header))->mbid = ntohl(incoming->mbid);
	((sbuFlowTags *)(output->flowtag_header))->tag = ntohl(incoming->tag);
	((sbuFlowTags *)(output->flowtag_header))->state = ntohl(incoming->state);

#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: Msg type %d mbid %d tag %d state %d\n", ntohl(incoming->type), ntohl(incoming->mbid), ntohl(incoming->tag), ntohl(incoming->state));
#endif
	if((output->flowtag_header)->type == FLOWTAG_INTAG && ntohl(incoming->state) != 0){
		
		five_tuple = (sbuFiveTuple *)(packet + sizeof(sbuFlowTags));
		output->five_tuple = (sbuFiveTuple *)malloc(sizeof(sbuFiveTuple));
		
		((sbuFiveTuple *)(output->five_tuple))->protocol = five_tuple->protocol;
		((sbuFiveTuple *)(output->five_tuple))->srcPT = ntohs(five_tuple->srcPT);
		((sbuFiveTuple *)(output->five_tuple))->dstPT = ntohs(five_tuple->dstPT);
		((sbuFiveTuple *)(output->five_tuple))->srcIP = ntohl(five_tuple->srcIP);
		((sbuFiveTuple *)(output->five_tuple))->dstIP = ntohl(five_tuple->dstIP);
		
#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: 5tuple proto(%d) srcPt(%hu) dstPt(%hu) srcIP(%d) dstIP(%d)\n", five_tuple->protocol, ntohs(five_tuple->srcPT),  ntohs(five_tuple->dstPT), ntohl(five_tuple->srcIP), ntohl(five_tuple->dstIP)); 
#endif

	}else{
		output->five_tuple = NULL;
#ifdef DEBUG_sbuFLOWTAGS
	fprintf(stdout, "FLOWTAGS API: at getFlowTags set five_tuple = NULL\n");
#endif
	}
	
	return output;
}

int main(int argc, char *argv[])
{   
	sbuFiveTuple *ftuple;

//	flowtags_set_mbid(LOCAL_MB_ID);
	  flowtags_set_mbid(1);
	flowtags_connect("172.16.1.1");
	
	if( flowtags_is_connected() ){
	
		fprintf(stdout, "newTag: %d \n", flowtags_get_action(0, 167772161));
    /*
		fprintf(stdout, "newTag: %d \n", flowtags_get_action(0,1));
		fprintf(stdout, "newTag: %d \n", flowtags_get_action(0,2));
    */


		 
    /*
		ftuple = flowtags_get_fivetuple(11);
		if( ftuple != NULL){
			fprintf(stdout, "sip: %d \n", ftuple->srcIP );
		}else{
			fprintf(stdout, "controller suggest not to change the five tuple \n");
		}
    */
    /*
		
		ftuple = flowtags_get_fivetuple(0);
		if( ftuple != NULL){
			fprintf(stdout, "sip: %d \n", ftuple->dstIP );
		}else{
			fprintf(stdout, "controller suggest not to change the five tuple \n");
		}
    */
		
		flowtags_disconnect();
	}else{
		fprintf(stdout, "Not connected\n");
	}
  /*
	  flowtags_set_mbid(2);
	flowtags_connect(argv[1]);
	if( flowtags_is_connected() ){
		fprintf(stdout, "newTag: %d \n", flowtags_get_action(56,0));
		fprintf(stdout, "newTag: %d \n", flowtags_get_action(56,1));
		fprintf(stdout, "newTag: %d \n", flowtags_get_action(60,0));
		flowtags_disconnect();
	}else{
		fprintf(stdout, "Not connected\n");
	}
  */
	
    return 0;
} 
