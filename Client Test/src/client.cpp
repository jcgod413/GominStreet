#include "client.h"

userInfo user_info;

int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in serv_adr;

	if(argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);

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
	
	printf("게임 시작\n");
	char request[PACKET_SIZE];
	int num;		
	user_info.FD = sock;

	while(1) {	
		printf("1. 로그인\n");
		printf("2. 회원가입\n");
		printf("3. 종료\n");
		printf(">> ");
		scanf("%d", &num);	
		getchar();
			
		if(num == 1) 
			login(request);
		else if(num == 2) {
			signup(request);
			continue;
		}
		else if(num == 3) {
			printf("게임 종료\n");
			break;
		}
		else {
			printf("잘못 입력\n");
			continue;
		}
	}

	close(sock);
	exit(0);
}

void init_message(char message[], int c0, int c1) {
	memset(message, 0, PACKET_SIZE);
	snprintf(message, PACKET_SIZE, "GOMIN%d%d ", c0, c1);
}

bool is_valid(char message[], char c0, char c1) {
	char identifier[6];
	int i;

	for(i = 0; i < 5; i++)
		identifier[i] = message[i];	
	identifier[i] = '\0';
	

	if(strcmp(identifier, "GOMIN"))
		return false;

	if(message[5] != c0)
		return false;

	if(message[6] != c1)
		return false;

	return true;
}

void read_message(char response[]) {
	int read_size = 0, total_size = 0;

	while((read_size = read(sock, response, PACKET_SIZE - total_size)) >= 0) {
		total_size += read_size;	
		if(total_size == PACKET_SIZE)
			break;
	}
}
void login(char message[]) {
	char id[ID_SIZE], pw[PW_SIZE], response[PACKET_SIZE];
	int sock = user_info.FD;	
	while(1) {
		printf("로그인\n");
		init_message(message, 0, 0);	
		printf("아이디 : ");
		fgets(id, ID_SIZE - 1, stdin);	
		id[strlen(id) - 1] = '\0';

		printf("비밀번호 : ");
		fgets(pw, PW_SIZE - 1, stdin);	
		pw[strlen(pw) - 1] = '\0';	
		
		strcat(message, id);
		strcat(message, " ");
		strcat(message, pw);
		write(sock, message, PACKET_SIZE);
	
		if(!is_valid(response, '0', '0')) {
			printf("로그인 실패\n");
			continue;
		}
		
		read_message(response);	

		if(response[8] == '0') {
			printf("로그인 실패\n"); 
			continue;
		}
		printf("로그인 성공\n");
		user_info.number = response[8] - '0';
		break;
	}

	
	room_menu();
}

void signup(char message[]) {
	char id[ID_SIZE], pw[PW_SIZE], response[PACKET_SIZE];
	int sock = user_info.FD;
	
	while(1) {
		printf("회원가입\n");
		init_message(message, 0, 1);	
		printf("아이디 : ");
		fgets(id, ID_SIZE - 1, stdin);	
		id[strlen(id) - 1] = '\0';

		printf("비밀번호 : ");
		fgets(pw, PW_SIZE - 1, stdin);	
		pw[strlen(pw) - 1] = '\0';	
		
		strcat(message, id);
		strcat(message, " ");
		strcat(message, pw);
		write(sock, message, PACKET_SIZE);

		read_message(response);	
	
		if(!is_valid(response, '0', '1')) {
			printf("회원가입 실패\n");
			continue;
		}
		
		if(response[8] == '0') {
			printf("동일한 아이디 존재\n"); 
			continue;
		}

		printf("회원가입 성공\n");
		break;
	}
}

void room_menu() {
	int num;

	while(1) {
		printf("방 대기실 입장\n");
		printf("1. 방 생성하기\n");
		printf("2. 방 목록보기\n");
		printf("3. 방 입장하기\n");
		printf("4. 방 대기실 퇴장\n");
		printf(">> ");

		scanf("%d", &num);
		getchar();

		if(num == 1)
			create_room();
		else if(num == 2) {
					
			continue;	
		}
		else if(num == 3) {

			continue;
		}
		else if(num == 4) {
			
			break;
		}
		else {
			printf("잘못 입력\n");
			continue;
		}
	}
}

void create_room() {
	int sock = user_info.FD;
	char request[PACKET_SIZE], response[PACKET_SIZE], room_name[ROOM_NAME];
	init_message(request, 1, 0);

	printf("방 이름을 입력하세요.\n");
	printf(">> ");
	fgets(room_name, ROOM_NAME - 1, stdin);
	room_name[strlen(room_name) - 1] = '\0';
	strcat(request, room_name);
	write(sock, request, PACKET_SIZE);
	read_message(response);	
}

