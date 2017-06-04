using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;

public class Player_script : MonoBehaviour {
    private int start;
    private int end;
    private int move_length;
    private int speed = Global.speed;
    private bool send_visit = false;
    private bool moving;
    private Vector2 vector;
    private Vector3 position;
    private Thread thread;
	// Use this for initialization
	void Start () {
        position = this.GetComponent<RectTransform>().anchoredPosition3D;
        //move_length = 45*9 + 90;
        //vector = new Vector2(0, 1);
	}
	
	// Update is called once per frame
	void Update () {
        if (move_length > 0)
        {
            moving = true;
            move_length -= speed;
            move_process(vector);
        }else{
            moving = false;
        }
	}

    void move_process(Vector2 vector)
    {
        //position.x = vector.x * Time.deltaTime * speed;
        //position.y += vector.y * Time.deltaTime * speed;
        //position.y = vector.y;
        position.x += vector.x * speed;
        position.y += vector.y * speed;
        //Debug.Log(position);
        this.GetComponent<RectTransform>().anchoredPosition3D = position;
    }

    public void move(Vector2 vector, int length)
    {
        //this.vector = vector*Time.deltaTime*speed;
        this.moving = true;
        this.vector = vector;
        this.move_length = length;
    }

    public void set_speed(int speed)
    {
        this.speed = speed;
    }

    public bool is_moving()
    {
        return this.moving;
    }

    public void move_position(int start, int end)
    {
        this.start = start;
        this.end = end;
        thread = new Thread(move_thread);
        thread.Start();
    }

    private void move_thread()
    {
        if (end < start)
        {
            end += 40;
        }
        for (int i = start; i < end; i++)
        {
            if (i == 40)
            {
                i -= 40;
                end -= 40;
            }
            //Debug.Log("move_thread - i : " + i);
            if (i == 0)
            {
                move(new Vector2(0, 1), Global.move_scale  + 40);
            }
            else if (1 <= i && i <= 8)
            {
                move(new Vector2(0, 1), Global.move_scale);
            }
            else if (i == 9)
            {
                move(new Vector2(0, 1), Global.move_scale + 40);
            }
            else if (i == 10)
            {
                move(new Vector2(1, 0), Global.move_scale + 40);
            }
            else if (11 <= i && i <= 18)
            {
                move(new Vector2(1, 0), Global.move_scale);
            }
            else if (i == 19)
            {
                move(new Vector2(1, 0), Global.move_scale + 40);
            }
            else if (i == 20)
            {
                move(new Vector2(0, -1), Global.move_scale + 40);
            }
            else if (21 <= i && i <= 28)
            {
                move(new Vector2(0, -1), Global.move_scale);
            }
            else if (i == 29)
            {
                move(new Vector2(0, -1), Global.move_scale + 40);
            }
            else if (i == 30)
            {
                move(new Vector2(-1, 0), Global.move_scale + 40);
            }
            else if (31 <= i && i <= 38)
            {
                move(new Vector2(-1, 0), Global.move_scale);
            }
            else if (i == 39)
            {
                move(new Vector2(-1, 0), Global.move_scale + 40 );
            }
            while (is_moving()) { }
        }
        if (send_visit)
        {
            Room_scene.send_position();
            send_visit = false;
        }
    }

    public void set_send_visit()
    {
        send_visit = true;
    }
}
