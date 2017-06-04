using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.Collections.Generic;
using System.Text;
using System;
using System.Threading;

public class Room_scene : MonoBehaviour {
    private bool showPopUp = false;
    private bool showGoldKey = false;
    private bool showBuyCheck = false;
    private bool showIsolation = false;
    private bool showMyTurn = false;
    private string GoldKey_message = "";
    private string popup_name = "땅 정보";
    private Board[] board_arr;
    private static PlayerInfo[] player_info;
    private Board target_board;
    private int popup_x = (int)(Screen.width / 3);
    private int popup_y = (int)(Screen.height / 1.5);
    private StringBuilder log = new StringBuilder();
    private int play_index = 0;
    private Thread thread;
    private static bool my_turn;
    private static object lock_obj = new object();
    private static object lock_string = new object();
    private int room_player_id;

    private static int main_thread_turn = 0;
    private static int main_thread_dice;
    private static bool main_thread_check = false;

    private static bool main_thread_log_check = false;
    private static string main_thread_log = "";
    private static bool main_thread_log_finish = true;

    private static bool main_thread_buy = false;

    private static bool main_thread_player_info = false;

    private static bool main_thread_change_player_info = false;

    private static bool showPay = false;
    private static int main_thread_pay_user;
    private static int main_thread_recv_user;
    private static int main_thread_money;

    private static bool end_init = false;

    public Text player_info_text;
    public GameObject Building;
    public Text Log;
	public Texture board_start;
	public Texture board_normal;
	public Texture board_gold;
	public Texture board_isolation;

    class Board
    {
        public int building_count;
        public int owner;
        public int board_id;
        public int board_status;
        public string texture_path;
        public string board_info;
        public string name;
        public int price;
    }

    class PlayerInfo
    {
        public int money;
        public string id;
        public ArrayList board_list = new ArrayList();
        public bool is_empty;
        public int room_player_id;
        public int position;
    }

	// Use this for initialization
	void Start () {
        init_player();
        init_board();
        init_game();
        end_init = true;
        thread = new Thread(play_game);
        thread.Start();
	}

    private void init_game()
    {
        if (!Global.room_maker)
        {
            GameObject.Destroy(GameObject.Find("Cavas/Map/Start_btn"));
        }
    }
    private void play_game()
    {
        while (end_init == false) { }

        while (true)
        {
            Debug.Log("while--");
            string return_message = Socket_script.receive_message();
            if (Socket_script.message_type == Global.Room_Alert_Enter)
            {
                Debug.Log("Room_Alert_Enter");
                string[] parse = return_message.Split(Global.delim);
                int player_num = Int32.Parse(parse[0]);
                Debug.Log("Player Num : "+player_num);
                for (int i = 0; i < player_num; i++)
                {
                    player_info[i].id = parse[i+1];
                    player_info[i].money = Global.Start_Money;
                    player_info[i].is_empty = false;
                    player_info[i].room_player_id = i+1;
                    player_info[i].position = 0;
                    if (Int32.Parse(parse[i + 1]) == Socket_script.user_idx)
                    {
                        this.room_player_id = i + 1;
                    }
                }
                set_log_ready("새로운 플레이어가 접속했습니다.");
            }
            else if (Socket_script.message_type == Global.Room_Start)
            {
                set_log_ready("게임이 시작됩니다.");
                while (true)
                {
                    while (main_thread_log_finish == false) { }
                    return_message = Socket_script.receive_message();
                    if (Socket_script.message_type == Global.Game_Turn)
                    {
                        Debug.Log("Game Turn");
                        lock (lock_obj)
                        {
                            if (this.room_player_id == Int32.Parse(return_message))
                            {
                                my_turn = true;
                                showMyTurn = true;
                            }
                            else
                            {
                                my_turn = false;
                            }
                        }
                        lock (lock_string)
                        {
                            //set_log_ready(("플레이어" + return_message.ToString() + "이(가)순서입니다."));
                            string tmp = Int32.Parse(return_message).ToString();
                            //set_log_ready("플레이어" + return_message.ToString() + "이(가)순서입니다.");
                            set_log_ready("플레이어 " + tmp + "의 순서입니다.");
                        }
                    }
                    else if (Socket_script.message_type == Global.Game_DiceRoll)
                    {
                        string[] token = return_message.Split(Global.delim);
                        int turn = Int32.Parse(token[0]);
                        int dice = Int32.Parse(token[1]);
                        if (turn != 0)
                        {
                            
                        }
                        else
                        {
                            //showIsolation = true;
                        }
                    }
                    else if (Socket_script.message_type == Global.Game_Move)
                    {
                        Debug.Log("Game Move");
                        string[] token = return_message.Split(Global.delim);
                        int turn = Int32.Parse(token[0]);
                        int dice = Int32.Parse(token[1]);
                        main_thread_turn = turn;
                        main_thread_dice = dice;
                        set_log_ready("플레이어 " + main_thread_turn.ToString() + "이(가) 이동했습니다.");
                        main_thread_check = true;
                    }
                    else if (Socket_script.message_type == Global.Game_Buy)
                    {
                        Debug.Log("buy restaurant");
                        string turn = main_thread_turn.ToString();
                        string restaurant = board_arr[player_info[main_thread_turn-1].position].name;
                        set_log_ready("플레이어 " + turn +" 이(가) " + restaurant + "를 구매했습니다.");
                        main_thread_buy = true;
                    }
                    else if (Socket_script.message_type == Global.Game_Pay)
                    {
                        Debug.Log("Game_Pay");
                        string[] token = return_message.Split(Global.delim);
                        Debug.Log(return_message);
                        int chk = Int32.Parse(token[0]);
                        int pay = Int32.Parse(token[1]);
                        int recv = Int32.Parse(token[2]);
                        int money = Int32.Parse(token[3]);

                        if (chk == 1)
                        {
                            if (pay == 0 && recv != 0)
                            {
                                player_info[recv - 1].money += money;
                            }
                            else if (pay != 0 && recv == 0)
                            {
                                player_info[pay - 1].money -= money;
                            }
                            else
                            {
                                player_info[recv - 1].money += money;
                                player_info[pay - 1].money -= money;
                            }

                            main_thread_change_player_info = true;
                            main_thread_pay_user = pay;
                            main_thread_recv_user = recv;
                            main_thread_money = money;
                            showPay = true;
                        }
                    }
                    else if (Socket_script.message_type == Global.Game_GoldKey)
                    {
                        Debug.Log("Golden Key");
                        GoldKey_message = Global.Golden_Key[Int32.Parse(return_message)];
                        Debug.Log("GoldKey message : " + GoldKey_message);
                        showGoldKey = true;
                    }
                    else if (Socket_script.message_type == Global.Game_BuyCheck)
                    {
                        Debug.Log("Buy Check");
                        target_board = board_arr[player_info[main_thread_turn-1].position];
                        showBuyCheck = true;
                    }
                    else if (Socket_script.message_type == Global.Game_Isolation)
                    {
                        Debug.Log("Isolation");
                        showIsolation = true;
                    }
                    else if (Socket_script.message_type == Global.Game_Salay)
                    {
                        Debug.Log("Salay");
                        int turn = Int32.Parse(return_message);
                        player_info[turn - 1].money += Global.Game_Salay;
                        main_thread_change_player_info = true;
                    }
                }
            }
        }
    }

    public void play_game_btn()
    {
        Debug.Log("play_game_btn");
        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_Room);
        sb.Append(Global.Room_Start);
        sb.Append(Global.padding);
        sb.Append(Global.room_number);

        //send
        if (Socket_script.send_message(sb))
        {
            Debug.Log("Send Success");
        }
        else
        {
            Debug.Log("Send Fail");
        }
    }

    public void change_player_info(int player_num)
    {
        string player = main_thread_turn.ToString();
        string money = player_info[player_num - 1].money.ToString();
        StringBuilder text = new StringBuilder();
        text.Append("ID : ");
        text.Append(player);
        text.Append(System.Environment.NewLine);
        text.Append("돈 : ");
        text.Append(money);
        text.Append(System.Environment.NewLine);
        Debug.Log(text.ToString());
        foreach (Board board in player_info[player_num - 1].board_list)
        {
            text.Append(board.board_info.Split()[0]+" ");
        }
        player_info_text.text = text.ToString();
    }

    private void change_money(int player_num, int money)
    {
        player_info[player_num - 1].money = money;
        change_player_info(player_num);
    }

    private void add_board(int player_num, int board_id)
    {
        player_info[player_num - 1].board_list.Add(board_arr[board_id]);
    }

    private void init_player(){
        Debug.Log("Init player()");
        player_info = new PlayerInfo[4];
        for (int i = 0; i < 4; i++)
        {
            player_info[i] = new PlayerInfo();
        }
    }

	// Update is called once per frame
	void Update () {
        if (main_thread_check)
        {
            Debug.Log("move id : " + main_thread_turn);
            if (main_thread_turn == room_player_id)
            {
                GameObject.Find("Canvas/Player/Player"+main_thread_turn).GetComponent<Player_script>().set_send_visit();
            }
            GameObject.Find("Canvas/Player/Player"+main_thread_turn).GetComponent<Player_script>().move_position(player_info[main_thread_turn - 1].position, main_thread_dice);
            player_info[main_thread_turn - 1].position = main_thread_dice;
            main_thread_check = false;
        }
        if (main_thread_log_check)
        {
            lock (lock_string)
            {
                set_log(main_thread_log);
            }
            main_thread_log_check = false;
        }
        if (main_thread_buy)
        {
            board_arr[player_info[main_thread_turn - 1].position].owner = main_thread_turn;
            board_arr[player_info[main_thread_turn - 1].position].building_count++;
            if (board_arr[player_info[main_thread_turn - 1].position].building_count == 1)
            {
                change_board_color(main_thread_turn, player_info[main_thread_turn - 1].position);
            }
            else
            {
                draw_building(player_info[main_thread_turn - 1].position);
            }
            player_info[main_thread_turn - 1].board_list.Add(board_arr[player_info[main_thread_turn - 1].position]);
            player_info[main_thread_turn - 1].money -= board_arr[player_info[main_thread_turn - 1].position].price;
            main_thread_buy = false;
            main_thread_player_info = true;
        }
        if (main_thread_player_info)
        {
            main_thread_change_player_info = true;
            main_thread_player_info = false;
        }
        if (main_thread_change_player_info)
        {
            change_player_info(main_thread_turn);
            main_thread_change_player_info = false;
        }
	}

    void OnGUI()
    {
        if (showBuyCheck)
        {
            GUI.Window(0, new Rect((Screen.width / 2) - popup_x / 2, (Screen.height / 2) - popup_y / 2, popup_x, popup_y), ShowRestaurant, "땅 구매");
        }
        else
        {
            if (showPopUp)
            {
                GUI.Window(0, new Rect((Screen.width / 2) - popup_x / 2, (Screen.height / 2) - popup_y / 2, popup_x, popup_y), ShowGUI, popup_name);
            }
            if (showGoldKey)
            {
                GUI.Window(0, new Rect((Screen.width / 2) - 300 / 2, (Screen.height / 2) - 150 / 2, 300, 150), ShowGoldKey, "황금열쇠");
            }
            if (showIsolation)
            {
                GUI.Window(0, new Rect((Screen.width / 2) - 300 / 2, (Screen.height / 2) - 150 / 2, 300, 150), ShowIsolation, "무인도");
            }
            if (showMyTurn)
            {
                GUI.Window(0, new Rect((Screen.width / 2) - 300 / 2, (Screen.height / 2) - 150 / 2, 300, 150), ShowMyTurn, "순서");
            }
            if (showPay)
            {
                GUI.Window(0, new Rect((Screen.width / 2) - 300 / 2, (Screen.height / 2) - 150 / 2, 300, 150), ShowPay, "지불");
            }
        }
    }

    public void click_board(int board_id)
    {
        target_board = board_arr[board_id];
        showPopUp = true;
    }

    void ShowGoldKey(int windowID)
    {
        //text
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.UpperCenter;
        GUI.Label(new Rect(0, 20, 280, 50), GoldKey_message, centeredTextStyle);

        //button
        if (GUI.Button(new Rect(100, 50, 100, 30), "닫기"))
        {
            showGoldKey = false;
        }
    }

    void ShowIsolation(int windowID)
    {
        //text
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.UpperCenter;
        GUI.Label(new Rect(0, 20, 280, 50), "무인도에 있습니다.", centeredTextStyle);

        //button
        if (GUI.Button(new Rect(100, 50, 100, 30), "닫기"))
        {
            showIsolation = false;
        }
    }

    void ShowMyTurn(int windowID)
    {
        //text
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.UpperCenter;
        GUI.Label(new Rect(0, 20, 280, 50), "주사위를 던지세요", centeredTextStyle);

        //button
        if (GUI.Button(new Rect(100, 50, 100, 30), "닫기"))
        {
            showMyTurn = false;
        }
    }

    void ShowPay(int windowID)
    {
        //text
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.UpperCenter;
        string str;
        if (main_thread_pay_user == 0)
        {
            str = "플레이어 " +main_thread_recv_user + " 가 " + main_thread_money + "을(를) 받았습니다.";
        }
        else if (main_thread_recv_user == 0)
        {
            str = "플레이어 " + main_thread_pay_user + " 가 " + main_thread_money + "을(를) 지불했습니ㅏ.";
        }
        else
        {
            str = "플레이어 " + main_thread_pay_user + " 가 " + main_thread_recv_user+"에게"+ main_thread_money + "을(를) 지불했습니ㅏ.";
        }
        GUI.Label(new Rect(0, 20, 280, 50), str, centeredTextStyle);

        //button
        if (GUI.Button(new Rect(100, 50, 100, 30), "닫기"))
        {
            showPay = false;
        }
    }

    void ShowRestaurant(int windowID)
    {
        int label_size_x = popup_x / 3;
        int label_size_y = popup_y / 6;
        int label_x = popup_x / 2 - label_size_x / 2;
        int label_y = popup_y * 3 / 6;
        int button_size_x = popup_x / 3;
        int button_size_y = popup_y / 12;
        int button_x = popup_x / 2 - button_size_x / 2;
        int button_y = popup_y * 7 / 8;
        int image_size_x = popup_x * 5 / 6;
        int image_size_y = popup_y * 2 / 5;
        int image_x = popup_x / 2 - image_size_x / 2;
        int image_y = popup_y / 15;
        int text_size_x = popup_x * 5 / 6;
        int text_size_y = popup_y * 3 / 9;
        int text_x = popup_x / 2 - text_size_x / 2;
        int text_y = popup_y * 1 / 2;

        //Texture image = AssetDatabase.LoadAssetAtPath(target_board.texture_path, typeof(Texture)) as Texture; 

        //image
        Texture image;
        if (target_board.board_id == 0)
        {
            image = board_start;
        }
        else if (target_board.board_id == 10 || target_board.board_id == 20 || target_board.board_id == 30)
        {
            image = board_isolation;
        }
        else if (target_board.board_id == 2 || target_board.board_id == 7 || target_board.board_id == 12 || target_board.board_id == 25 || target_board.board_id == 32 || target_board.board_id == 37)
        {
            image = board_gold;
        }
        else
        {
            image = board_normal;
        }
        GUI.DrawTexture(new Rect(image_x, image_y, image_size_x, image_size_y), image);

        /*
        // You may put a label to show a message to the player
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.MiddleCenter;
        GUI.Label(new Rect(label_x, label_y, label_size_x, label_size_y), target_board.board_info, centeredTextStyle);
        */

        //board_info
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.UpperCenter;
        GUI.Label(new Rect(text_x, text_y, text_size_x, text_size_y), target_board.board_info, centeredTextStyle);
        //GUI.DrawTexture(new Rect(text_x, text_y, text_size_x, text_size_y), image);

        //button
        if (GUI.Button(new Rect(popup_x/2 - button_size_x, button_y, button_size_x, button_size_y), "구매"))
        {
            Debug.Log("Buy(O) btn");
            //message 생성
            StringBuilder sb = new StringBuilder();
            sb.Append("GOMIN");
            sb.Append(Global.Major_Game);
            sb.Append(Global.Game_Buy);
            sb.Append(Global.padding);
            sb.Append(Global.room_number);
            sb.Append(Global.delim);
            sb.Append(Global.Success);

            //send
            if (Socket_script.send_message(sb))
            {
                Debug.Log("Send Success");
                showBuyCheck = false;
            }
            else
            {
                Debug.Log("Send Fail");
            }
        }
        if (GUI.Button(new Rect(popup_x/2, button_y, button_size_x, button_size_y), "닫기"))
        {
            Debug.Log("Buy(X) btn");
            //message 생성
            StringBuilder sb = new StringBuilder();
            sb.Append("GOMIN");
            sb.Append(Global.Major_Game);
            sb.Append(Global.Game_Buy);
            sb.Append(Global.padding);
            sb.Append(Global.room_number);
            sb.Append(Global.delim);
            sb.Append(Global.Fail);

            //send
            if (Socket_script.send_message(sb))
            {
                Debug.Log("Send Success");
                showBuyCheck = false;
            }
            else
            {
                Debug.Log("Send Fail");
            }
        }
    }

    void ShowGUI(int windowID)
    {
        int label_size_x = popup_x/3;
        int label_size_y = popup_y/6;
        int label_x = popup_x / 2 - label_size_x / 2;
        int label_y = popup_y * 3 / 6;
        int button_size_x = popup_x / 3;
        int button_size_y = popup_y / 12;
        int button_x = popup_x / 2 - button_size_x / 2;
        int button_y = popup_y * 7 / 8;
        int image_size_x = popup_x * 5 / 6;
        int image_size_y = popup_y * 2 / 5;
        int image_x = popup_x / 2 - image_size_x / 2;
        int image_y = popup_y /15;
        int text_size_x = popup_x * 5 / 6;
        int text_size_y = popup_y * 3 / 9;
        int text_x = popup_x /2 - text_size_x / 2;
        int text_y = popup_y * 1 / 2;

        //Texture image = AssetDatabase.LoadAssetAtPath(target_board.texture_path, typeof(Texture)) as Texture; 

        Texture image;
        if (target_board.board_id == 0)
        {
            image = board_start;
        }
        else if (target_board.board_id == 10 || target_board.board_id == 20 || target_board.board_id == 30)
        {
            image = board_isolation;
        }
        else if (target_board.board_id == 2 || target_board.board_id == 7 || target_board.board_id == 12 || target_board.board_id == 25 || target_board.board_id == 32 || target_board.board_id == 37)
        {
            image = board_gold;
        }
        else
        {
            image = board_normal;
        }
        GUI.DrawTexture(new Rect(image_x, image_y, image_size_x, image_size_y), image);

        /*
        // You may put a label to show a message to the player
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.MiddleCenter;
        GUI.Label(new Rect(label_x, label_y, label_size_x, label_size_y), target_board.board_info, centeredTextStyle);
        */

        //board_info
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.UpperCenter;
        GUI.Label(new Rect(text_x, text_y, text_size_x, text_size_y), target_board.board_info, centeredTextStyle);
        //GUI.DrawTexture(new Rect(text_x, text_y, text_size_x, text_size_y), image);

        //button
        if (GUI.Button(new Rect(button_x, button_y, button_size_x, button_size_y), "닫기"))
        {
            showPopUp = false;
        }
    }

    private void init_board()
    {
        board_arr = new Board[40];
        for (int i = 0; i < 40; i++)
        {
            board_arr[i] = new Board();
            board_arr[i].building_count = 0;
            board_arr[i].owner = 0;
            board_arr[i].board_id = i;
            board_arr[i].board_info = Global.Board_Info[i];
            board_arr[i].texture_path = "Assets/Image/Board/board_0.png";
            board_arr[i].board_status = Global.Normal_Board;
            board_arr[i].price = Global.Board_Price[i];
            board_arr[i].name = Global.Board_name[i];
        }
    }

    public void click_dice()
    {
        Debug.Log("click_dice - room_player_id : "+this.room_player_id);
        if (my_turn)
        {
            Debug.Log("click dice");
            //message 생성
            StringBuilder sb = new StringBuilder();
            sb.Append("GOMIN");
            sb.Append(Global.Major_Game);
            sb.Append(Global.Game_DiceRoll);
            sb.Append(Global.padding);
            sb.Append(Global.room_number);

            //send
            if (Socket_script.send_message(sb))
            {
                Debug.Log("Send Success");
            }
            else
            {
                Debug.Log("Send Fail");
            }
        }
        //int player = 1;
        //GameObject.Find("Canvas/Player/Player"+player).GetComponent<Player_script>().move_position(0, 39);
        //change_board_color(1, 1);
        //draw_building(33);
    }

    private void change_board_color(int player_num, int board_id)
    {
        GameObject.Find("Canvas/Map/map_" + board_id + "/Panel").GetComponent<Image>().color = Global.player_Color[player_num - 1];
    }

    private void draw_building(int board_id){
        GameObject child = null;
        Debug.Log("draw_building - board_id : "+board_id);

        child = Instantiate(Building, new Vector3(0, 0, 0), GameObject.Find("Canvas/Map/map_" + board_id).GetComponent<Transform>().rotation) as GameObject;
        if (1 <= board_id && board_id <= 9)
        {
            if(board_arr[board_id].building_count == 2){
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(-320, -215 + board_id*Global.move_scale, 0);
            }
            else if (board_arr[board_id].building_count == 3)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(-320, -235 + board_id * Global.move_scale, 0);
            }
        }
        else if (11 <= board_id && board_id <= 19)
        {
            if (board_arr[board_id].building_count == 2)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(-270 + (board_id - 11)* Global.move_scale, 220 , 0);
            }
            else if (board_arr[board_id].building_count == 3)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(-290 + (board_id - 11) * Global.move_scale, 220, 0);
            }
        }
        else if(21 <= board_id && board_id <= 29)
        {
            if (board_arr[board_id].building_count == 2)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(120, 190 - (board_id - 21) * Global.move_scale, 0);
            }
            else if (board_arr[board_id].building_count == 3)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(120, 170 - (board_id - 21) * Global.move_scale, 0);
            }
        }
        else if(31 <= board_id && board_id <= 39)
        {
            if (board_arr[board_id].building_count == 2)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(70 - (board_id - 31) * Global.move_scale, -220, 0);
            }
            else if (board_arr[board_id].building_count == 3)
            {
                child.transform.parent = GameObject.Find("Canvas").transform;
                child.transform.localPosition = new Vector3(90 - (board_id - 31) * Global.move_scale, -220, 0);
            }
        }
    }

    private static void set_log_ready(string log_text){
        main_thread_log_finish = false;
        Debug.Log("set log read : ");
        Debug.Log(log_text);
        main_thread_log = log_text;
        main_thread_log_check = true;
    }

    private void set_log(string log_text)
    {
        Debug.Log("set_log : " + log_text);
        log.Append(log_text + System.Environment.NewLine);
        Log.text = log.ToString();
        Debug.Log("Set_log : " + log.ToString());
        GameObject.Find("Canvas/Log/Scrollbar Vertical").GetComponent<Scrollbar>().value = 0;
        main_thread_log_finish = true;
    }
    public static void send_position()
    {
        if (my_turn)
        {
            Debug.Log("Game visit");
            //message 생성
            StringBuilder sb = new StringBuilder();
            sb.Append("GOMIN");
            sb.Append(Global.Major_Game);
            sb.Append(Global.Game_Visit);
            sb.Append(Global.padding);
            sb.Append(Global.room_number);

            //send
            if (Socket_script.send_message(sb))
            {
                Debug.Log("Send Success");
            }
            else
            {
                Debug.Log("Send Fail");
            }
        }
    }
}
