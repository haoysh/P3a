CC=gcc
CFLAGS=-Wall -Werror -lrt -lpthread -g

server_frame: shm_server.c
	$(CC) shm_server.c -o shm_server $(CFLAGS) 
	$(CC) shm_client.c -o shm_client $(CFLAGS)

clean:
	rm shm_server
	rm shm_client
