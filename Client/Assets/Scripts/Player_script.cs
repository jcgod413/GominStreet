using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_script : MonoBehaviour {
    private int move_length;
    private int speed = 4;
    private Vector2 vector;
    private Vector3 position;
	// Use this for initialization
	void Start () {
        position = this.GetComponent<RectTransform>().anchoredPosition3D;
        move_length = 200;
        vector = new Vector2(0, 1);
	}
	
	// Update is called once per frame
	void Update () {
        if (move_length > 0)
        {
            move_length -= speed;
            move_process(vector);
        }
	}

    void move_process(Vector2 vector)
    {
        //position.x = vector.x * Time.deltaTime * speed;
        //position.y += vector.y * Time.deltaTime * speed;
        //position.y = vector.y;
        position.x += vector.x * speed;
        position.y += vector.y * speed;
        Debug.Log(position);
        this.GetComponent<RectTransform>().anchoredPosition3D = position;
    }

    public void move(Vector2 vector, int length)
    {
        this.vector = vector*Time.deltaTime*speed;
        this.move_length = length;
    }
}
