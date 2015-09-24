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
int
build_packets( cmd_kind type, char data[], struct fcproto_pkt pkt_arr[],
	uuid_t uuid ) {

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

int
create_socket() {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
    int rv;
	int broadcast = 1;

	memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 0;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
  		perror( "can\'t create socket" );
  		continue;
      }

	  // this call is what allows broadcast packets to be sent:
      if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
          sizeof broadcast) == -1) {
          perror( "setsockopt (SO_BROADCAST)" );
          exit(1);
      }

      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      	close(sockfd);
      	perror( "can\'t bind" );
      	continue;
      }

      break;
    }

    if (p == NULL) {
      return 0;
    }

    freeaddrinfo(servinfo);

	return sockfd;
}


/*
 * Recieves command packets after sending command packets.
 *
 * type - Will be the command type.
 *
 * uuid - Will be a uuid of a previously created packet (if any).
 * If uuid is NULL, the uuid field of the packet headers will be generated.s
 *
 *
 */
int
wait_recv( cmd_kind type, struct sockaddr *addr, uuid_t uuid, f_cmd_cb cb ) {
  int numbytes;
  char buf[FCPROTO_MAXBUFLEN];
  socklen_t addr_len;
  int sockfd;

  addr_len = sizeof addr;

  sockfd = create_socket();

  if (
      (numbytes = recvfrom(
        sockfd,
        buf,
        FCPROTO_MAXBUFLEN-1,
        0,
        addr,
        &addr_len
      )) == -1
    )
  {
    perror( "recvfrom" );
    return 0;
  }

  cb( (struct fcproto_pkt *)buf );

  return 1;
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
int
send_cmd( cmd_kind cmd_type, struct sockaddr *addr, uuid_t uuid,
	char data[], f_sender sender ) {

  struct fcproto_pkt pkts[1];
  int n = 0;
  int i;
  int sockfd;

  sockfd = create_socket();

  if ((n = build_packets( cmd_type, data, pkts, uuid ))) {
    for( int i = 0; i < n; i++ ) {
      if (
            (sender(
              sockfd,
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
    }
  }

  return n;
}


/*
 * Sends a REG command packet.
 *
 * socket - A socket.
 *
 * addr - Address structure for destination.
 *
 * uuid - Will be a uuid of a previously created packet (if any).
 * If uuid is NULL, the uuid field of the packet headers will be generated.s
 *
 * data - Array of characters to send a payload.
 *
 */
int
send_reg( struct sockaddr *addr, uuid_t uuid, char data[],
	f_sender sender ) {
  return send_cmd( CMD_REG, addr, uuid, data, sender );
}


/*
 * Sends a REG command packet.
 *
 * socket - A socket.
 *
 * addr - Address structure for destination.
 *
 * uuid - Will be a uuid of a previously created packet (if any).
 * If uuid is NULL, the uuid field of the packet headers will be generated.s
 *
 */
int
send_ack( struct sockaddr *addr, uuid_t uuid, f_sender sender ) {
  return send_cmd( CMD_ACK, addr, uuid, NULL, sender );
}


/*
 * Sends a QUEAS command packet.
 *
 * socket - A socket.
 *
 * addr - Address structure for destination.
 *
 * uuid - Will be a uuid of a previously created packet (if any).
 * If uuid is NULL, the uuid field of the packet headers will be generated.s
 *
 */
int
send_queas( struct sockaddr *addr, uuid_t uuid, char data[],
	f_sender sender ) {
  return send_cmd( CMD_QUEAS, addr, uuid, data, sender );
}


/*
 * Sends a CONT command packet.
 *
 * socket - A socket.
 *
 * addr - Address structure for destination.
 *
 * uuid - Will be a uuid of a previously created packet (if any).
 * If uuid is NULL, the uuid field of the packet headers will be generated.s
 *
 */
int
send_cont( struct sockaddr *addr, uuid_t uuid, char data[],
	f_sender sender ) {
  return send_cmd( CMD_CONT, addr, uuid, data, sender );
}
