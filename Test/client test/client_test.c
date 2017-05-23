#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 256
#define LOGIN_SIZE 32

void login(char *message, char *response);

int main(int argc, char *argv[]) {
	int sock;
	char message[BUF_SIZE], response[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)	{
		printf("connect() error!\n");
		return -1;
	}
	else
		puts("Connected...........\n");

	while(1) {
		memset(message, 0, sizeof(message));
		strcpy(message, "GOMIN");
		int c0;
		printf("0. user manager\n");
		printf("1. room manager\n");
		printf("2. game manager\n");
		printf("input number : ");
		scanf("%d", &c0);

		int c1;
		if(c0 == 0) {
			strcat(message, "0");

			printf("0. login\n");
			printf("1. signup\n");
			printf("2. record\n");
			printf("3. win\n");
			printf("4. lose\n");
			printf("input number : ");
			scanf("%d", &c1);
			switch(c1)	{
				case 0:
					strcat(message, "0");
					login(message, response);
					break;
				case 1:
					strcat(message, "1");
					//signup(message, response);
					break;
				case 2:
					strcat(message, "2");
					strcat(message, " 1");	// id
					//record(message, response);
					break;
				case 3:
					strcat(message, "3");
					strcat(message, " 2");	// id
					//win(message, response);
					break;
				case 4:
					strcat(message, "4");
					strcat(message, " 2");	// id
					//lose(message, response);
			}
		}
		else if(c0 == 1) {
			strcat(message, "1");

			printf("0. create\n");
			printf("1. list\n");
			printf("2. enter\n");
			printf("3. exit\n");
			printf("4. alert enter\n");
			printf("5. alert exit\n");
			printf("6. start\n");
			printf("input number : ");
			scanf("%d", &c1);
			switch(c1)	{
				case 0:
					strcat(message, "0");
					strcat(message, " 1");	// id
					break;
				case 1:
					strcat(message, "1");
					//listRoom(message, response);
					break;
				case 2:
					strcat(message, "2");
					strcat(message, " 1");	// room id
					strcat(message, " 1");	// id
					//enterRoom(message, response);
					break;
				case 3:
					strcat(message, "3");
					strcat(message, " 1");	// room id
					strcat(message, " 1");	// id
					printf("message !!!! : %s", message);
					//exitRoom(message, response);
					break;
				case 4:
					strcat(message, "4");
					//enterAlertRoom(message, response);
					break;
				case 5:
					strcat(message, "5");
					//exitAlertRoom(message, response);
					break;
				case 6:
					strcat(message, "6");
					//startRoom(message, response);
			}
		}
		else if(c0 == 2) {
			strcat(message, "2");

			printf("0. dice roll\n");
			printf("1. turn\n");
			printf("2. move\n");
			printf("3. buy\n");
			printf("4. pay\n");
			printf("5. gold key\n");
			printf("6. isolation\n");
			printf("input number : ");
			scanf("%d", &c1);

			switch(c1)	{
				case 0:
					strcat(message, "0");
					//diceRoll(message);
					break;
				case 1:
					strcat(message, "1");
					//turn(message);
					break;
				case 2:
					strcat(message, "2");
					//move(message);
					break;
				case 3:
					strcat(message, "3");
					//buy(message);
					break;
				case 4:
					strcat(message, "4");
					//pay(message);
					break;
				case 5:
					strcat(message, "5");
					//goldKey(message);
					break;
				case 6:
					strcat(message, "6");
					//isolation(message);
			}
		}
			write(sock, message, sizeof(message));
	}
	close(sock);
	return 0;
}

void login(char * message, char *response) {
	char userid[LOGIN_SIZE];
	char passwd[LOGIN_SIZE];

	memset(userid, 0, sizeof(userid));
	memset(passwd, 0, sizeof(passwd));

	printf("ID : ");
	fgets(userid, BUF_SIZE, stdin);
	userid[strlen(userid) - 1] = '\0';
	strcat(message, userid);
	
	printf("PW : ");
	fgets(passwd, BUF_SIZE, stdin);
	passwd[strlen(passwd) - 1] = '\0';

	strcat(message, " ");
	strcat(message, passwd);
}