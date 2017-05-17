using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Global : MonoBehaviour {
    public static int message_size  =       256;
    public static byte padding =            0;
    public static char delim =              ' ';
    public static byte Major_User =         0;
    public static byte Major_Room =         1;
    public static byte Major_Game =         2;
    public static byte User_Login =         0;
    public static byte User_Signup =        1;
    public static byte User_Recore =        2;
    public static byte User_Win =           3;
    public static byte User_Loss =          4;
    public static byte Room_create =        0;
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
}