using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System.Collections.Generic;
using System.Text;
using System;

public class List_scene : MonoBehaviour {
    private bool showPopUp = false;
    private string popup_name;
    private string popup_text;
    private int popup_x = 300;
    private int popup_y = 140;
    private string game_name = "";

    public GameObject RoomObject;
    public Transform Content;
    public List<Room_Info> Room_List;
    public Text Player_Info;

    void Start()
    {
        request_player_info();
        reload_list();
    }

    public void create_game()
    {
        popup_name = "방 생성";
        showPopUp = true;
    }

    private void request_player_info()
    {
        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_User);
        sb.Append(Global.User_Record);
        sb.Append(Global.padding);
        sb.Append(Socket_script.user_idx);

        //send
        if (Socket_script.send_message(sb))
        {
            Debug.Log("Send Success");
        }
        else
        {
            Debug.Log("Send Fail");
        }

        //receive
        string return_message = Socket_script.receive_message();
        string[] param = return_message.Split(' ');
        Player_Info.text = Socket_script.user_id + "\n" + param[0] + "승 | " + param[1] + "패 ("+(Int32.Parse(param[0])/(Int32.Parse(param[0]) + Int32.Parse(param[1])))+"%)";
    }

    private void create_game_process()
    {
        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_Room);
        sb.Append(Global.Room_Create);
        sb.Append(Global.padding);
        sb.Append(Socket_script.user_idx);
        sb.Append(Global.delim);
        sb.Append(game_name);

        //send
        if (Socket_script.send_message(sb))
        {
            Debug.Log("Send Success");
        }
        else
        {
            Debug.Log("Send Fail");
        }

        //receive
        string return_message = Socket_script.receive_message();

        if (return_message != Global.Fail)
        {
            SceneManager.LoadScene("room_scene");
        }
        else
        {
            Debug.Log("게임 생성이 실패했습니다.");
        }
    }

    void OnGUI()
    {
        if (showPopUp)
        {
            GUI.Window(0, new Rect((Screen.width / 2) - popup_x / 2, (Screen.height / 2) - popup_y / 2, popup_x, popup_y), ShowGUI, popup_name);

        }
    }

    void ShowGUI(int windowID)
    {
        int label_size_x = 200;
        int label_size_y = 40;
        int label_x = popup_x / 2 - label_size_x / 2;
        int label_y = 25;
        int button_size_x = 100;
        int button_size_y = 30;
        int button_x = popup_x / 2;
        int button_y = 100;

        // You may put a label to show a message to the player
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.MiddleCenter;
        GUI.Label(new Rect(label_x, label_y, label_size_x, label_size_y), popup_text, centeredTextStyle);

        //input_text
        centeredTextStyle = new GUIStyle("TextField");
        centeredTextStyle.fontSize = 20;
        centeredTextStyle.alignment = TextAnchor.MiddleCenter;
        game_name = GUI.TextField(new Rect(20, 30, 260, 40), game_name, 20, centeredTextStyle);

        //button
        if (GUI.Button(new Rect(button_x, button_y, button_size_x, button_size_y), "생성"))
        {
            create_game_process();
            showPopUp = false;
        }
        if (GUI.Button(new Rect(button_x - button_size_x, button_y, button_size_x, button_size_y), "취소"))
        {
            showPopUp = false;
        }
    }

    public void reload_list()
    {
        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_Room);
        sb.Append(Global.Room_List);
        sb.Append(Global.padding);

        //send
        if (Socket_script.send_message(sb))
        {
            Debug.Log("Send Success");
        }
        else
        {
            Debug.Log("Send Fail");
        }

        //receive
        string return_message = Socket_script.receive_message();

        Room_List.Clear();

        //list count
        int list_count = Int32.Parse(return_message);
        Debug.Log("list_count : "+list_count);
        
        //add room to list
        for (int i = 0; i < list_count; i++)
        {
            Room_Info room_info = new Room_Info();
            return_message = Socket_script.receive_message();
            string[] param = return_message.Split(' ');

            room_info.Room_ID = Int32.Parse(param[0]);
            room_info.status = Int32.Parse(param[1]);
            room_info.Player_Num = "( "+param[2]+" / "+"4)";
            room_info.Name = param[3];
            
            room_info.OnButtonClick = new Button.ButtonClickedEvent();
            room_info.OnButtonClick.AddListener(() => enter_room(room_info.Room_ID));

            Room_List.Add(room_info);
        }
        Socket_script.receive_message();

        this.Binding();
        Debug.Log("click");
    }

    private void enter_room(int room_id)
    {
        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_Room);
        sb.Append(Global.Room_Enter);
        sb.Append(Global.padding);
        sb.Append(room_id);
        sb.Append(Global.delim);
        sb.Append(Socket_script.user_idx);

        //send
        if (Socket_script.send_message(sb))
        {
            Debug.Log("Send Success");
        }
        else
        {
            Debug.Log("Send Fail");
        }

        //receive
        string return_message = Socket_script.receive_message();

        if (return_message != Global.Fail)
        {
            SceneManager.LoadScene("room_scene");
        }
        else
        {
            Debug.Log(return_message);
            Debug.Log("게임 입장이 실패했습니다.");
        }
    }

    private void Binding()
    {
        GameObject btn_room_temp;
        Room_Object item_object_temp;
        Debug.Log("Binding");

        Transform[] child;
        child = new Transform[transform.childCount];
        int i = 0;

        foreach (Transform t in Content)
        {
            Destroy(t.gameObject);
        }

        foreach (Room_Info room_info in this.Room_List)
        {
            btn_room_temp = Instantiate(this.RoomObject) as GameObject;
            item_object_temp = btn_room_temp.GetComponent<Room_Object>();

            item_object_temp.Name.text = room_info.Name;
            item_object_temp.Player_Num.text = room_info.Player_Num;
            item_object_temp.Room_ID = room_info.Room_ID;
            item_object_temp.Button.onClick = room_info.OnButtonClick;

            item_object_temp.transform.SetParent(this.Content);
        }
    }

    public void auto_matching()
    {
        Debug.Log("aa");
    }
}
