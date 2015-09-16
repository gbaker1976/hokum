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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "fcproto.h"

#define SERVERPORT 4950    // the port users will be connecting to

// struct fcproto_hdr *build_header( cmd_kind cmd ) {
//
// }
//
// struct fcproto_pkt *build_packet( struct fcproto_hdr *hdr, char *data[] ) {
//
// }

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    int broadcast = 1;
    //char broadcast = '1'; // if that doesn't work, try this

    if (argc != 3) {
        fprintf(stderr,"usage: bcast hostname message\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // this call is what allows broadcast packets to be sent:
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
        sizeof broadcast) == -1) {
        perror("setsockopt (SO_BROADCAST)");
        exit(1);
    }

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(SERVERPORT); // short, network byte order
    their_addr.sin_addr = *( (struct in_addr *)he->h_addr );
    memset( their_addr.sin_zero, '\0', sizeof their_addr.sin_zero );

    struct fcproto_pkt packet;

    packet.hdr.seq = 1;
    packet.hdr.tot = 1;
    packet.hdr.cmd = CMD_ACK;
    strcpy( packet.data, argv[2] );

    if (
          (numbytes = sendto(
            sockfd,
            &packet,
            sizeof(packet),
            0,
            (struct sockaddr *)&their_addr,
            sizeof their_addr
          )
        ) == -1)
    {
        perror("sendto");
        exit(1);
    }

    printf(
      "sent %d bytes to %s\n",
      numbytes,
      inet_ntoa( their_addr.sin_addr )
    );

    close( sockfd );

    return 0;
}
