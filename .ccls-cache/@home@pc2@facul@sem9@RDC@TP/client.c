#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define BUFSZ 1024

int main(int argc, char **argv){
	if(argc < 3) DieWithUserMessage("Fail!", "menos args");
	
	struct sockaddr_storage storage;
	if(0 != addrparse(argv[1], argv[2], &storage)) {
		DieWithSystemMessage("Erro no parser!");
	}

	// Socket
	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if(s == -1){
		DieWithSystemMessage("Erro ao abrir socket!");
	}

	// Connect
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		DieWithSystemMessage("Erro ao connectar!");
	}
	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("connect to %s\n", addrstr);

	// Send msg
	char buf[BUFSZ], splitBuf[BUFSZ];
	char del[] = " ";
	memset(buf, 0, BUFSZ);
	memset(splitBuf, 0, BUFSZ);
	printf("Mensagem> ");
	fgets(buf, BUFSZ-1, stdin);
	strcpy(splitBuf, buf);
	char *strPtr = strtok(splitBuf, del);
	char *AstrSplit[3];
	int nSplits = 0;
	while(strPtr){
		AstrSplit[nSplits++] = strPtr;
		strPtr = strtok(NULL, del);
	}
	if(!strcmp(AstrSplit[0], "select")){
		printf("OK!\n");
	}
//	FILE* ptr = fopen("")

	size_t count = send(s, buf, strlen(buf)+1, 0);
	if(count != strlen(buf)+1){
		DieWithSystemMessage("Erro ao enviar!");
	}

	// Recieve Msg
	memset(buf, 0, BUFSZ);
	unsigned total = 0;
	while(1){
		count = recv(s, buf + total, BUFSZ-total, 0);
		if(count == 0){
			// Terminou conexão.
			break;
		}
		total += count;
	}

	close(s);
	printf("Received %u bytes\n", total);
	puts(buf);
	exit(EXIT_SUCCESS);
}
