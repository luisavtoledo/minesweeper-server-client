all:
	gcc -c common.c
	gcc client.c common.c -o ./bin/client
	gcc server.c common.c -o ./bin/server
