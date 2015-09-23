#include <stdio.h>
#include "utils.h"
#include <stdlib.h>

typedef enum {NUL=1, NEW, INVALID, ESTABLISHED, TIMEOUT} ConnState;

void showState(ConnState s){
	switch (s){
	case NUL:
  		printf("\nCurrent state: NULL\n\n");
  		break;
	case NEW:
  		printf("\nCurrent state: NEW\n");
  		break;
	case INVALID:
  		printf("\nCurrent state: INVALID\n");
  		break;
	default:
  		printf("\nCurrent state: ESTABLISHED\n");
  		break;
	}
}

void SYN_Observed(ConnState *s){
	if (*s == NUL)
		*s = NEW;
	else 
		if (*s == NEW)
			*s = INVALID;	
}

void SYNACK_Observed(ConnState *s){
	if (*s == NEW)
		*s = ESTABLISHED;
	else
		if (*s == NUL)
			*s = INVALID;
}

void FIN_Observed(ConnState *s){
	if (*s == ESTABLISHED)
		*s = NUL;
	else
		if (*s == NEW)
			*s = INVALID;
		else
			if (*s == NUL)
				*s = INVALID;
}

void ELSE_Observed(ConnState *s){
	if (*s == NUL)
		*s = INVALID;
	else
		if (*s == NEW)
			*s = INVALID;
}

void TIMEOUT_Observed(ConnState *s){
		*s = NUL;
}

int main(){
	ConnState s = NUL;
	showState(s);

	while (1){
		printf("enter the number for the packet or event type:\n1: SYN packet\n2: SYNACK packet\n3: FIN packet\n4: Other packet\n5: TIMEOUT\n");
		int pkt;
		scanf("%d", &pkt);	

		switch (pkt){
		case 1:
			printf("case 1\n");
  			SYN_Observed(&s);
  			break;
		case 2:
			printf("case 2\n");
  			SYNACK_Observed(&s);
  			break;
		case 3:
			printf("case 3\n");
  			FIN_Observed(&s);
  			break;
		case 4:
			printf("case 4\n");
  			ELSE_Observed(&s);
  			break;
		case 5:
			printf("case 5\n");
  			TIMEOUT_Observed(&s);
  			break;
		default:
			printf("case default\n");
			printf("invalid choice");
			return 1;
		}

		showState(s);
	}

	return 0;
}
