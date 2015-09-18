#include <string.h>
#include <sys/socket.h>
#include <uuid/uuid.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "fcproto.h"

#define MYPORT "4950"

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * Builds a set of packets for a given command type.
 *
 * type - Will be the command type.
 *
 * data - Will be the data to send.
 *
 * pkt_arr - Will be the array of packets to save the packets to.
 *
 * uuid - Will be a uuid of a previously created packet (if any).
 * If uuid is NULL, the uuid field of the packet headers will be generated.s
 *
 *
 */
int build_packets( cmd_kind type, char data[], struct fcproto_pkt pkt_arr[], uuid_t uuid ) {
  int data_len = strlen( data );
  int num_pkts = sizeof( *pkt_arr ) / sizeof( pkt_arr[0] );

  for( int i = 0; i < num_pkts; i++ ) {
    struct fcproto_pkt pkt;

    pkt.hdr.seq = i;
    pkt.hdr.tot = num_pkts;
    pkt.hdr.cmd = type;

    strcpy( pkt.hdr.uuid, (char *)uuid );
    strcpy( pkt.data, data );

    pkt_arr[ i ] = pkt;
  }

  return num_pkts;
}

/*
 *
 */
int wait_recv( cmd_kind type, uuid_t uuid ) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  struct sockaddr_storage their_addr;
  char buf[FCPROTO_MAXBUFLEN];
  socklen_t addr_len;
  char s[INET6_ADDRSTRLEN];

  memset( &hints, 0, sizeof hints );
  hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
          perror( "can\'t create socket" );
      continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror( "can\'t bind" );
      continue;
    }

    break;
  }

  if (p == NULL) {
    return 2;
  }

  freeaddrinfo(servinfo);

  addr_len = sizeof their_addr;

  if (
      (numbytes = recvfrom(
        sockfd,
        buf,
        FCPROTO_MAXBUFLEN-1,
        0,
        (struct sockaddr *)&their_addr,
        &addr_len
      )) == -1
    )
  {
    perror( "recvfrom" );
    exit(1);
  }

  struct fcproto_pkt packet = *((struct fcproto_pkt *)buf);

  switch( packet.hdr.cmd ) {
    case CMD_REG :

      break;
    case CMD_ACK :

      break;
    case CMD_QUEAS :

      break;
    case CMD_CONT :

      break;
  }

  if ( packet.hdr.cmd & CMD_REG ) {
    printf( "listener: packet is a REG packet\n" );
  }

  if ( packet.hdr.cmd & CMD_ACK ) {
    printf( "listener: packet is an ACK packet \n" );
  }

  if ( packet.hdr.cmd & CMD_QUEAS ) {
    printf( "listener: packet is an QUEAS packet \n" );
  }
  return 1;
}

/*
 *
 */
int send_reg( int socket, struct sockaddr *addr, uuid_t uuid, char data[] ) {
  struct fcproto_pkt pkts[1];
  int n = 0;
  int i;

  if ((n = build_packets( CMD_REG, data, pkts, uuid ))) {
    for( int i = 0; i < n; i++ ) {
      if (
            (sendto(
              socket,
              &pkts[i],
              sizeof(pkts[i]),
              0,
              addr,
              sizeof( *addr )
            )
          ) == -1)
      {
          return -1;
      }

      wait_recv( CMD_ACK, uuid );
    }
  }

  return n;
}

int send_ack( int socket, uuid_t uuid ) {
  return 1;
}

int send_queas( int socket ) {
  return 1;
}

int send_cont( int socket, uuid_t uuid, char data[] ) {
  return 1;
}
