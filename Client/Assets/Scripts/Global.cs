using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Global : MonoBehaviour {
    //readonly
    public static readonly int message_size  =       256;
    public static readonly byte padding = 0;
    public static readonly string Success = "1";
    public static readonly string Fail = "0";
    public static readonly char delim = ' ';

    public static readonly byte Major_User = 0;
    public static readonly byte Major_Room = 1;
    public static readonly byte Major_Game = 2;

    public static readonly byte User_Login = 0;
    public static readonly byte User_Signup = 1;
    public static readonly byte User_Record = 2;
    public static readonly byte User_Win = 3;
    public static readonly byte User_Loss = 4;

    public static readonly byte Room_Create = 0;
    public static readonly byte Room_List = 1;
    public static readonly byte Room_Enter = 2;
    public static readonly byte Room_Exit = 3;
    public static readonly byte Room_Alert_Enter = 4;
    public static readonly byte Room_Alert_Exit = 5;
    public static readonly byte Room_Start = 6;

    public static readonly byte Game_DiceRoll = 0;
    public static readonly byte Game_Turn = 1;
    public static readonly byte Game_Move = 2;
    public static readonly byte Game_Buy = 3;
    public static readonly byte Game_Pay = 4;
    public static readonly byte Game_GoldKey = 5;
    public static readonly byte Game_Isolation = 6;
    public static readonly byte Game_Salay = 7;
    public static readonly byte Game_Sell_Restaurant = 8;
    public static readonly byte Game_Visit = 9;
    public static readonly byte Game_BuyCheck = 10;

    public static readonly int Normal_Board = 0;
    public static readonly int Golden_Card = 1;
    public static readonly int Start_Board = 2;

    public static readonly int move_scale = 45;
    public static readonly int speed = 5;

    public static readonly int Salay = 40;
    public static readonly int Start_Money = 1000;

    public static readonly Color[] player_Color = { new Color(255, 0, 0), new Color(0, 0, 255), new Color(0, 255, 0), new Color(255, 0, 255) };
    public static readonly Color default_color = new Color(255, 255, 0);

    public static readonly string[] Board_Info = 
    {
        "출발\n월급 100만원을 받습니다.\n\n\n\n",                 //0
        "고기 스토리\n8만원\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "붕구스 밥버거\n15만원\n\n\n\n",
        "지지고\n25만원\n\n\n\n",
        "연래춘\n30만원\n\n\n\n",                                                           //5
        "더피자\n35만원\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "현선이네\n43만원\n\n\n\n",
        "만복\n47만원\n\n\n\n",
        "식중독\n식중동에 걸렸습니다.\n3턴동안 움직이지 못합니다.\n\n\n",                                           //10
        "빈털털이\n54만원\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "엄마손 햄전골\n58만원\n\n\n\n",
        "맨동\n63만원\n\n\n\n",
        "고향 삼계탕\n67만원\n\n\n\n",                                                           //15
        "대나무 골\n68만원\n\n\n\n",
        "스톤\n72만원\n\n\n\n",
        "코웍\n80만원\n\n\n\n",
        "미스 사이공\n88만원\n\n\n\n",
        "식중독\n식중동에 걸렸습니다.\n3턴동안 움직이지 못합니다.\n\n\n",                                                             //20
        "숭실골\n95만원\n\n\n\n",
        "이례 김밥\n96만원\n\n\n\n",
        "황궁\n99만원\n\n\n\n",
        "우마이\n104만원\n\n\n\n",
        "황금열쇠\n\n\n\n\n",                                                             //25
        "도스마스\n107만원\n\n\n\n",
        "진국\n110만원\n\n\n\n",
        "미담길\n112만원\n\n\n\n",
        "청운\n120만원\n\n\n\n",
        "식중독\n식중동에 걸렸습니다.\n3턴동안 움직이지 못합니다.\n\n\n",                                                             //30
        "논두렁\n157만원\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "추억과 김밥\n160만원\n\n\n\n",
        "이모네\n165만원\n\n\n\n",
        "맥도날드\n170만원\n\n\n\n",                                                             //35
        "더키친\n170만원\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "내찜닭\n180만원\n\n\n\n",
        "커피내일\n200만원\n\n\n\n",
    };

    public static readonly int[] Board_Price = 
    {
        0,
        8,
        0,
        15,
        25,
        30,
        35,
        0,
        43,
        47,
        0,
        54,
        0,
        58,
        63,
        67,
        68,
        72,
        80,
        88,
        0,
        95,
        96,
        99,
        104,
        0,
        107,
        110,
        112,
        120,
        0,
        157,
        0,
        160,
        165,
        170,
        170,
        0,
        180,
        200
    };

    public static readonly string[] Board_name = 
    {
        "출발",                                           //0
        "고기 스토리",
        "황금열쇠",
        "붕구스 밥버거",
        "지지고",
        "연래춘",                                          //5
        "더피자",
        "황금열쇠",
        "현선이네",
        "만복",
        "식중독",                                          //10
        "빈털털이",
        "황금열쇠",
        "엄마손 햄전골",
        "맨동",
        "고향 삼계탕",                                       //15
        "대나무 골",
        "스톤",
        "코웍",
        "미스 사이공",
        "식중독",                                             //20
        "숭실골",
        "이례 김밥",
        "황궁",
        "우마이",
        "황금열쇠",                                     //25
        "도스마스",
        "진국",
        "미담길",
        "청운",
        "식중독",                                        //30
        "논두렁",
        "황금열쇠",
        "추억과 김밥",
        "이모네",
        "맥도날드",                                        //35
        "더키친",
        "황금열쇠",
        "내찜닭",
        "커피내일",
    };

    public static readonly string[] Golden_Key =
    {
        "",
        "세무조사",
        "앞으로 3 이동",
        "뒤로 3 이동",
        "발전기금",
        "착한싱당\n방송에 착한 식당으로 소개되었습니다.\n200만원을 받습니다."
    };

    ///////////////////////////////////////////////////////////////////////

    public static bool room_maker;
    public static int room_number;
}