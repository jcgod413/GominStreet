#define PACKET_SIZE 256
#define IDENTIFIER_SIZE 5
#define DELIM 	"  "

const char IDENTIFIER[] = "GOMIN";

enum Response   {
    Response_Fail,
    Response_Success,
};

enum Category   {
    Major,
    Minor,
};

enum MajorCate	{
	Major_User,
	Major_Room,
	Major_Game,
};

enum UserCate	{
	User_Login,
	User_Signup,
    User_Record,
    User_Win,
    User_Loss,
};

enum RoomCate	{
    Room_Create,
    Room_List,
    Room_Enter,
    Room_Exit,
    Room_Alert_Enter,
    Room_Alert_Exit,
    Room_Start,
};

enum GameCate	{
    Game_DiceRoll,
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