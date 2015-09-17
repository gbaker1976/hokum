#include <uuid/uuid.h>
#include <string.h>
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
  uuid_t uuid2;

  for( int i = 0; i < num_pkts; i++ ) {
    struct fcproto_pkt pkt;

    pkt.hdr.seq = i;
    pkt.hdr.tot = num_pkts;
    pkt.hdr.cmd = type;

    if ( NULL != uuid ) {
      strcpy( pkt.hdr.uuid, (char *)uuid );
    } else {
      uuid_generate( uuid2 );
      strcpy( pkt.hdr.uuid, (char *)uuid2 );
    }

    strcpy( pkt.data, data );

    pkt_arr[ i ] = pkt;
  }

  return num_pkts;
}
