#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSZ 1024

int main(int argc, char *argv[]){

	struct sockaddr_storage storage;
	if(0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
		DieWithSystemMessage("Erro no parser!");
	}

	// Socket
	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if(s == -1){
		DieWithSystemMessage("Erro ao abrir socket!");
	}
	int enable = 1;
	if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
		DieWithSystemMessage("Erro setsocket!");
	}

	struct sockaddr *addr = (struct sockaddr *)(&storage);
	//bind
	if(0 != bind(s, addr, sizeof(storage))){
		DieWithSystemMessage("Bind error!");
	}
	//listen
	if(0 != listen(s, 1)) {
		DieWithSystemMessage("listen error!");
	}
	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("bount to %s, wainting...\n", addrstr);
	
	while(1){
		//accept
		struct sockaddr_storage cstorage;
		struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
		socklen_t caddrlen = sizeof(cstorage);
		int csock = accept(s, caddr, &caddrlen);
		if(csock == -1){
			DieWithSystemMessage("Accept error!");
		}
		char caddrstr[BUFSZ];
		addrtostr(caddr, caddrstr, BUFSZ);
		printf("conection from %s.\n", caddrstr);

		char buf[BUFSZ];
		memset(buf, 0, BUFSZ);
		size_t count = recv(csock, buf, BUFSZ, 0);
		printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

		sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
		count = send(csock, buf, strlen(buf)+1, 0);
		if(count != strlen(buf)+1){
			DieWithSystemMessage("bytes erro!");
		}
		close(csock);
	}

	exit(EXIT_SUCCESS);
}
