#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <klee/klee.h>

//remember which obj is being requested
int pendingHttpObj;
//to remember for whom the proxy is fetching the missing obj
int waitingClientIP;
int waitingClientPort;

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
	char a[3][10];
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

locatedPacket packetParser(char* pktStr){
	char *a[TRAFFIC_FILE_NO_OF_FIELDS];
	int n=0, i;
 
	a[n]=strtok(pktStr, "\t");

	while(a[n] && (n<TRAFFIC_FILE_NO_OF_FIELDS))
		a[++n] = strtok(NULL, "\t");

	locatedPacket pkt;
	pkt.packet.id = atoi(a[0]);
	pkt.packet.srcIP = atoi(a[1]);
	pkt.packet.dstIP = atoi(a[2]);
	pkt.packet.srcPort = atoi(a[3]);
	pkt.packet.dstPort = atoi(a[4]);
	pkt.packet.proto = atoi(a[5]);
	pkt.packet.isHttp = atoi(a[6]);
	pkt.packet.httpGetObj = atoi(a[7]);
	pkt.packet.httpRespObj = atoi(a[8]);
	pkt.packet.tag = atoi(a[9]);
	pkt.packet.tcpSYN = atoi(a[10]);
	pkt.packet.tcpACK = atoi(a[11]);
	pkt.packet.tcpFIN = atoi(a[12]);
	pkt.packet.connId = atoi(a[13]);
	pkt.packet.fromClient = atoi(a[14]);
	pkt.packet.timeout = atoi(a[15]);
	pkt.packet.dropped = atoi(a[16]);
	pkt.port.num = atoi(a[17]);

	return pkt;
}

locatedPacket bohateiIPSProc(int bohateiIPSIndex, locatedPacket inPkt){
	//first we set the outpacket to be the same as in packet
	locatedPacket outPkt;
	outPkt = inPkt;
	outPkt.packet.tag = BOHATEIIPS_PROCESSED_TAG;//indicates that the packet is processed by the IPS
	
	// TODO add to port list
	// outPkt.port.num = ipsPorts[ipsIndex];

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

		if (inPkt.packet.tcpSYN){
			printf("WOOT1\n");	
        		ipsConnStates[ipsIndex][inPkt.packet.srcIP] = Count_1;
			printf("in this WOOT1 case, ipsConnStates[%d][%d] just became %d\n",ipsIndex,inPkt.packet.srcIP, ipsConnStates[ipsIndex][inPkt.packet.srcIP]);
			if (DROP_THRESHOLD == 1)
				outPkt.packet.dropped = 1;
			return outPkt;
	}
	
        if (ipsConnStates[ipsIndex][inPkt.packet.srcIP] == Count_1){

		if (inPkt.packet.tcpSYN){
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

locatedPacket proxyProc(int proxyIndex, locatedPacket inPkt){
	locatedPacket outPkt;
	outPkt = inPkt;

        //packet from client and client tcp being in NULL state
        if ((inPkt.packet.fromClient) && (proxyClientSideTcpStates[proxyIndex][inPkt.packet.srcIP] == NUL_PROXY_CLIENT)){
		if (inPkt.packet.tcpSYN){
			//send a SYNACK to client
       		        outPkt.packet.tcpACK = 1;
       		        outPkt.packet.fromClient = 0;
	        	proxyClientSideTcpStates[proxyIndex][inPkt.packet.srcIP] = SYNACKSENT_PROXY_CLIENT;
			outPkt.packet.dstIP = inPkt.packet.srcIP;
			outPkt.packet.dstPort = inPkt.packet.srcPort;
			outPkt.packet.srcIP = PROXY_IP;
			outPkt.packet.srcPort = PROXY_TCP_PORT;
			outPkt.packet.tag = TCP_TRAFFIC_TAG;

			outPkt.port.num = proxyPorts[proxyIndex];

        		return outPkt;
		}
	}

        //packet from client and client tcp being in SYNACKSENT state
        if ((inPkt.packet.fromClient) && (proxyClientSideTcpStates[proxyIndex][inPkt.packet.srcIP] == SYNACKSENT_PROXY_CLIENT)){
		if (inPkt.packet.tcpACK){
        		proxyClientSideTcpStates[proxyIndex][inPkt.packet.srcIP] = ESTABLISHED_PROXY_CLIENT;
			//no need to respond
			outPkt.port.num = proxyPorts[proxyIndex];
			outPkt.packet.srcIP = PROXY_IP;
			outPkt.packet.srcPort = PROXY_TCP_PORT;

			//just returning the default response, which is "dropped"
			outPkt.packet.dropped = 1;
	        	return outPkt;
		}
	}

        //packet from client and client tcp is already established
        if ((inPkt.packet.fromClient) && (proxyClientSideTcpStates[proxyIndex][inPkt.packet.srcIP] == ESTABLISHED_PROXY_CLIENT)){
		//check if the packet is an HTTP GET
		if (inPkt.packet.httpGetObj){ 
			//check if the HTTP object is cached
			if (proxyHttpObjectsStates[proxyIndex][inPkt.packet.httpGetObj] == CACHED_PROXY){
				inPkt.packet.fromClient = 0;
				outPkt.packet.httpRespObj = inPkt.packet.httpGetObj;

				outPkt.packet.dstIP = inPkt.packet.srcIP;
				outPkt.packet.dstPort = inPkt.packet.srcPort;
				outPkt.packet.srcIP = PROXY_IP;
				outPkt.packet.srcPort = PROXY_TCP_PORT;
				outPkt.packet.tag = CACHE_HIT_TAG;
				outPkt.port.num = proxyPorts[proxyIndex];

	        		return outPkt;
			}
			//if not cached
			else{
				//check the status of tcp connection with the server
        			if (proxyServerSideTcpStates[proxyIndex][inPkt.packet.dstIP] == NUL_PROXY_SERVER){
					//remember which obj is being requested
					pendingHttpObj = inPkt.packet.httpGetObj;

					outPkt.packet.tcpSYN = 1;
					outPkt.packet.tcpACK = 0;
		        		proxyServerSideTcpStates[proxyIndex][inPkt.packet.dstIP] = SYNSENT_PROXY_SERVER;

					//remember for whom the proxy is fetching the missing obj
					waitingClientIP = inPkt.packet.srcIP;
					waitingClientPort = inPkt.packet.srcPort;

					//proxy re-writes its own ip and tcp_port in the outgoing request
					outPkt.packet.srcIP = PROXY_IP;
					outPkt.packet.srcPort = PROXY_TCP_PORT;

					outPkt.port.num = proxyPorts[proxyIndex];
					outPkt.packet.tag = TCP_TRAFFIC_TAG;

	        			return outPkt;
				}
        			if (proxyServerSideTcpStates[proxyIndex][inPkt.packet.dstIP] == ESTABLISHED_PROXY_SERVER){
					//remember for whom the proxy is fetching the missing obj
					waitingClientIP = inPkt.packet.srcIP;
					waitingClientPort = inPkt.packet.srcPort;
					
					//proxy re-writes its own ip and tcp_port in the outgoing request
					outPkt.packet.srcIP = PROXY_IP;
					outPkt.packet.srcPort = PROXY_TCP_PORT;

					outPkt.port.num = proxyPorts[proxyIndex];
					outPkt.packet.tag = CACHE_MISS_TAG;

	        			return outPkt;
				}
			}
		}
	}

	//check if the packet is from server and is an SYNACK
	if ((inPkt.packet.tcpSYN) && (inPkt.packet.tcpACK) && (!inPkt.packet.fromClient)){
		if (proxyServerSideTcpStates[proxyIndex][inPkt.packet.srcIP] == SYNSENT_PROXY_SERVER){
	        	proxyServerSideTcpStates[proxyIndex][inPkt.packet.srcIP] = ESTABLISHED_PROXY_SERVER;
			outPkt.packet.tcpSYN = 0;
			outPkt.packet.tcpACK = 1;
			outPkt.packet.fromClient = 1;
			outPkt.packet.httpGetObj = pendingHttpObj;
			outPkt.packet.srcIP = PROXY_IP;
			outPkt.packet.srcPort = PROXY_TCP_PORT;
			outPkt.packet.dstIP = inPkt.packet.srcIP;
			outPkt.packet.dstPort = inPkt.packet.srcPort;
			outPkt.packet.tag = TCP_TRAFFIC_TAG;
			
			outPkt.port.num = proxyPorts[proxyIndex];

       			return outPkt;
		}
	}

	//check if the packet is from server and is an HTTP RESP
	if ((inPkt.packet.httpRespObj) && (!inPkt.packet.fromClient)){
		if (proxyServerSideTcpStates[proxyIndex][inPkt.packet.srcIP] == ESTABLISHED_PROXY_SERVER){
			proxyHttpObjectsStates[proxyIndex][inPkt.packet.httpRespObj] = CACHED_PROXY;
			inPkt.packet.fromClient = 0;
			outPkt.packet.httpRespObj = inPkt.packet.httpGetObj;
			outPkt.packet.srcIP = PROXY_IP;
			outPkt.packet.srcPort = PROXY_TCP_PORT;
			outPkt.packet.dstIP = waitingClientIP;
			outPkt.packet.dstPort = waitingClientPort;
			outPkt.packet.tag = REMOTE_RESP_TAG;

			outPkt.port.num = proxyPorts[proxyIndex];

	       		return outPkt;
		}
	}
	
	//if the packet is irrelevant, pass it on
//	outPkt.packet.dropped = 1;
	outPkt.packet.tag = IRRELEVANT_TAG;
	return outPkt;
}

void showProxyState(int proxyIndex, int clientIP, int serverIP, int objId){
        switch (proxyClientSideTcpStates[proxyIndex][clientIP]){
        case NUL_PROXY_CLIENT:
                printf("\nproxy, w.r.t. client %d, in NULL state\n", clientIP);
                break;
        case SYNACKSENT_PROXY_CLIENT:
                printf("\nproxy, w.r.t. client %d, in SYNACKSENT state\n", clientIP);
                break;
        case ESTABLISHED_PROXY_CLIENT:
                printf("\nproxy, w.r.t. client %d, in ESTABLISHED state\n", clientIP);
                break;
        }

        switch (proxyServerSideTcpStates[proxyIndex][serverIP]){
        case NUL_PROXY_SERVER:
                printf("\nproxy, w.r.t. server %d, in NULL state\n", serverIP);
                break;
        case SYNSENT_PROXY_SERVER:
                printf("\nproxy, w.r.t. server %d, in SYNSENT state\n", serverIP);
                break;
        case ESTABLISHED_PROXY_SERVER:
                printf("\nproxy, w.r.t. server %d, in ESTABLISHED state\n", serverIP);
                break;
        }

        switch (proxyHttpObjectsStates[proxyIndex][objId]){
        case NOTCACHED_PROXY:
                printf("\nproxy, w.r.t. object %d, in NOTCACHED state\n", objId);
                break;
        case CACHED_PROXY:
                printf("\nproxy, w.r.t. object %d, in CACHED state\n", objId);
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

	// FILE *trafficFile = fopen("./testTraffic.dat","r");
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

	//initialize firewall status for all conns
	for (i=0; i<MAX_NO_OF_FIREWALLS; i++)
		for (j=0; j<MAX_NO_OF_FLOWS; j++)
			fwConnStates[i][j] = NUL_FW;

	//initialize ips status for all conns
	for (i=0; i<MAX_NO_OF_IPSES; i++)
		for (j=0; j<MAX_NO_OF_SOURCES; j++)
			ipsConnStates[i][j] = Count_0;

	//initialize proxy status for all clients, servers, and objects
	for (i=0; i<MAX_NO_OF_PROXIES; i++)
		for (j=0; j<MAX_NO_OF_CLIENTS; j++)
			proxyClientSideTcpStates[i][j] = NUL_PROXY_CLIENT;

	for (i=0; i<MAX_NO_OF_PROXIES; i++)
		for (j=0; j<MAX_NO_OF_SERVERS; j++)
			proxyServerSideTcpStates[i][j] = NUL_PROXY_SERVER;

	for (i=0; i<MAX_NO_OF_PROXIES; i++)
		for (j=0; j<MAX_NO_OF_HTTP_OBJECTS; j++)
			proxyHttpObjectsStates[i][j] = NOTCACHED_PROXY;

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

	int noOfFws = 0;
	for (i=0; i<MAX_NO_OF_FIREWALLS; i++)
		fwPorts[i] = -1;

	int noOfProxies = 0;
	for (i=0; i<MAX_NO_OF_PROXIES; i++)
		proxyPorts[i] = -1;

	int noOfIPSes = 0;
	for (i=0; i<MAX_NO_OF_IPSES; i++)
		ipsPorts[i] = -1;

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

		if (node.type[0] == 'f')
			fwPorts[noOfFws++] = node.port.num;
		if (node.type[0] == 'i')
			ipsPorts[noOfIPSes++] = node.port.num;
		if (node.type[0] == 'p')
			proxyPorts[noOfProxies++] = node.port.num;
		if (node.type[0] == 'h')
			hostPorts[noOfHosts++] = node.port.num;
		if (node.type[0] == 's'){
			swPorts[node.index][swPortsSeen[node.index]++] = node.port.num;
			if (node.index >= noOfSws)
				noOfSws = node.index + 1;
//			printf("DEBUG: %d\n", node.index);
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
		locatedPacket pkt = packetParser(currentPacketStr);
		
		printf("got a packet with ID %d\n", pkt.packet.id);
		showLocatedPacket(pkt);


		//move the packet until it arrives its destination or gets dropped
		while ((pkt.port.num != hostPorts[pkt.packet.dstIP]) && (!pkt.packet.dropped)){
			//forward pkt on the link
			pkt.port.num = linksPort[pkt.port.num];
			
			showLocatedPacket(pkt);
			printf("packet ID %d got forwarded by a link\n\n", pkt.packet.id);

			if (portInfo[pkt.port.num].type[0] == 'f'){
				pkt = firewallProc(portInfo[pkt.port.num].index, pkt);
				showLocatedPacket(pkt);
				printf("packet ID %d got processed by a firewall\n", pkt.packet.id);
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
			
			printf("proxy state: ");
			showProxyState(0, 0, 1, 1);
			printf("ips state: ");
			showIpsState(0, 0);
			printf("ips state: ");
			showIpsState(0, 3);

			printf("\n");
		}

		printf("#############packet fated###################\n");
	}
*/

//automatic test generation block

	int zz;

	locatedPacket pkt1;
	pkt1.packet.id = 1;
	pkt1.packet.srcIP = 0;
//	pkt1.packet.dstIP = 2;
	pkt1.packet.dropped = 0;
	pkt1.packet.tag = 0;
	pkt1.packet.isHttp = 0;
	pkt1.packet.timeout = 0;
	pkt1.port.num = 0;

	locatedPacket pkt2;
	pkt2.packet.id = 2;
	pkt2.packet.srcIP = 0;
//	pkt2.packet.dstIP = 2;
	pkt2.packet.dropped = 0;
	pkt2.packet.tag = 0;
	pkt2.packet.isHttp = 0;
	pkt2.packet.timeout = 0;
	pkt2.port.num = 0;

	locatedPacket pkt3;
	pkt3.packet.id = 3;
	pkt3.packet.srcIP = 0;
//	pkt3.packet.dstIP = 2;
	pkt3.packet.dropped = 0;
	pkt3.packet.tag = 0;
	pkt3.packet.isHttp = 0;
	pkt3.packet.timeout = 0;
	pkt3.port.num = 0;

	locatedPacket pkt4;
	pkt4.packet.id = 4;
	pkt4.packet.srcIP = 0;
//	pkt4.packet.dstIP = 2;
	pkt4.packet.dropped = 0;
	pkt4.packet.tag = 0;
	pkt4.packet.isHttp = 0;
	pkt4.packet.timeout = 0;
	pkt4.port.num = 0;

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
	
	klee_make_symbolic(&dstIP_of_pkt1, sizeof(dstIP_of_pkt1), "pkt1.packet.dstIP");
	klee_make_symbolic(&dstIP_of_pkt2, sizeof(dstIP_of_pkt2), "pkt2.packet.dstIP");
	klee_make_symbolic(&dstIP_of_pkt3, sizeof(dstIP_of_pkt3), "pkt3.packet.dstIP");
	klee_make_symbolic(&dstIP_of_pkt4, sizeof(dstIP_of_pkt4), "pkt4.packet.dstIP");
	
	memcpy(&pkt1.packet.dstIP, &dstIP_of_pkt1, sizeof(IP));
	memcpy(&pkt2.packet.dstIP, &dstIP_of_pkt2, sizeof(IP));
	memcpy(&pkt3.packet.dstIP, &dstIP_of_pkt3, sizeof(IP));
	memcpy(&pkt4.packet.dstIP, &dstIP_of_pkt4, sizeof(IP));
	
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

			if (portInfo[pkt.port.num].type[0] == 'p'){
				pkt = proxyProc(portInfo[pkt.port.num].index, pkt);
//				showLocatedPacket(pkt);
//				printf("packet ID %d got processed by a proxy\n", pkt.packet.id);
			}

			if (portInfo[pkt.port.num].type[0] == 's'){
				pkt = swProc(pkt);
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

	
	klee_assert(ipsConnStates[0][0] != 1);

	return 0;
}
