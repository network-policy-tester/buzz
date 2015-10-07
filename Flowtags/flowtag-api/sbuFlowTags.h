#ifndef FLOWTAGS_H
#define FLOWTAGS_H

//Begin Constant to change 
#define FLOWTAG_CONTROLLER_IP	"127.0.0.1"
#define FLOWTAG_CONTROLLER_PT	2020
#define LOCAL_MB_ID 3
//End Constant to change 

#define FLOWTAG_QUERY  0x00				//	sbuFlowTags->type
#define FLOWTAG_INTAG  0x01				//
#define FLOWTAG_OUTTAG 0x02				//

#define FLOWTAG_TAGFIELD_NOTDEFINED 0x00
#define FLOWTAG_TAGFIELD_TOS        0x01
#define FLOWTAG_TAGFIELD_VLAN       0x02

#define FLOWTAG_SQUID_MISS 0 
#define FLOWTAG_SQUID_HIT 1

#define FLOWTAG_SNORT_OK   0 
#define FLOWTAG_SNORT_FAIL 1
#define FLOWTAG_SNORT_TOTAL_STATES 2

#define FLOWTAG_IPTABLES_ACCEPT   0 

typedef struct sbuFlowTags
{
	int type;
	int mbid;
	int tag;
	int state;					//  used for reply the tagfield, and send to the controller the state of the mb
}sbuFlowTags;

// total 13 bytes, the order of the variables should be in order to avoid padding
// http://stackoverflow.com/questions/119123/why-isnt-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member
typedef struct __attribute__ ((__packed__)) sbuFiveTuple_struct
{
	char protocol;				// 1 byte
	short srcPT;				// 2 bytes
	short dstPT;				// 2 bytes
	int srcIP;					// 4 bytes
	int dstIP;					// 4 bytes
}sbuFiveTuple;

typedef struct sbuFlowTagsWrapper
{
	sbuFlowTags *flowtag_header;
	sbuFiveTuple *five_tuple;
}sbuFlowTagsWrapper;

#endif