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
	char buf[BUFSZ], splitBuf[BUFSZ];
	FILE *ptr; // = fopen("teste.txt", "r"); // TODO: REMOVE IT;
	char filename[BUFSZ]; // = "teste.txt";
	while(1){
		// Send msg
		char del[] = " ";
		memset(buf, 0, BUFSZ);
		memset(splitBuf, 0, BUFSZ);
		fgets(buf, BUFSZ-1, stdin);
		buf[strlen(buf)-1] = '\0';

		strcpy(splitBuf, buf);
		char *strPtr = strtok(splitBuf, del);
		char *AstrSplit[3]; // To Check extension
		int nSplits = 0;
		while(strPtr){
			AstrSplit[nSplits++] = strPtr;
			strPtr = strtok(NULL, del);
		}
		if(!strcmp(AstrSplit[0], "select")){
			FILE *test_ptr = fopen(AstrSplit[2], "r");
			if(test_ptr){
				ptr = fopen(AstrSplit[2], "r");
				strcpy(filename, AstrSplit[2]);
				char del[] = "."; int extsize = 0;
				char *fileExtension[2];
				char *fileExtensionSplit = strtok(filename, del);
				while(fileExtensionSplit){
					fileExtension[extsize++] = fileExtensionSplit;
					fileExtensionSplit = strtok(NULL, del);
				}
				if(extsize == 0) printf("%s not valid!", AstrSplit[2]);
				char *validExtensions[] = {"txt", "c", "cpp", "py", "tex", "java"};
				int valid = 0;
				for(int i=0;i<6;++i) {
					if(!strcmp(validExtensions[i], fileExtension[extsize-1])){
						valid = 1;
						break;
					}
				}
				if(valid) {
					strcpy(filename, AstrSplit[2]);
					printf("%s selected\n", AstrSplit[2]);
				}else{
					printf("%s not valid!\n", AstrSplit[2]);
					ptr = NULL;
				}

			}else{
				printf("%s does not exist\n", AstrSplit[2]);
			}
		}else if(!strcmp(AstrSplit[0], "send")){
			if(ptr){
				ptr = fopen(filename, "r");
				if(!ptr) {
					printf("%s does not exist\n", filename);
					continue;
				}
				char content[BUFSZ];
				memset(content, 0, BUFSZ);
				strcpy(content, filename);
				content[strlen(content)] = '|';
				int contentOffset = strlen(content);
				while(fgets(content+contentOffset, BUFSZ-contentOffset, ptr)){
					contentOffset = strlen(content);
				}
				content[contentOffset] = '\\';
				content[contentOffset+1] = 'e';
				content[contentOffset+2] = 'n';
				content[contentOffset+3] = 'd';
				// SEND
				size_t count = send(s, content, strlen(content), 0);
				if(count != strlen(content)){
					DieWithSystemMessage("Erro ao enviar!");
				}
				// Recieve Msg
				memset(buf, 0, BUFSZ);
				unsigned total = 0;
				while(1){
					size_t count = recv(s, buf + total, BUFSZ-total, 0);
					total += count;
					if(count == 0) break;
					if(buf[total-4] == '\\' && buf[total-3] == 'e' && buf[total-2] == 'n' && buf[total-1] == 'd') break;
				}
				buf[strlen(buf)-4] = '\0';
				printf("%s\n", buf);
			}else{
				printf("no file selected!\n");
			}
		}else if(!strcmp(AstrSplit[0], "exit")){
			char exit[8];
			memset(exit, 0, 8);
			strcpy(exit, AstrSplit[0]);
			exit[4] = '\\';
			exit[5] = 'e';
			exit[6] = 'n';
			exit[7] = 'd';
			size_t count = send(s, exit, 8, 0);
			if(count != 8){
				DieWithSystemMessage("Erro ao enviar!");
			}
			memset(buf, 0, BUFSZ);
			unsigned total = 0;
			while(1){
				size_t count = recv(s, buf + total, BUFSZ-total, 0);
				total += count;
				if(count == 0) break;
				if(buf[total-4] == '\\' && buf[total-3] == 'e' && buf[total-2] == 'n' && buf[total-1] == 'd') break;
			}
			buf[strlen(buf)-4] = '\0';
			printf("%s\n", buf);
			break;
		}else{
		}

	}
	if(ptr != NULL) fclose(ptr);
	close(s);
	exit(EXIT_SUCCESS);
}
