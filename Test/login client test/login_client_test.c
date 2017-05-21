#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 256
#define LOGIN_SIZE 32
void error_handling(char *message);

int main(int argc, char *argv[]) {
	int sock;
	char message[BUF_SIZE], response[BUF_SIZE];
	char userid[LOGIN_SIZE];
	char passwd[LOGIN_SIZE];
	//int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

	memset(message, 0, sizeof(message));
	memset(userid, 0, sizeof(userid));
	memset(passwd, 0, sizeof(passwd));

	strcpy(message, "GOMIN00 ");
	fputs("ID : ", stdout);
	fgets(userid, LOGIN_SIZE, stdin);
	userid[strlen(userid) - 1] = '\0';
	strcat(message, userid);

  	fputs("PW : ", stdout);
	fgets(passwd, BUF_SIZE, stdin);
	passwd[strlen(passwd) - 1] = '\0';

	strcat(message, " ");
	strcat(message, passwd);
  write(sock, message, sizeof(message));

	ssize_t len = read(sock, re)
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
