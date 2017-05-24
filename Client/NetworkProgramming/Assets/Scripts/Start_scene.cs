using UnityEngine;
using System.Collections;
using System.Text;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using System;

public class Start_scene : MonoBehaviour {
    public InputField ID_text;
    public InputField Password_text;
    private Socket_script socket_script;
    private bool showPopUp = false;
    private string popup_name = "Error";
    private string popup_text;
    private int popup_x = 300;
    private int popup_y = 140;
    private string id;
    private string password;
    private int user_idx;

    private void get_id_password()
    {
        id = ID_text.text;
        password = Password_text.text;
    }
    public void Login(){
        get_id_password();
        Debug.Log("id = " + id);
        Debug.Log("password = " + password);
        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_User);
        sb.Append(Global.User_Login);
        sb.Append(Global.padding);
        sb.Append(id);
        sb.Append(Global.delim);
        sb.Append(password);
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

        if (return_message == Global.Fail)
        {
            popup_name = "Error";
            popup_text = "아이디와 비밀번호를 확인해주세요.";
            showPopUp = true;
            
        }
        else
        {
            user_idx = Int32.Parse(return_message);
            Socket_script.user_idx = user_idx;
            Socket_script.user_id = id;
            SceneManager.LoadScene("list_scene");
        }
    }

    public void create_id()
    {
        get_id_password();
        Debug.Log("id = " + id);
        Debug.Log("password = " + password);

        //message 생성
        StringBuilder sb = new StringBuilder();
        sb.Append("GOMIN");
        sb.Append(Global.Major_User);
        sb.Append(Global.User_Signup);
        sb.Append(Global.padding);
        sb.Append(id);
        sb.Append(Global.delim);
        sb.Append(password);
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
        Debug.Log(return_message);
        if (return_message == Global.Fail)//중복 ID 존재
        {
            popup_name = "Error";
            popup_text = "중복된 아이디가 있습니다.";
        }
        else//ID 생성 성공
        {
            popup_name = "Success";
            popup_text = "아이디가 생성되었습니다.";
        }
        showPopUp = true;
    }

    void OnGUI()
    {
        if (showPopUp)
        {
            GUI.Window(0, new Rect((Screen.width / 2) - popup_x/2, (Screen.height / 2) - popup_y/2, popup_x, popup_y), ShowGUI, popup_name);

        }
    }

    void ShowGUI(int windowID)
    {
        int label_size_x = 200;
        int label_size_y = 40;
        int label_x = popup_x/2 - label_size_x/2;
        int label_y = 25;
        int button_size_x = 70;
        int button_size_y = 30;
        int button_x = popup_x / 2 - button_size_x / 2;
        int button_y = 80;
        
        // You may put a label to show a message to the player
        GUIStyle centeredTextStyle = new GUIStyle("label");
        centeredTextStyle.alignment = TextAnchor.MiddleCenter;
        GUI.Label(new Rect(label_x, label_y, label_size_x, label_size_y), popup_text, centeredTextStyle);

        // You may put a button to close the pop up too

        if (GUI.Button(new Rect(button_x, button_y, button_size_x, button_size_y), "OK"))
        {
            showPopUp = false;
        }

    }
}
