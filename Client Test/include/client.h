#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PACKET_SIZE 256
#define ID_SIZE 50
#define PW_SIZE 50
#define ROOM_NAME 100

struct userInfo	{
	int number;
	int FD;
	int money;
	int rest_turn;
	int position;
};

void init_message(char[], int, int);
bool is_valid(char[], char, char);
void login(int, char[]);
void signup(int, char[]);

#endif
