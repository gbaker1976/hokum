#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

// The packet length
#define PCKT_LEN 8192

// The IP header's structure
struct ipheader {
	unsigned char      iph_ihl:5, iph_ver:4;
	unsigned char      iph_tos;
	unsigned short int iph_len;
	unsigned short int iph_ident;
	unsigned char      iph_flag;
	unsigned short int iph_offset;
	unsigned char      iph_ttl;
	unsigned char      iph_protocol;
	unsigned short int iph_chksum;
	unsigned int       iph_sourceip;
	unsigned int       iph_destip;
};

// UDP header's structure
struct udpheader {
	unsigned short int udph_srcport;
	unsigned short int udph_destport;
	unsigned short int udph_len;
	unsigned short int udph_chksum;
};
// total udp header length: 8 bytes (=64 bits)

// Function for checksum calculation. From the RFC,
// the checksum algorithm is:
//  "The checksum field is the 16 bit one's complement of the one's
//  complement sum of all 16 bit words in the header.  For purposes of
//  computing the checksum, the value of the checksum field is zero."
unsigned short csum(unsigned short *buf, int nwords) {
	unsigned long sum;
	for ( sum=0; nwords>0; nwords-- ) {
		sum += *buf++;
	}
	sum = ( sum >> 16 ) + ( sum &0xffff );
	sum += ( sum >> 16 );
	return ( unsigned short )( ~sum );
}

int parse_optval( char *val, char *optparts[] ) {
	char *str1;
	unsigned int i;
	char delim = ':';

	for ( i = 0, str1 = val; i < 2; i++, str1 = NULL ) {
		optparts[i] = strtok( str1, &delim );
	}

	return ( NULL == optparts[0] || NULL == optparts[1] ) ? 0 : 1;
}

void usage() {
	printf( "\nUsage:\n" );
	printf( "client [st]\n" );
	printf( "\t-s Specify the source [hostname/ip]:[port] \n" );
	printf( "\t-t Specify the target [hostname/ip]:[port] \n" );
}

int check_args( int argc, char *argv[], char *src[], char *tgt[] ) {
	int c;

	while ( ( c = getopt ( argc, argv, "s:t:" ) ) != -1 ) {
		switch(c) {
			case 's':
				if ( !parse_optval( optarg, src ) ) {
					printf( "invalid -s\n" );
					return 0;
				}
				break;
			case 't':
				if ( !parse_optval( optarg, tgt ) ) {
					printf( "invalid -t\n" );
					return 0;
				}
				break;
			case '?':
				if ( optopt == 's' ) {
					printf( "\n-s parameter requires an option\n" );
				} else if ( optopt == 't' ) {
					printf( "\n-t parameter requires an option\n" );
				} else {
					printf( "\nunrecognized parameter: -%c\n", optopt );
				}
			default:
				usage();
				return 0;
		}
	}

	return 1;
}

// Source IP, source port, target IP, target port from the command line arguments
int main( int argc, char *argv[] ) {
	int sd;
	char *src[2];
	char *tgt[2];

	// No data/payload just datagram
	char buffer[ PCKT_LEN ];

	// Our own headers' structures
	struct ipheader *ip = ( struct ipheader * )buffer;
	struct udpheader *udp = ( struct udpheader * )( buffer + sizeof( struct ipheader ) );

	// Source and destination addresses: IP and port
	struct sockaddr_in sin, din;

	int one = 1;
	const int *val = &one;

	memset( buffer, 0, PCKT_LEN );

	if ( !check_args( argc, argv, src, tgt ) ) {
		exit( -1 );
	}

	// Create a raw socket with UDP protocol
	sd = socket( PF_INET, SOCK_RAW, IPPROTO_UDP );

	if ( sd < 0 ) {
		perror( "socket() error" );
		// If something wrong just exit
		exit( -1 );
	} else {
		printf( "socket() - Using SOCK_RAW socket and UDP protocol is OK.\n" );
	}

	// The source is redundant, may be used later if needed
	// The address family
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;

	// Port numbers
	sin.sin_port = htons( atoi( src[1] ) );
	din.sin_port = htons( atoi( tgt[1] ) );

	// IP addresses
	sin.sin_addr.s_addr = inet_addr( src[0] );
	din.sin_addr.s_addr = inet_addr( tgt[0] );

	// Fabricate the IP header or we can use the
	// standard header structures but assign our own values.
	ip->iph_ihl = 5;
	ip->iph_ver = 4;
	ip->iph_tos = 16; // Low delay
	ip->iph_len = sizeof( struct ipheader ) + sizeof( struct udpheader );
	ip->iph_ident = htons( 54321 );
	ip->iph_ttl = 64; // hops
	ip->iph_protocol = 17; // UDP

	// Source IP address, can use spoofed address here!!!
	ip->iph_sourceip = inet_addr( src[0] );

	// The destination IP address
	ip->iph_destip = inet_addr( tgt[0] );

	// Fabricate the UDP header. Source port number, redundant
	udp->udph_srcport = htons( atoi( src[1] ) );

	// Destination port number
	udp->udph_destport = htons( atoi( tgt[1] ) );
	udp->udph_len = htons( sizeof( struct udpheader ) );

	// Calculate the checksum for integrity
	ip->iph_chksum = csum( ( unsigned short * )buffer, sizeof( struct ipheader ) + sizeof( struct udpheader) );

	// Inform the kernel do not fill up the packet structure. we will build our own...
	if(setsockopt( sd, IPPROTO_IP, IP_HDRINCL, val, sizeof( one ) ) < 0 ) {
		perror( "setsockopt() error" );
		exit( -1 );
	} else {
		printf( "setsockopt() is OK.\n" );
	}

	// Send loop, send for every 2 second for 100 count
	printf("Trying...\n");
	printf("Using raw socket and UDP protocol\n");
	printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n", src[0], atoi(src[1]), tgt[0], atoi(tgt[1]));

	int count;
	for( count = 1; count <=20; count++ ) {
		// Verify
		if( sendto( sd, buffer, ip->iph_len, 0, ( struct sockaddr * )&sin, sizeof( sin ) ) < 0 ) {
			perror( "sendto() error" );
			exit( -1 );
		} else {
			printf( "Count #%u - sendto() is OK.\n", count );
			sleep( 2 );
		}
	}

	close( sd );
	return 0;

}
