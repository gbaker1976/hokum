#define FCPROTO_MAXPACKETLEN 512
#define FCPROTO_MAXBUFLEN 530

typedef enum {
  CMD_REG = 0x01,
  CMD_ACK = 0x02,
  CMD_QUEAS = 0x04,
  CMD_CONT = 0x08
} cmd_kind;

struct fcproto_hdr {
  char uuid[16];
  unsigned int seq;
  unsigned int tot;
  cmd_kind cmd;
};

struct fcproto_pkt {
  struct fcproto_hdr hdr;
  char data[FCPROTO_MAXPACKETLEN];
};

typedef int
(*f_cmd_cb)( struct fcproto_pkt *pkt );

typedef ssize_t
(*f_sender)( int socket, const void *buffer, size_t length, int flags,
	const struct sockaddr *dest_addr, socklen_t dest_len );

extern int
build_packets( cmd_kind type, char data[], struct fcproto_pkt pkt_arr[],
	uuid_t uuid );

extern int
send_cmd( cmd_kind cmd_type, struct sockaddr *addr, uuid_t uuid,
	char data[], f_sender sender );

extern int
send_reg( struct sockaddr *addr, uuid_t uuid, char data[],
	f_sender sender );

extern int
wait_recv( cmd_kind type, struct sockaddr *addr, uuid_t uuid, f_cmd_cb cb );
