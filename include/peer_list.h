#ifndef _PEER_LIST_H
#define _PEER_LIST_H


#include "list.h"
#include "inetutil.h"
#include "ioutil.h"

struct peer_node {
	struct sockaddr_in peer_addr;
	unsigned long peer_rate;
	unsigned short dw_port;
	char flag;
};

struct node *peer_list_head;

int curr_p_count;

void insert_peer(const struct sockaddr_in *peer_addr, unsigned long peer_rate, unsigned short dw_port);

void sorted_insert_peer(const struct sockaddr_in *peer_addr, unsigned long peer_rate, unsigned short dw_port);

void remove_peer(const struct sockaddr_in *peer_addr);

void remove_peer_node(struct node *peer_node);

struct node *get_node_peer(const struct sockaddr_in *peer_addr);

void free_list_peer();

struct peer_node *new_peer_node(const struct sockaddr_in *peer_addr, unsigned long rate, unsigned short port); 

struct sockaddr_in *get_addr_head();

#endif
