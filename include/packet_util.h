#ifndef _PACKET_UTIL_H
#define _PACKET_UTIL_H

#include "command.h"
#include "ioutil.h"
#include "inetutil.h"

#define MAX_PACKET_SIZE 1034
#define HEADER_SIZE CMD_STR_LEN + 6

struct packet {
	char  cmd[CMD_STR_LEN];
	short index;
	short TTL;
	short data_len;
	char  data [1024];
};

char buf[MAX_PACKET_SIZE + 1];

int new_packet(struct packet * to_create, char* cmd,short index,char* data,short data_len,short ttl);

void pckcpy(struct packet *dest, const struct packet *src);

int new_join_packet(struct packet *pck, short index);

int new_join_packet_rate(struct packet *pck, short index , long rate);

int new_ack_packet(struct packet *pck, short index);

int new_ping_packet(struct packet *pck, short index);

int new_pong_packet(struct packet *pck, short index);

int new_promote_packet(struct packet *pck, short index);

int new_redirect_packet(struct packet *pck, short index, struct sockaddr_in *child);

int get_pcklen(struct packet *input_pkg);

void pck_to_b(char *str, const struct packet *input);

void b_to_pck(const char *input, struct packet *output);

int recv_packet(int socksd, struct packet *pck);

int recvfrom_packet(int socksd, struct sockaddr_in *addr, struct packet *pck, int *len);

int send_packet(int socksd, const struct sockaddr_in *addr, const struct packet *pck);


#endif