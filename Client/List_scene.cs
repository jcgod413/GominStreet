using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;

public class List_scene : MonoBehaviour {
    private bool showPopUp = false;
    private string popup_name;
    private string popup_text;
    private int popup_x = 300;
    private int popup_y = 140;
    private string game_name = "";

    public void create_game()
    {
        popup_name = "방 생성";
        showPopUp = true;
    }

    private void create_game_process()
    {
        /*
         * 통신
         * */

        SceneManager.LoadScene("room_scene");
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
}
