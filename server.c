#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char **argv) {

    //getting revealed board
    FILE *file;

    file = fopen(argv[4], "r");
    if(file == NULL){
        printf("Cannot open file\n");
        return 1;
    }

    int revealed_board[4][4];
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            fscanf(file, "%d, ", &revealed_board[i][j]);
        }
    }

    //printing revealed board
    char char_board;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            char_board = int_to_char(revealed_board[i][j]);
            printf("%c\t\t", char_board);
        }
        printf("\n");
    }

    //initianting server (handles ipv4 and ipv6)
    struct sockaddr_storage storage;
    server_sock_address_init(argv[1], argv[2], &storage);

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *address = (struct sockaddr *)(&storage);
    if (0 != bind(s, address, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 1)) {
        logexit("listen");
    }

    struct sockaddr_storage client_storage;
    struct sockaddr *client_address = (struct sockaddr *)(&client_storage);
    socklen_t client_address_length = sizeof(client_storage);

    while (1) {

        //establishing connection with client
        int client_sock = accept(s, client_address, &client_address_length);

        if (client_sock == -1) {
            logexit("accept");
        }

        printf("client connected\n");

       struct action message;
       struct action game_board;
       int moves_left = 16;
       int x, y;

       while(1){

        //receiving message
        size_t count = recv(client_sock, &message, sizeof(struct action), 0);

        //handling message from client and setting reply
        if(message.type == 0){
            game_board.type = 3;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    game_board.board[i][j] = -2;
                }
            }
        }
        else if(message.type == 1){
            x = message.coordinates[0];
            y = message.coordinates[1];
            game_board.type = 3;

            if(revealed_board[x][y] == -1){
                game_board.type = 8;
                for(int i = 0; i < 4; i++){
                    for(int j = 0; j < 4; j++){
                        game_board.board[i][j] = revealed_board[i][j];
                    }
                }
            }
            else{
                moves_left = moves_left - 1;

                if(moves_left <= 3){
                    game_board.type = 6;
                    for(int i = 0; i < 4; i++){
                        for(int j = 0; j < 4; j++){
                            game_board.board[i][j] = revealed_board[i][j];
                        }
                    }
                }
                else{
                    game_board.board[x][y] = revealed_board[x][y];
                }
            }
        }
        else if(message.type == 2){
            x = message.coordinates[0];
            y = message.coordinates[1];

            game_board.type = 3;
            game_board.board[x][y] = -3;

        }
        else if(message.type == 4){
            x = message.coordinates[0];
            y = message.coordinates[1];

            game_board.type = 3;
            game_board.board[x][y] = -2;
        }
        else if(message.type == 5){
            printf("starting new game\n");
            game_board.type = 3;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    game_board.board[i][j] = -2;
                }
            }
            moves_left = 16;
        }
        else if(message.type == 7){
            printf("client disconnected\n");
            break;
        }

        //sending reply to client
        count = send(client_sock, &game_board, sizeof(struct action), 0);
        if (count != sizeof(struct action)) {
		logexit("send");
	    }
       }

    }

    exit(EXIT_SUCCESS);
}