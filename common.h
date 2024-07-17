#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

void logexit(const char *message);

int parse_address(const char *address, const char *port_string, struct sockaddr_storage *storage);

int server_sock_address_init(const char *protocol, const char *port_dtring, struct sockaddr_storage *storage);

struct action {
    int type;
    int coordinates[2];
    int board[4][4];
};

char int_to_char(int n);