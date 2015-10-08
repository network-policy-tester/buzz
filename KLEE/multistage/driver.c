#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <klee/klee.h>

Link linkParser(char *linksLineStr){
	char *a[3];
	int n=0, i;
 
	a[n]=strtok(linksLineStr, "\t");

	while(a[n] && (n<4))
		a[++n] = strtok(NULL, "\t");

	Link l;
	l.end1.num = atoi(a[0]);
	l.end2.num = atoi(a[1]);

	return l;
}

Node nodeParser(char *nodesLineStr){
	char a[3][11];
	int n=0, i;

	char *token;
	token = strtok(nodesLineStr ,"\t");

	while((token != NULL) && (n<3)){
		strcpy(a[n++], token);
		token = strtok(NULL, "\t");
	}

	Node node;
	strcpy(node.type, a[0]);
	node.index = atoi(a[1]);
	node.port.num = atoi(a[2]);

	return node;
}

void forwardingTablesParser(char *forwardingTablesFileLineStr){
	char a[6][10];
	int n=0, i;

	char *token;
	token = strtok(forwardingTablesFileLineStr ,"\t");

	while((token != NULL) && (n<6)){
		strcpy(a[n++], token);
		token = strtok(NULL, "\t");
	}

	int inPort = atoi(a[1]);
	int srcIP = atoi(a[2]);
	int dstIP = atoi(a[3]);
	int tag = atoi(a[4]);
	int outPort = atoi(a[5]);

	nextHop[inPort][srcIP][dstIP][tag] = outPort;
}

void packetParser(char* pktStr, locatedPacket *pkt){
	//char *a[TRAFFIC_FILE_NO_OF_FIELDS];
	char *a[128];
	int n=0, i;
 
	a[n]=strtok(pktStr, "\t");

	while(a[n] && (n<TRAFFIC_FILE_NO_OF_FIELDS))
		a[++n] = strtok(NULL, "\t");

	// locatedPacket pkt;
	pkt->packet.id = atoi(a[0]);
	pkt->packet.srcIP = atoi(a[1]);
	pkt->packet.dstIP = atoi(a[2]);
	pkt->packet.srcPort = atoi(a[3]);
	pkt->packet.dstPort = atoi(a[4]);
	pkt->packet.proto = atoi(a[5]);
	pkt->packet.isHttp = atoi(a[6]);
	pkt->packet.httpGetObj = atoi(a[7]);
	pkt->packet.httpRespObj = atoi(a[8]);
	pkt->packet.tag = atoi(a[9]);
	pkt->packet.tcpSYN = atoi(a[10]);
	pkt->packet.tcpACK = atoi(a[11]);
	pkt->packet.tcpFIN = atoi(a[12]);
	pkt->packet.connId = atoi(a[13]);
	pkt->packet.fromClient = atoi(a[14]);
	pkt->packet.timeout = atoi(a[15]);
	pkt->packet.dropped = atoi(a[16]);
	pkt->port.num = atoi(a[17]);

//	return pkt;
}

locatedPacket bohateiIPSProc(int ipsIndex, locatedPacket inPkt){
	//first we set the outpacket to be the same as in packet
	locatedPacket outPkt;
	outPkt = inPkt;
	outPkt.packet.tag = BOHATEIIPS_PROCESSED_TAG;//indicates that the packet is processed by the IPS
	
	// TODO add to port list
	outPkt.port.num = ipsPorts[ipsIndex];
	if (ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] == Diff_0)
	{
		printf("WOOT1\n");	
		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT1\n");	
        		ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] = Diff_1;
			printf("in this WOOT1 case, ipsDiffConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP]);
			if (DIFF_THRESHOLD == 1)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
	}

	if (ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] == Diff_1){
		printf("WOOT2\n");	
		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT2\n");	
        		ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] = Diff_2;
			printf("in this WOOT2 case, ipsDiffConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP]);

			if (DIFF_THRESHOLD == 2)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
	}

	if (ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] == Diff_2){
			if (DIFF_THRESHOLD == 2)
				outPkt.packet.dropped = 1;
			return outPkt; 
	}

	return outPkt; 

	/*
	if (inPkt.packet.tcpSYN){
		// SYNCount++
	}

	if (inPkt.packet.tcpACK){
		// ACKCount++
	}

	// DiffCount = SYNCount - ACKCount;

	if (DiffCount == SYNCount){
		outPkt.packet.dropped = 1;
	}
	else if (DiffCount > 0){
		// forward to IPS proxy
	}
	else if (DiffCount == 0){
		// pass
	}
	else{
		// print error
	}

	*/
}

locatedPacket heavyIPSProc(int ipsIndex, locatedPacket inPkt){
	//first we set the outpacket to be the same as in packet
	locatedPacket outPkt;
	outPkt = inPkt;
	outPkt.packet.tag = HEAVYIPS_PROCESSED_TAG;//indicates that the packet is processed by the IPS
	
	// TODO add to port list
	outPkt.port.num = hipsPorts[ipsIndex];
	if (inPkt.packet.signature == 1){
		printf("WOOT drop\n");	
		outPkt.packet.dropped = 1;
		return outPkt;
	}
	printf("WOOT pass\n");	

	return outPkt; 
}

locatedPacket lightIPSProc(int ipsIndex, locatedPacket inPkt){
	//first we set the outpacket to be the same as in packet
	locatedPacket outPkt;
	outPkt = inPkt;
	outPkt.packet.tag = LIGHTIPS_PROCESSED_TAG;//indicates that the packet is processed by the IPS
	
	// TODO add to port list
	outPkt.port.num = ipsPorts[ipsIndex];
	if (ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] == Diff_0)
	{
		printf("WOOT1\n");	
		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT1\n");	
        		ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] = Diff_1;
			printf("in this WOOT1 case, ipsDiffConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP]);
			if (DIFF_THRESHOLD == 1)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
	}

	if (ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] == Diff_1){
		printf("WOOT2\n");	
		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT2\n");	
        		ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] = Diff_2;
			printf("in this WOOT2 case, ipsDiffConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP]);

			if (DIFF_THRESHOLD == 2)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
	}

	if (ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] == Diff_2){
		printf("WOOT3\n");	
		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT3\n");	
        		ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP] = Diff_3;
			printf("in this WOOT2 case, ipsDiffConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsDiffConnStates[ipsIndex][inPkt.packet.srcIP]);

			if (DIFF_THRESHOLD == 3)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
	}

	return outPkt; 

}


locatedPacket ipsProc(int ipsIndex, locatedPacket inPkt){
	//first we set the outpacket to be the same as in packet
	locatedPacket outPkt;
	outPkt = inPkt;
	outPkt.packet.tag = IPS_PROCESSED_TAG;//indicates that the packet is processed by the IPS

	outPkt.port.num = ipsPorts[ipsIndex];
/*
       	//simply pass on unless we got a SYN packets or timeout 
	if (!(((inPkt.packet.tcpSYN == 1) && (inPkt.packet.tcpACK == 0)) || (inPkt.packet.timeout == 1))){
		printf("WOOT0\n");	
      		return outPkt;
       	}
*/
	//Otherwise, we should count up if we see SYN from an existing source,
	//or count down if we get time out regarding that source
        if (ipsConnStates[ipsIndex][inPkt.packet.srcIP] == Count_0)

//			klee_assert(0);

		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT1\n");	
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_1;
			printf("in this WOOT1 case, ipsConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsConnStates[ipsIndex][inPkt.packet.srcIP]);
			if (DROP_THRESHOLD == 1)
				outPkt.packet.dropped = 1;
			return outPkt;
	}
	
        if (ipsConnStates[ipsIndex][inPkt.packet.srcIP] == Count_1){

		if (inPkt.packet.tcpSYN == 1){
			printf("WOOT2\n");	
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_2;
			printf("in this WOOT2 case, ipsConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsConnStates[ipsIndex][inPkt.packet.srcIP]);


			if (DROP_THRESHOLD == 2)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
		if (inPkt.packet.timeout){
        		ipsConnStates[ipsIndex][outPkt.packet.srcIP] = Count_0;
			return outPkt;
		}
	}
	
        if (ipsConnStates[ipsIndex][inPkt.packet.srcIP] == Count_2){

		if (inPkt.packet.tcpSYN){
			printf("WOOT3\n");	
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_3;
			printf("in this WOOT3 case, ipsConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsConnStates[ipsIndex][inPkt.packet.srcIP]);

//			klee_assert(0);

			if (DROP_THRESHOLD == 3){
				outPkt.packet.dropped = 1;
			}
			return outPkt;
		}
		if (inPkt.packet.timeout){
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_1;
			return outPkt;
		}
	}
	
        if (ipsConnStates[ipsIndex][inPkt.packet.srcIP] == Count_3){
		if (inPkt.packet.tcpSYN){
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_4;
			if (DROP_THRESHOLD == 4)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
		if (inPkt.packet.timeout){
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_2;
			return outPkt;
		}
	}
	
        if (ipsConnStates[ipsIndex][inPkt.packet.srcIP] == Count_4){
		if (inPkt.packet.tcpSYN){
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_5;
			if (DROP_THRESHOLD == 5)
				outPkt.packet.dropped = 1;
			return outPkt;
		}
		if (inPkt.packet.timeout){
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_3;
			return outPkt;
		}
	}

        //default case. should not get here.
        return outPkt;
}

void showbohateiIpsState(int ipsIndex, int srcIP){
        switch (ipsDiffConnStates[ipsIndex][srcIP]){
        case Diff_0:
                printf("\nsrc %d in Count_0 state\n", srcIP);
                break;
        case Diff_1:
                printf("\nsrc %d in Count_1 state\n", srcIP);
                break;
        case Diff_2:
                printf("\nsrc %d in Count_2 state\n", srcIP);
                break;
        case Diff_3:
                printf("\nsrc %d in Count_3 state\n", srcIP);
                break;
        case Diff_4:
                printf("\nsrc %d in Count_4 state\n", srcIP);
                break;
        case Diff_5:
                printf("\nsrc %d in Count_5 state\n", srcIP);
                break;
	}

	printf("*****************************\n");
}
void showIpsState(int ipsIndex, int srcIP){
        switch (ipsConnStates[ipsIndex][srcIP]){
        case Count_0:
                printf("\nsrc %d in Count_0 state\n", srcIP);
                break;
        case Count_1:
                printf("\nsrc %d in Count_1 state\n", srcIP);
                break;
        case Count_2:
                printf("\nsrc %d in Count_2 state\n", srcIP);
                break;
        case Count_3:
                printf("\nsrc %d in Count_3 state\n", srcIP);
                break;
        case Count_4:
                printf("\nsrc %d in Count_4 state\n", srcIP);
                break;
        case Count_5:
                printf("\nsrc %d in Count_5 state\n", srcIP);
                break;
	}

	printf("*****************************\n");
}

locatedPacket firewallProc(int fwIndex, locatedPacket inPkt){
	//first we set the outpacket to be the same as in packet
	locatedPacket outPkt;
	outPkt = inPkt;

	outPkt.port.num = fwPorts[fwIndex];

        //processing conn being in NULL state
        if (fwConnStates[fwIndex][inPkt.packet.connId] == NUL_FW){
                //simply pass on a non-tcp packet
                if (inPkt.packet.proto != 0)
                {
                        return outPkt;
                }

                //processing TCP SYN from client
                if (inPkt.packet.tcpSYN && !inPkt.packet.tcpACK && inPkt.packet.fromClient){
                        fwConnStates[fwIndex][inPkt.packet.connId] = NEW_FW;
                        return outPkt;
                }
                else
                {
                        fwConnStates[fwIndex][inPkt.packet.connId] = INVALID_FW;
                        outPkt.packet.dropped = 1;
                        return outPkt;
                }
        }

        //processing conn being in NEW state
        if (fwConnStates[fwIndex][inPkt.packet.connId] == NEW_FW){
                //processing TCP SYN/ACK from server
                if (inPkt.packet.tcpSYN && inPkt.packet.tcpACK && !inPkt.packet.fromClient){
                        fwConnStates[fwIndex][inPkt.packet.connId] = ESTABLISHED_FW;
                        return outPkt;
                }
                if (inPkt.packet.timeout){
                        fwConnStates[fwIndex][inPkt.packet.connId] = NUL_FW;
                        return outPkt;
                }
                else
                {
                        fwConnStates[fwIndex][inPkt.packet.connId] = INVALID_FW;
                        outPkt.packet.dropped = 1;
                        return outPkt;
                }
        }

        //processing conn being in ESTABLISHED state
        if (fwConnStates[fwIndex][inPkt.packet.connId] == ESTABLISHED_FW){
                //processing FIN or timeout
                if (inPkt.packet.tcpFIN || inPkt.packet.timeout){
                        fwConnStates[fwIndex][inPkt.packet.connId] = NUL_FW;
                        return outPkt;
                }
                else
                {
                        return outPkt;
                }
        }

        //processing conn being in INVALID state
        if (fwConnStates[fwIndex][inPkt.packet.connId] == INVALID_FW){
                //processing timeout
                if (inPkt.packet.timeout){
                        fwConnStates[fwIndex][inPkt.packet.connId] = NUL_FW;
                        return outPkt;
                }
                else
                {
                        outPkt.packet.dropped = 1;
                        return outPkt;
                }
        }

        //default case. should not get here.
        return outPkt;
}

void showFwState(int fwIndex, int connId){
        switch (fwConnStates[fwIndex][connId]){
        case NUL_FW:
                printf("\nconn %d in NULL state\n", connId);
                break;
        case NEW_FW:
                printf("\nconn %d in NEW state\n", connId);
                break;
        case INVALID_FW:
                printf("\nconn %d in INVALID state\n", connId);
                break;
        case ESTABLISHED_FW:
                printf("\nconn %d in ESTABLISHED state\n", connId);
                break;
        }
	
	printf("*****************************\n");
}

locatedPacket swProc(locatedPacket inPkt){
	locatedPacket outPkt;
	outPkt = inPkt;
//	printf("inside swProc()\n");
	outPkt.port.num = nextHop[inPkt.port.num][inPkt.packet.srcIP][inPkt.packet.dstIP][inPkt.packet.tag];


//	printf("nextHop[%d][%d][%d][%d] = %d\n", inPkt.port.num, inPkt.packet.srcIP, inPkt.packet.dstIP, inPkt.packet.tag, outPkt.port.num);

//	printf("ending swProc()\n");

	return outPkt;
}

void showLocatedPacket(locatedPacket pkt){
	printf("&&& pkt id:%d, srcIP:%d, dstIP:%d tag:%d, dropped:%d, @port:%d\n", pkt.packet.id, pkt.packet.srcIP, pkt.packet.dstIP, pkt.packet.tag, pkt.packet.dropped, pkt.port.num);
}

int main(int argc, char *argv[]){

    //FILE *trafficFile = fopen("testTraffic.dat","r");
    FILE *nodesFile = fopen("nodes.dat","r");
    FILE *linksFile = fopen("links.dat","r");
    FILE *forwardingTablesFile = fopen("forwardingTables.dat","r");

/*
	if (trafficFile == 0){
		printf("Could not open traffic file\n");
		return 1;
	}
*/

	if (nodesFile == 0){
		printf("Could not open links file\n");
		return 1;
	}

	if (linksFile == 0){
		printf("Could not open links file\n");
		return 1;
	}

	if (forwardingTablesFile == 0){
		printf("Could not open links file\n");
		return 1;
	}

	int i;
	int j;

	//initialize ips status for all conns
	for (i=0; i<MAX_NO_OF_IPSES; i++)
		for (j=0; j<MAX_NO_OF_BOHATEI; j++)
			ipsDiffConnStates[i][j] = Diff_0;

//int false = 0;
//if (false){
	//Reading the links file************************************************
    char *currentPacketStr = NULL;
    char *linksFileLineStr = NULL;
    size_t len = 0;

	
	//ignore the first line
	getline(&linksFileLineStr, &len, linksFile);

	int linksPort[MAX_NO_OF_NETWIDE_PORTS];
	for (i=0; i<MAX_NO_OF_NETWIDE_PORTS; i++)
		linksPort[i] = -1;

	while (getline(&linksFileLineStr, &len, linksFile) != -1){
		Link l = linkParser(linksFileLineStr);
		linksPort[l.end1.num] = l.end2.num;
		linksPort[l.end2.num] = l.end1.num;
	}
	
	//Reading the nodes file************************************************
	int noOfPorts = 0;
	Node portInfo[MAX_NO_OF_NETWIDE_PORTS];//each port corresponds to one line of nodes.dat
	for (i=0; i<MAX_NO_OF_NETWIDE_PORTS; i++){
		portInfo[i].index = -1;
		portInfo[i].port.num = -1;
	}

	// bohatei IPS
	// light IPS
	int noOfIPSes = 0;
	for (i=0; i<MAX_NO_OF_IPSES; i++)
		ipsPorts[i] = -1;
	
	// heavy IPS
	int noOfHIPSes = 0;
	for (i=0; i<MAX_NO_OF_IPSES; i++)
		hipsPorts[i] = -1;


	int noOfHosts = 0;
	for (i=0; i<MAX_NO_OF_HOSTS; i++)
		hostPorts[i] = -1;

	int noOfSws = 0;
	int swPorts[MAX_NO_OF_SWITCHES][MAX_NO_OF_SWITCH_PORTS];
	int swPortsSeen[MAX_NO_OF_SWITCHES];

	for (i=0; i<MAX_NO_OF_SWITCHES; i++){
		swPortsSeen[i] = 0;
		for (j=0; j<MAX_NO_OF_SWITCH_PORTS; j++)
			swPorts[i][j] = -1;
	}

       	char *nodesFileLineStr = NULL;

	//ignore the first line
	getline(&nodesFileLineStr, &len, nodesFile);

	while (getline(&nodesFileLineStr, &len, nodesFile) != -1){
		Node node = nodeParser(nodesFileLineStr);

		//this is to have all info about each port in one place as in node.dat
		strcpy(portInfo[node.port.num].type, node.type);
		portInfo[node.port.num].index = node.index;
		portInfo[node.port.num].port.num = node.port.num;

//		printf("%s\n", node.type);

		// bohatei node type
		if (node.type[0] == 'b')
			ipsPorts[noOfIPSes++] = node.port.num;
		if (node.type[0] == 'l')
			ipsPorts[noOfIPSes++] = node.port.num;
		if (node.type[0] == 'H')
			hipsPorts[noOfHIPSes++] = node.port.num;
		if (node.type[0] == 'h')
			hostPorts[noOfHosts++] = node.port.num;
		if (node.type[0] == 's'){
			swPorts[node.index][swPortsSeen[node.index]++] = node.port.num;
			if (node.index >= noOfSws)
				noOfSws = node.index + 1;
		}
	}

/*
	printf("ips ports:\n");
	for (i=0; i<noOfIPSes; i++)
		printf("%d\n", ipsInfo[i]);

	printf("proxy ports:\n");
	for (i=0; i<noOfProxies; i++)
		printf("%d\n", proxyInfo[i]);

	printf("fw ports:\n");
	for (i=0; i<noOfFws; i++)
		printf("%d\n", fwInfo[i]);

	printf("host ports:\n");
	for (i=0; i<noOfHosts; i++)
		printf("%d\n", hostInfo[i]);

	printf("sw info:\n");
	for (i=0; i<noOfSws; i++)
		for (j=0; j < swPortsSeen[i]; j++)
			printf("sw %d, port %d: %d\n", i, j, swInfo[i][j]);

	printf("*********printing port info*********\n");
	for (i=0; i<MAX_NO_OF_NETWIDE_PORTS; i++){
		printf("%d %d %s\n",portInfo[i].index, portInfo[i].port.num, portInfo[i].type);
	}
	printf("*********done printing port info*********\n");
*/

	//Reading the forwarding tables file************************************************
	int k;
	int l;

	for (i=0; i< MAX_NO_OF_NETWIDE_PORTS; i++)
		for (j=0; j< MAX_NO_OF_NODES; j++)
			for (k=0; k< MAX_NO_OF_NODES; k++)
				for (l=0; l< MAX_NO_OF_TAGS; l++)
					nextHop[i][j][k][l] = -1;

       	char *forwardingTablesFileLineStr = NULL;

	//ignore the first line
	getline(&forwardingTablesFileLineStr, &len, forwardingTablesFile);

	while (getline(&forwardingTablesFileLineStr, &len, forwardingTablesFile) != -1){
		forwardingTablesParser(forwardingTablesFileLineStr);
	}

/*
	printf("!!!!forwarding tables!!!!\n");
	for (i=0; i< MAX_NO_OF_NETWIDE_PORTS; i++)
		for (j=0; j< MAX_NO_OF_NODES; j++)
			for (k=0; k< MAX_NO_OF_NODES; k++)
				for (l=0; l< MAX_NO_OF_TAGS; l++)
					if (nextHop[i][j][k][l] >= 0)
						printf("inport:%d, srcIP:%d, dstIP:%d, tag:%d:::outport%d\n",i,j,k,l,nextHop[i][j][k][l]);
	printf("!!!!end of forwarding tables!!!!\n");
*/
	//this is a test to see if an injected packet can follow through the topology
	int injectionPortNo = 0;
/*
	//Reading the traffic file************************************************
	//ignore the first line
	getline(&currentPacketStr, &len, trafficFile);

	//process one packet at a time
	while (getline(&currentPacketStr, &len, trafficFile) != -1){

		//put together packet data
		locatedPacket pkt;
   		packetParser(currentPacketStr, &pkt);
		
		//printf("got a packet with ID %d\n", pkt.packet.id);
		showLocatedPacket(pkt);


		//move the packet until it arrives its destination or gets dropped
		while ((pkt.port.num != hostPorts[pkt.packet.dstIP]) && (!pkt.packet.dropped)){
			//forward pkt on the link
			pkt.port.num = linksPort[pkt.port.num];
		
				
			showLocatedPacket(pkt);
			//printf("packet ID %d got forwarded by a link\n\n", pkt.packet.id);

			if (portInfo[pkt.port.num].type[0] == 'f'){
				pkt = firewallProc(portInfo[pkt.port.num].index, pkt);
				showLocatedPacket(pkt);
				//printf("packet ID %d got processed by a firewall\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 'i'){
				pkt = ipsProc(portInfo[pkt.port.num].index, pkt);
				showLocatedPacket(pkt);
				printf("packet ID %d got processed by an ips\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 'p'){
				pkt = proxyProc(portInfo[pkt.port.num].index, pkt);
				showLocatedPacket(pkt);
				printf("packet ID %d got processed by a proxy\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 's'){
				pkt = swProc(pkt);
				showLocatedPacket(pkt);
				printf("packet ID %d got processed by a switch\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 'b'){
				pkt = bohateiIPSProc(portInfo[pkt.port.num].index, pkt);
				showLocatedPacket(pkt);
				printf("packet ID %d got processed by a bohatei\n", pkt.packet.id);
			}
			
			printf("proxy state: ");
			// showProxyState(0, 0, 1, 1);
			printf("ips state: ");
			//showIpsState(0, 0);
			showbohateiIpsState(0, 0);
			//printf("ips state: ");
			//showIpsState(0, 3);

			printf("\n");
			printf("%d %d\n", pkt.port.num, hostPorts[pkt.packet.dstIP]);
			
		}

		printf("#############packet fated###################\n");
	}
*/

//automatic test generation block


	int zz;

	locatedPacket pkt1;
	pkt1.packet.id = 1;
	pkt1.packet.srcIP = 0;
	pkt1.packet.dstIP = 1;
	pkt1.packet.dropped = 0;
	pkt1.packet.tag = 0;
	pkt1.packet.isHttp = 0;
	pkt1.packet.timeout = 0;
	pkt1.port.num = 0;

	locatedPacket pkt2;
	pkt2.packet.id = 2;
	pkt2.packet.srcIP = 0;
	pkt2.packet.dstIP = 1;
	pkt2.packet.dropped = 0;
	pkt2.packet.tag = 0;
	pkt2.packet.isHttp = 0;
	pkt2.packet.timeout = 0;
	pkt2.port.num = 0;

	locatedPacket pkt3;
	pkt3.packet.id = 3;
	pkt3.packet.srcIP = 0;
	pkt3.packet.dstIP = 1;
	pkt3.packet.dropped = 0;
	pkt3.packet.tag = 0;
	pkt3.packet.isHttp = 0;
	pkt3.packet.timeout = 0;
	pkt3.port.num = 0;

	locatedPacket pkt4;
	pkt4.packet.id = 4;
	pkt4.packet.srcIP = 0;
	pkt4.packet.dstIP = 1;
	pkt4.packet.dropped = 0;
	pkt4.packet.tag = 0;
	pkt4.packet.isHttp = 0;
	pkt4.packet.timeout = 0;
	pkt4.port.num = 0;


	pkt1.packet.tcpSYN = 1;
	pkt2.packet.tcpSYN = 1;
	pkt3.packet.tcpSYN = 1;
	pkt4.packet.tcpSYN = 1;

	int syn_of_pkt1;
	int syn_of_pkt2;
	int syn_of_pkt3;
	int syn_of_pkt4;

	IP dstIP_of_pkt1;
	IP dstIP_of_pkt2;
	IP dstIP_of_pkt3;
	IP dstIP_of_pkt4;

	klee_make_symbolic(&syn_of_pkt1, sizeof(syn_of_pkt1), "pkt1.packet.tcpSYN");
	klee_make_symbolic(&syn_of_pkt2, sizeof(syn_of_pkt2), "pkt2.packet.tcpSYN");
	klee_make_symbolic(&syn_of_pkt3, sizeof(syn_of_pkt3), "pkt3.packet.tcpSYN");
	klee_make_symbolic(&syn_of_pkt4, sizeof(syn_of_pkt4), "pkt4.packet.tcpSYN");

	memcpy(&pkt1.packet.tcpSYN, &syn_of_pkt1, sizeof(syn_of_pkt1));
	memcpy(&pkt2.packet.tcpSYN, &syn_of_pkt2, sizeof(syn_of_pkt2));
	memcpy(&pkt3.packet.tcpSYN, &syn_of_pkt3, sizeof(syn_of_pkt3));
	memcpy(&pkt4.packet.tcpSYN, &syn_of_pkt4, sizeof(syn_of_pkt4));
	
	pkt1.packet.signature = 1;
	pkt2.packet.signature = 1;
	pkt3.packet.signature = 1;
	pkt4.packet.signature = 1;

	int sig_of_pkt1;
	int sig_of_pkt2;
	int sig_of_pkt3;
	int sig_of_pkt4;

	klee_make_symbolic(&sig_of_pkt1, sizeof(sig_of_pkt1), "pkt1.packet.signature");
	klee_make_symbolic(&sig_of_pkt2, sizeof(sig_of_pkt2), "pkt2.packet.signature");
	klee_make_symbolic(&sig_of_pkt3, sizeof(sig_of_pkt3), "pkt3.packet.signature");
	klee_make_symbolic(&sig_of_pkt4, sizeof(sig_of_pkt4), "pkt4.packet.signature");

	memcpy(&pkt1.packet.signature, &sig_of_pkt1, sizeof(sig_of_pkt1));
	memcpy(&pkt2.packet.signature, &sig_of_pkt2, sizeof(sig_of_pkt2));
	memcpy(&pkt3.packet.signature, &sig_of_pkt3, sizeof(sig_of_pkt3));
	memcpy(&pkt4.packet.signature, &sig_of_pkt4, sizeof(sig_of_pkt4));


/*
	klee_make_symbolic(&(pkt1.packet.tcpSYN), sizeof(pkt1.packet.tcpSYN), "pkt1.packet.tcpSYN");
	klee_make_symbolic(&(pkt2.packet.tcpSYN), sizeof(pkt2.packet.tcpSYN), "pkt2.packet.tcpSYN");
	klee_make_symbolic(&(pkt3.packet.tcpSYN), sizeof(pkt3.packet.tcpSYN), "pkt3.packet.tcpSYN");
	klee_make_symbolic(&(pkt4.packet.tcpSYN), sizeof(pkt4.packet.tcpSYN), "pkt4.packet.tcpSYN");
*/
	
/*
	klee_make_symbolic(&dstIP_of_pkt1, sizeof(dstIP_of_pkt1), "pkt1.packet.dstIP");
	klee_make_symbolic(&dstIP_of_pkt2, sizeof(dstIP_of_pkt2), "pkt2.packet.dstIP");
	klee_make_symbolic(&dstIP_of_pkt3, sizeof(dstIP_of_pkt3), "pkt3.packet.dstIP");
	klee_make_symbolic(&dstIP_of_pkt4, sizeof(dstIP_of_pkt4), "pkt4.packet.dstIP");
	
	memcpy(&pkt1.packet.dstIP, &dstIP_of_pkt1, sizeof(IP));
	memcpy(&pkt2.packet.dstIP, &dstIP_of_pkt2, sizeof(IP));
	memcpy(&pkt3.packet.dstIP, &dstIP_of_pkt3, sizeof(IP));
	memcpy(&pkt4.packet.dstIP, &dstIP_of_pkt4, sizeof(IP));
*/
	
//	klee_assert(pkt1.packet.tcpSYN != 7);

	//make it happen with at most 4 packets
	for (zz=0; zz<4;zz++)
	{
		locatedPacket pkt;
		if (zz==0)
			pkt = pkt1;
		if (zz==1)
			pkt = pkt2;
		if (zz==2)
			pkt = pkt3;
		if (zz==3)
			pkt = pkt4;
		
//		printf("got a packet with ID %d\n", pkt.packet.id);
//		showLocatedPacket(pkt);

		//move the packet until it arrives its destination or gets dropped
		while ((pkt.port.num != hostPorts[pkt.packet.dstIP]) && (!pkt.packet.dropped)){
			//forward pkt on the link
			pkt.port.num = linksPort[pkt.port.num];
			
//			showLocatedPacket(pkt);
//			printf("packet ID %d got forwarded by a link\n\n", pkt.packet.id);

			if (portInfo[pkt.port.num].type[0] == 'f'){
				pkt = firewallProc(portInfo[pkt.port.num].index, pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by a firewall\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 'i'){
				pkt = ipsProc(portInfo[pkt.port.num].index, pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by an ips\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 's'){
				pkt = swProc(pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by a switch\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 'b'){
				pkt = bohateiIPSProc(portInfo[pkt.port.num].index, pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by a switch\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 'l'){
				pkt = lightIPSProc(portInfo[pkt.port.num].index, pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by a switch\n", pkt.packet.id);
			}
			if (portInfo[pkt.port.num].type[0] == 'H'){
				pkt = heavyIPSProc(portInfo[pkt.port.num].index, pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by a switch\n", pkt.packet.id);
			}
			
//			printf("proxy state: ");
//			showProxyState(0, 0, 1, 1);
//			printf("ips state: ");
//			showIpsState(0, 0);


			printf("\n");
		}

		printf("#############packet fated###################\n");
	}

	
	klee_assert(ipsDiffConnStates[0][0] != 3);
	// klee_assert(ipsConnStates[0][0] != 1);
//} // if false

	return 0;
}
