
#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#define data_size 2048

#define NET_REQ_PING           0
#define NET_REQ_PARTIE_LIST    1
#define NET_REQ_PARTIE_INFO    2
#define NET_REQ_PARTIE_JOIN    3
#define NET_REQ_PARTIE_LEAVE   4
#define NET_REQ_PARTIE_CREATE  5

typedef struct {
    int type;
    int size;
    char data[data_size];
} NetMessage;

#endif

