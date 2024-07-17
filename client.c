#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {

	struct sockaddr_storage storage;

	//handling ipv4 and ipv6
	parse_address(argv[1], argv[2], &storage);

	//creating socket
	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}

	//connecting to server
	struct sockaddr *address = (struct sockaddr *)(&storage);
	if (0 != connect(s, address, sizeof(storage))) {
		logexit("connect");
	}

	struct action message;
	char input[100];
    char command[100];
	int x = -1;
	int y = -1;
	int error = 0;
	int size_of_input;
	char char_board;

	memset(&message, 0, sizeof(struct action));

	while(1) {

		error = 0;

		//getting input
		fgets(input, sizeof(input), stdin);

        size_of_input = sscanf(input, "%s %d,%d", command, &x, &y);

		//setting message
		if(size_of_input == 1 && strcmp(command, "start") == 0){
			message.type = 0;
		} 
		else if(size_of_input == 3 && strcmp(command, "reveal") == 0){
			if(x < 0 || y < 0 || x > 3 || y > 3){
				printf("error: invalid cell\n");
				error = 1;
			}
			else if(message.board[x][y] >= 0){
				printf("error: cell already revealed\n");
				error = 1;
			}
			else{
				message.type = 1;
				message.coordinates[0] = x;
				message.coordinates[1] = y;
			}
		}
		else if(size_of_input == 3 && strcmp(command, "flag") == 0){
			if(x < 0 || y < 0 || x > 3 || y > 3){
				printf("error: invalid cell\n");
				error = 1;
			}
			else if(message.board[x][y] >= 0){
				printf("error: cannot insert flag in revealed cell\n");
				error = 1;
			}
			else if(message.board[x][y] == -3){
				printf("error: cell already has a flag\n");
				error = 1;
			}
			else{
				message.type = 2;
				message.coordinates[0] = x;
				message.coordinates[1] = y;
			}
		}
		else if(size_of_input == 3 && strcmp(command, "remove_flag" )== 0){
			if(x < 0 || y < 0 || x > 3 || y > 3){
				printf("error: invalid cell\n");
				error = 1;
			}
			else{
				message.type = 4;
				message.coordinates[0] = x;
				message.coordinates[1] = y;
			}
		}
		else if(size_of_input == 1 && strcmp(command, "reset") == 0){
			message.type = 5;
		}
		else if(size_of_input == 1 && strcmp(command, "exit") == 0){
			message.type = 7;
		}
		else{
			printf("error: command not found\n");
			error = 1;
		}	

		//sending message
		if(error == 0){
			size_t count = send(s, &message, sizeof(struct action), 0);
			if (count != sizeof(struct action)) {
				logexit("send");
			}
			if(message.type == 7){
				//closing socket
				close(s);
				break;
			}
		}else if(error == 1){
			continue;
		}

		//receiving message
		size_t count = recv(s, &message, sizeof(struct action), 0);
		if(count == 0) {
			break;
		}

		//handling reply from server
		if(message.type == 3){
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					char_board = int_to_char(message.board[i][j]);
					printf("%c\t\t", char_board);
				}
				printf("\n");
			}

		}
		else if(message.type == 6){
			printf("YOU WIN!\n");
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					char_board = int_to_char(message.board[i][j]);
					printf("%c\t\t", char_board);
				}
				printf("\n");
			}

		}
		else if(message.type == 8){
			printf("GAME OVER!\n");
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					char_board = int_to_char(message.board[i][j]);
					printf("%c\t\t", char_board);
				}
				printf("\n");
			}

		}

		error = 0;

	}

	exit(EXIT_SUCCESS);
}