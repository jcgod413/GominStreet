using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Global : MonoBehaviour {
    public static int message_size  =       256;
    public static byte padding =            0;
    public static string Success =          "1";
    public static string Fail =             "0";
    public static char delim =              ' ';

    public static byte Major_User =         0;
    public static byte Major_Room =         1;
    public static byte Major_Game =         2;
    public static byte User_Login =         0;
    public static byte User_Signup =        1;
    public static byte User_Record =        2;
    public static byte User_Win =           3;
    public static byte User_Loss =          4;
    public static byte Room_Create =        0;
    public static byte Room_List =          1;
    public static byte Room_Enter =         2;
    public static byte Room_Exit =          3;
    public static byte Room_Alert_Enter =   4;
    public static byte Room_Alert_Exit =    5;
    public static byte Room_Start =         6;
    public static byte Game_Turn =          0;
    public static byte Game_Move =          1;
    public static byte Game_Buy =           2;
    public static byte Game_Pay =           3;
    public static byte Game_GoldKey =       4;
    public static byte Game_Isolation =     5;

    public static int Normal_Board = 0;
    public static int Golden_Card = 1;
    public static int Start_Board = 2;

    public static string[] Board_Info = 
    {
        "출발\n반환시 100만원을 받습니다.\n\n\n\n",                 //0
        "고기 스토리\n\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "붕구스 밥버거\n\n\n\n\n",
        "지지고\n\n\n\n\n",
        "연래춘\n\n\n\n\n",                                                           //5
        "더피자\n\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "현선이네\n\n\n\n\n",
        "만복\n\n\n\n\n",
        "식중독\n식중동에 걸렸습니다.\n3턴동안 움직이지 못합니다.\n\n\n",                                           //10
        "빈털털이\n\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "엄마손 햄전골\n\n\n\n\n",
        "맨동\n\n\n\n\n",
        "고향 삼계탕\n\n\n\n\n",                                                           //15
        "대나무 골\n\n\n\n\n",
        "스톤\n\n\n\n\n",
        "코웍\n\n\n\n\n",
        "미스 사이공\n\n\n\n\n",
        "식중독\n식중동에 걸렸습니다.\n3턴동안 움직이지 못합니다.\n\n\n",                                                             //20
        "숭실골\n\n\n\n\n",
        "이례 김밥\n\n\n\n\n",
        "황궁\n\n\n\n\n",
        "우마이\n\n\n\n\n",
        "황금열쇠\n\n\n\n\n",                                                             //25
        "도스마스\n\n\n\n\n",
        "진국\n\n\n\n\n",
        "미담길\n\n\n\n\n",
        "청운\n\n\n\n\n",
        "식중독\n식중동에 걸렸습니다.\n3턴동안 움직이지 못합니다.\n\n\n",                                                             //30
        "논두렁\n\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "추억과 김밥\n\n\n\n\n",
        "이모네\n\n\n\n\n",
        "맥도날드\n\n\n\n\n",                                                             //35
        "더키친\n\n\n\n\n",
        "황금열쇠\n\n\n\n\n",
        "내찜닭\n\n\n\n\n",
        "커피내일\n\n\n\n\n",
    };
}