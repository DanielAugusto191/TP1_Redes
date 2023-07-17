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
	while(1){
		//accept
		struct sockaddr_storage cstorage;
		struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
		socklen_t caddrlen = sizeof(cstorage);
		int csock = accept(s, caddr, &caddrlen);
		if(csock == -1){
			DieWithSystemMessage("Accept error!");
		}
		char buf[BUFSZ];
		while(1){
			memset(buf, 0, BUFSZ);
			unsigned total = 0;
			while(1){
				size_t count = recv(csock, buf + total, BUFSZ - total, 0);
				if(count < 0) DieWithSystemMessage("Error!");
				total += (int)count;
				if(count <= 0) break;
				if(buf[total-4] == '\\' && buf[total-3] == 'e' && buf[total-2] == 'n' && buf[total-1] == 'd') break;
			}
			if(total == 0) break;
			buf[total-4] = '\0';
			if(!strcmp(buf, "exit")){
				char ans[] = "connection closed\\en";
				ans[strlen(ans)] = 'd';
				size_t count = send(csock, ans, 21, 0);
				if(count != 21) DieWithSystemMessage("bytes erro!");
				close(csock);
				exit(EXIT_SUCCESS);
			}
			// Recovering the file name from the header.
			char filename[BUFSZ];
			strcpy(filename, buf);
			int i = 0;
			while(filename[i] != '|') ++i;
			filename[i] = '\0';
			
			// Check if file exists.
			char ans[BUFSZ];
			memset(ans, 0, BUFSZ);
			if(fopen(filename, "r")){
				remove(filename);
				sprintf(ans, "file %.1000s overwritten", filename);
			} else sprintf(ans, "file %.1000s received", filename);
	
			// Writting in file
			FILE *ptr = fopen(filename, "w+");
			char *toWrite = buf+i+1; // remove file name;
			int k = fputs(toWrite, ptr);
			if(!k){
				sprintf(ans, "Fail to writte file: %.1000s", filename);
			}
			fclose(ptr);
	
			// Removing \0 and putting \end
			int n = strlen(ans);
			ans[n] = '\\';
			ans[n+1] = 'e';
			ans[n+2] = 'n';
			ans[n+3] = 'd';
	
			// SEND
			size_t count = send(csock, ans, strlen(ans), 0);
			if(count != strlen(ans)){
				DieWithSystemMessage("bytes erro!");
			}
		}
		close(csock);
	}
	exit(EXIT_SUCCESS);
}
