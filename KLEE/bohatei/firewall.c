#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

locatedPacket firewallProc(locatedPacket inPkt){
	//processing conn being in NULL state
	if (fwConnStates[inPkt.packet.connId] == NUL){
		printf("NULL STATE CASE!!!\n");
		printf("connId is: %d\n",inPkt.packet.connId);
		//simply pass on a non-tcp packet
		if (inPkt.packet.proto != 0)
		{
			printf("non-tcp. return the same\n");
			return inPkt;
		}
	
		//processing TCP SYN from client
		if (inPkt.packet.tcpSYN && !inPkt.packet.tcpACK && inPkt.packet.fromClient){
			fwConnStates[inPkt.packet.connId] = NEW;
			return inPkt;
		}
		else
		{
			fwConnStates[inPkt.packet.connId] = INVALID;
			inPkt.packet.dropped = 1;
			return inPkt;
		}
	}

	//processing conn being in NEW state
	if (fwConnStates[inPkt.packet.connId] == NEW){
		//processing TCP SYN/ACK from server
		if (inPkt.packet.tcpSYN && inPkt.packet.tcpACK && !inPkt.packet.fromClient){
			fwConnStates[inPkt.packet.connId] = ESTABLISHED;
			return inPkt;
		}
		if (inPkt.packet.timeout){
			fwConnStates[inPkt.packet.connId] = NUL;
			return inPkt;
		}
		else
		{
			fwConnStates[inPkt.packet.connId] = INVALID;
			inPkt.packet.dropped = 1;
			return inPkt;
		}
	}

	//processing conn being in ESTABLISHED state
	if (fwConnStates[inPkt.packet.connId] == ESTABLISHED){
		//processing FIN or timeout
		if (inPkt.packet.tcpFIN || inPkt.packet.timeout){
			fwConnStates[inPkt.packet.connId] = NUL;
			return inPkt;
		}
		else
		{
			return inPkt;
		}	
	}

	//processing conn being in INVALID state
	if (fwConnStates[inPkt.packet.connId] == INVALID){
		//processing timeout
		if (inPkt.packet.timeout){
			fwConnStates[inPkt.packet.connId] = NUL;
			return inPkt;
		}
		else
		{
			inPkt.packet.dropped = 1;
			return inPkt;
		}
	}

	//default case. should not get here.
	return inPkt;
}

void showFwState(int connId){
	switch (fwConnStates[connId]){
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


