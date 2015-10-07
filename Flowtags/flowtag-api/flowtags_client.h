#ifndef FLOWTAGS_CLIENT_H
#define FLOWTAGS_CLIENT_H

#include "sbuFlowTags.h"

void flowtags_set_mbid(int mbid);					// store local copy of middle box id

void flowtags_connect(char *controller);			// connect if was not previously connected
int flowtags_is_connected();						// return 1 if connected
void flowtags_disconnect();							// disconnect from the controller

int flowtags_get_tagfield();						// ask to the controller the tag field, and store the result in cache
 
int flowtags_get_action(int tag, int state);		// given a tag and state, return the new tag

// given a request, return the reply.
// if the fivetuple is not needed set it to NULL
sbuFlowTagsWrapper *get_reply_FlowTags(sbuFlowTags *request, sbuFiveTuple *fivetuple);

sbuFlowTagsWrapper *getFlowTags(unsigned char *packet);				// given an incoming FT message return the corresponding C structure 

sbuFiveTuple *flowtags_get_fivetuple(int tag);						// ask to the controller for the tuple

#endif
