typedef enum {
  CMD_REG = 01,
  CMD_ACK = 02,
  CMD_QUEAS = 04
} cmd_kind;

struct fcproto_hdr {
  unsigned int seq;
  unsigned int tot;
  cmd_kind cmd;
};

struct fcproto_pkt {
  struct fcproto_hdr hdr;
  char data[512];
};

#define MAXBUFLEN 530
