/*
** broadcaster.c -- a datagram "client" like talker.c, except
**                  this one can broadcast
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "fcproto.h"

#define SERVERPORT 4950    // the port users will be connecting to

int handle_packet_cb ( struct fcproto_pkt *pkt ) {
	return 1;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    //char broadcast = '1'; // if that doesn't work, try this

    if (argc != 3) {
        fprintf(stderr,"usage: bcast hostname message\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info
        perror("gethostbyname");
        exit(1);
    }

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(SERVERPORT); // short, network byte order
    their_addr.sin_addr = *( (struct in_addr *)he->h_addr );
    memset( their_addr.sin_zero, '\0', sizeof their_addr.sin_zero );

    int n;
    uuid_t uuid;
    uuid_generate( uuid );

    if ( (n = send_reg( (struct sockaddr *)&their_addr, uuid, argv[2],
		sendto )) > 0 ) {

			//wait_recv( CMD_ACK, uuid, cb );

	  printf(
        "sent %d bytes to %s\n",
        numbytes,
        inet_ntoa( their_addr.sin_addr )
      );
    } else {
      printf( "Failed to send" );
      exit(1);
    }

    return 0;
}
