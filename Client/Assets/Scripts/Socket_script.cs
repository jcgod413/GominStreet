﻿using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System;
using UnityEngine;

public class Socket_script : MonoBehaviour {
    public static int user_idx;
    public static string user_id;
    private string host_ip = "192.168.219.105";
    //private string host_ip = "43.230.1.128";
    //private string host_ip = "127.0.0.1";
    //private string host_ip = "172.20.10.3";
    //private string host_ip = "114.71.42.26";
    private int host_port = 20000;
    private static int send_length = Global.message_size;
    private static int receive_length = Global.message_size;
    private static byte[] send_byte;
    private static byte[] receive_byte = new byte[Global.message_size];
    private static Socket_script s_instance = null;
    private static Socket socket;
    private static object lock_obj = new object();
    private static object write_lock_obj = new object();
    private static bool do_connect = true;
    public static byte message_type;

    public static Socket_script instance
    {
        get
        {
            if (s_instance == null)
            {
                s_instance = new GameObject("Socket_script").AddComponent<Socket_script>();
                //오브젝트가 생성이 안되있을경우 생성.
            }
            return s_instance;
        }
    }

	// Use this for initialization
	void Start () {
        DontDestroyOnLoad(this);
        Screen.SetResolution(800, 600, true);
        connect();
	}

    private void connect()
    {
        if (do_connect)
        {
            Debug.LogError("This message will make the console appear in Development Builds");
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
            bool isConnected = false;
            try
            {
                socket.Connect(host_ip, host_port);
                isConnected = true;
            }
            catch (Exception e)
            {
                Debug.Log(e.Message);
                isConnected = false;
            }
            if (isConnected)
            {
                Debug.Log("Connect Success!!");
                do_connect = false;
            }
            else
            {
                Debug.Log("Connect Fail!!");
            }
        }
    }

	// Update is called once per frame
	void Update () {

	}

    void OnApplicationQuit()
    {
        s_instance = null;
        //게임종료시 삭제.
    }

    public static bool send_message(StringBuilder message)
    {
        lock (write_lock_obj)
        {
            try
            {
                send_byte = Encoding.Default.GetBytes(message.ToString());
                send_byte = set_byte_length(send_byte);
                socket.Send(send_byte, send_length, 0);
                return true;
            }
            catch
            {
                return false;
            }
        }
    }

    public static string receive_message()
    {
        lock (lock_obj)
        {
            try
            {
                string return_string;

                socket.Receive(receive_byte, 0, Global.message_size, SocketFlags.None);
                Debug.Log("receive");
                if (check_packit(receive_byte) == false)
                {
                    return "False";
                }
                Debug.Log(Encoding.Default.GetString(receive_byte));

                byte[] trim_byte;
                trim_byte = trim_byte_arr(receive_byte);

                return_string = Encoding.Default.GetString(trim_byte, 0, trim_byte.Length);
                return return_string;
            }
            catch(Exception e)
            {
                Debug.Log(e.Message);
                return "False";
            }
        }
    }

    private static bool check_packit(byte[] byte_arr)
    {
        char[] chk_byte = {'G', 'O', 'M', 'I', 'N'};
        for (int i = 0; i < 5; i++)
        {
            if (byte_arr[i] != chk_byte[i])
            {
                return false;
            }
        }
        return true;
    }

    private static byte[] set_byte_length(byte[] message)
    {
        byte[] tmp = new byte[send_length];

        for (int i = 0; i < message.Length; i++)
        {
            tmp[i] = message[i];
        }
        for (int i = message.Length; i < send_length; i++)
        {
            tmp[i] = 0;
        }
        return tmp;
    }

    private static byte[] trim_byte_arr(byte[] message){
        message_type = message[6];
        message_type -= (byte)'0';
        Debug.Log("message_type : " + message_type);
        byte[] return_byte_arr;
        int length = message.Length - 1;
        while (message[length] == 0)
            length--;

        return_byte_arr = new byte[length + 1 - 7];
        for (int i = 8; i < length+1; i++)
        {
            return_byte_arr[i-8] = message[i];
        }
        return return_byte_arr;
    }
}
