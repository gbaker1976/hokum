#define FCPROTO_MAXPACKETLEN 512
#define FCPROTO_MAXBUFLEN 530

typedef enum {
  CMD_REG = 0x01,
  CMD_ACK = 0x02,
  CMD_QUEAS = 0x04,
  CMD_CONT = 0x08
} cmd_kind;

typedef int (*proto_recv_cb)( int socket, uuid_t uuid );

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

extern int build_packets( cmd_kind type, char data[], struct fcproto_pkt pkt_arr[], uuid_t uuid );
extern int send_reg( int socket, struct sockaddr *addr, uuid_t uuid, char data[] );
extern int wait_recv( cmd_kind type, uuid_t uuid );
