#include <string.h>
#include <sys/socket.h>
#include <uuid/uuid.h>
#include "fcproto.h"

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

int wait_recv( int socket, uuid_t uuid ) {
  return 1;
}

int send_reg( int socket, struct sockaddr *addr, uuid_t uuid, char data[], proto_recv_cb callback ) {
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

      //callback();
    }
  }

  return n;
}

int send_ack( int socket, uuid_t uuid, proto_recv_cb callback ) {
  return 1;
}

int send_queas( int socket, proto_recv_cb callback ) {
  return 1;
}

int send_cont( int socket, uuid_t uuid, char data[], proto_recv_cb callback ) {
  return 1;
}
