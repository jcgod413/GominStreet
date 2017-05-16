#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define PACKET_SIZE 256
#define IDENTIFIER_SIZE 5
#define DELIM 	"  "

const char IDENTIFIER[] = "GOMIN";

enum Response   {
    Response_Fail='0',
    Response_Success,
};

enum Category   {
    Major,
    Minor,
};

enum MajorCate	{
	Major_User='0',
	Major_Room,
	Major_Game,
};

enum UserCate	{
	User_Login='0',
	User_Signup,
    User_Record,
    User_Win,
    User_Lose,
};

enum RoomCate	{
    Room_Create='0',
    Room_List,
    Room_Enter,
    Room_Exit,
    Room_Alert_Enter,
    Room_Alert_Exit,
    Room_Start,
};

enum GameCate	{
    Game_DiceRoll='0',
    Game_Turn,
    Game_Move,
    Game_Buy,
    Game_Pay,
    Game_GoldKey,
    Game_Isolation,
};

struct Message  {
    char identifier[5];
    char category[2];
    char padding;
    char data[248];
};

#endif
