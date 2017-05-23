using UnityEngine;
using System.Collections;
using UnityEditor;

public class Room_scene : MonoBehaviour {
    private bool showPopUp = false;
    private string popup_name = "땅 정보";
    private Board[] board_arr;
    private Board target_board;
    private int popup_x = (int)(Screen.width / 3);
    private int popup_y = (int)(Screen.height / 1.5);

    class Board
    {
        public int building_count;
        public int owner;
        public int board_id;
        public int board_status;
        public string texture_path;
        public string board_info;
    }

	// Use this for initialization
	void Start () {
        init_board();
	}
	
	// Update is called once per frame
	void Update () {
	
	}

    void OnGUI()
    {
        if (showPopUp)
        {
            GUI.Window(0, new Rect((Screen.width / 2) - popup_x / 2, (Screen.height / 2) - popup_y / 2, popup_x, popup_y), ShowGUI, popup_name);

        }
    }

    public void click()
    {

    }

    public void click_board(int board_id)
    {
        target_board = board_arr[board_id];
        showPopUp = true;
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

        Texture image = AssetDatabase.LoadAssetAtPath(target_board.texture_path, typeof(Texture)) as Texture; 

        //image
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
        }
    }
}
