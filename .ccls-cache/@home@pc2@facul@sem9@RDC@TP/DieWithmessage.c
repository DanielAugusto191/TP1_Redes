#include <stdio.h>
#include <stdlib.h>

void DieWithUserMessage(const char *msg, const char *detail) {
	fputs(msg, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

void DieWithSystemMessage(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}
