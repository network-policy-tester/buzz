//topology-related values
#define MAX_NO_OF_NETWIDE_PORTS 20
#define MAX_NO_OF_SWITCHES 3
#define MAX_NO_OF_SWITCH_PORTS 10
#define MAX_NO_OF_HOSTS 3
#define MAX_NO_OF_FIREWALLS 1
#define MAX_NO_OF_PROXIES 1
#define MAX_NO_OF_IPSES 1
#define MAX_NO_OF_NODES 6 //total no of nodes with IP addresses
#define MAX_NO_OF_TAGS 6   //total no of distinct tags

//tag values
#define IRRELEVANT_TAG 0//no need to tag irrelevant traffic at proxy
#define CACHE_MISS_TAG 1//cache hit
#define CACHE_HIT_TAG 2//cache miss
#define REMOTE_RESP_TAG 3//a remote http response first come to the proxy and then the proxy sends it to the client
#define TCP_TRAFFIC_TAG 4//to indicate non-http traffic (i.e., tcp handshake at proxy)
#define IPS_PROCESSED_TAG 5//to indicate packet has been processed by IPS
#define BOHATEIIPS_PROCESSED_TAG 6//to indicate packet has been processed by BOHATEIIPS

//switch forwarding rules (given by forwardingTables.dat)
int nextHop[MAX_NO_OF_NETWIDE_PORTS][MAX_NO_OF_NODES][MAX_NO_OF_NODES][MAX_NO_OF_TAGS];

//to store port number of each host and middlebox
int fwPorts[MAX_NO_OF_FIREWALLS];
int proxyPorts[MAX_NO_OF_PROXIES];
int ipsPorts[MAX_NO_OF_IPSES];
int hostPorts[MAX_NO_OF_HOSTS];//assuming the IP of each host is also its index

//packet-related values
#define TRAFFIC_FILE_NO_OF_FIELDS 18
#define MAX_FIELD_CHARS 16

//firewall-related data types and values
typedef enum {NUL_FW, NEW_FW, INVALID_FW, ESTABLISHED_FW, TIMEOUT_FW} ConnState;
#define  MAX_NO_OF_FLOWS 10
ConnState fwConnStates[MAX_NO_OF_FIREWALLS][MAX_NO_OF_FLOWS];

//ips-related data types and values
typedef enum {Count_0, Count_1, Count_2, Count_3, Count_4, Count_5} srcSynCount;//keeps count of observed SYNs per source
#define  DROP_THRESHOLD 3
#define  MAX_NO_OF_SOURCES 10
srcSynCount ipsConnStates[MAX_NO_OF_IPSES][MAX_NO_OF_SOURCES];

//proxy-related data types and values. a proxy has three types of states, each maintained separately.
#define PROXY_IP 3
#define PROXY_TCP_PORT 5
typedef enum {NUL_PROXY_CLIENT, SYNACKSENT_PROXY_CLIENT, ESTABLISHED_PROXY_CLIENT} ClientSideTcpState;
typedef enum {NUL_PROXY_SERVER, SYNSENT_PROXY_SERVER, ESTABLISHED_PROXY_SERVER} ServerSideTcpState;
typedef enum {NOTCACHED_PROXY, CACHED_PROXY} HttpObjectsState;
#define  MAX_NO_OF_CLIENTS 10
#define  MAX_NO_OF_SERVERS 10
#define  MAX_NO_OF_HTTP_OBJECTS 10
ClientSideTcpState proxyClientSideTcpStates[MAX_NO_OF_PROXIES][MAX_NO_OF_CLIENTS];
ServerSideTcpState proxyServerSideTcpStates[MAX_NO_OF_PROXIES][MAX_NO_OF_SERVERS];
HttpObjectsState proxyHttpObjectsStates[MAX_NO_OF_PROXIES][MAX_NO_OF_HTTP_OBJECTS];

typedef enum {IP_0=0, IP_=1, IP_2=2, IP_3=3} IP;
//this struct represents a tcp segment
typedef struct{
	int id;//this is a globally unique packet id preserved as the packet traverses the network
	IP srcIP;
	IP dstIP;
	int srcPort;
	int dstPort;
	int proto;//0:TCP, 1: UDP
	int isHttp;//0: no, 1; yes 
	int httpGetObj;//-1: not an HTTP GET. Obj_ID: otherwise
	int httpRespObj;//-1: not an HTTP RESP. Obj_ID: otherwise
	int tag;//general-purpose tag (as in flowtags)
	int tcpSYN;//0: unset, 1; set 
	int tcpACK;//0: unset, 1; set 
	int tcpFIN;//0: unset, 1; set 
	int connId;//-1: don't care (i.e., not part of a flow, e.g., for udp packets). otherwise denotes the conn. id.
	int fromClient;//1: src of the packet is a tcp client; 0: src of the packet is a tcp server 
	int timeout;//1: a special packet representing timeout of the connection; 0: otherwise
	int dropped;//1: a special packet representing a dropped packet; 0: otherwise 
	/* FTP */
	int isFtp;//0: no, 1; yes 
	int PortM;//-1: not an PORT cmd. Port_No: otherwise
	int isPortMRes; //0: no, 1; yes 
	int isPASV; //0: no, 1; yes
	int isDatachannel; //0: no, 1; yes 
	int isDatachannelRes; //0: no, 1; yes
}Packet;

//Ports represents points of connection in the network
typedef struct{
	int num;//each port has unique network-wide number
}Port;

typedef struct{
	Packet packet;
	Port port;
}locatedPacket;

//we are assuming ehach host and each middlebox has exactly one port (point of connection)
typedef struct{
	int IP;//assuming the IP of each host is also its index;
	Port port;
}Host;

typedef struct{
	int IP;
	Port port;
}Firewall;

typedef struct{
	int IP;
	Port port;
}IPS;

typedef struct{
	int IP;
	Port port;
}Proxy;

//each link connects two ports
typedef struct{
	Port end1;
	Port end2;
}Link;

//each node has a type, ip/sw_id, port number as in each line of nodes.dat
typedef struct{
	char type[10];
	int index;//index of the node within its type (e.g., fw_0, fw_1)
	Port port;
}Node;

typedef struct{
	int srcIP;
	int dstIP;
	int srcPort;
	int dstPort;
	int proto;//0:TCP, 1: UDP
}Conn;
