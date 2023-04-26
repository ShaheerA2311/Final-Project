using System;
using System.Diagnostics;
using System.IO;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class SocketClient : MonoBehaviour
{
    private Thread clientThread;
    private TcpClient tcpClient;
    private bool isConnected = false;
    Vector3 pos_vect, rot_vect;

    private Stopwatch stopwatch;

    private float scale = 3.0f;

    private List<double> pingValues;
    private Dictionary<Vector3,Vector3> PosAndOriValues;


    // Define a concurrent queue to store received pose values
    private ConcurrentQueue<(Vector3 position, Vector3 rotation)> poseQueue = new ConcurrentQueue<(Vector3, Vector3)>();


    // Use this for initialization
    void Start()
    {
        //set up timer to calculate ping
        stopwatch = new Stopwatch();

        pingValues = new List<double>();

        PosAndOriValues = new Dictionary<Vector3, Vector3>();

        // Create and start a new thread for the socket connection to avoid blocking the main thread
        clientThread = new Thread(ConnectToServer);
        clientThread.IsBackground = true;
        clientThread.Start();
    }

    async void ConnectToServer()
    {
        try
        {
            // Connect to the C++ server
            tcpClient = new TcpClient("127.0.0.1", 12345);
            isConnected = true;

            // Receive data from the server
            using (NetworkStream stream = tcpClient.GetStream())
            using (BinaryReader reader = new BinaryReader(stream))
            {
                while (isConnected && tcpClient.Connected)
                {
                    // Read the pose values from the server
                    float posX = reader.ReadSingle();
                    float posY = reader.ReadSingle();
                    float posZ = reader.ReadSingle();
                    float rotX = reader.ReadSingle();
                    float rotY = reader.ReadSingle();
                    float rotZ = reader.ReadSingle();


                    // Send the current timestamp (as a long) to the server
                    long timestamp = DateTime.UtcNow.Ticks;
                    byte[] timestampBytes = BitConverter.GetBytes(timestamp);
                    stream.Write(timestampBytes, 0, timestampBytes.Length);


                    // Process the received pose values (e.g., update a GameObject's position and rotation)

                    poseQueue.Enqueue((new Vector3(-posX * scale, -posY * scale, -posZ * scale), new Vector3(rotY, -rotX, rotZ)));

                    pos_vect = new Vector3(-posX * scale, -posY * scale, -posZ * scale);
                    rot_vect = new Vector3(rotY, -rotX, rotZ);

                    PosAndOriValues.Add(pos_vect, rot_vect);

                    // Receive the echoed timestamp asynchronously
                    byte[] receivedTimestampBytes = new byte[sizeof(long)];
                    int bytesRead = await stream.ReadAsync(receivedTimestampBytes, 0, receivedTimestampBytes.Length);
                    if (bytesRead > 0)
                    {
                        long receivedTimestamp = BitConverter.ToInt64(receivedTimestampBytes, 0);
                        long pingInTicks = DateTime.UtcNow.Ticks - receivedTimestamp;
                        double pingInMicroseconds = (double)pingInTicks / (TimeSpan.TicksPerMillisecond / 1000);
                        UnityEngine.Debug.Log($"Ping: {pingInMicroseconds} µs");
                        pingValues.Add(pingInMicroseconds);
                    }

                    




                }
            }
        }
        catch (Exception e)
        {
            UnityEngine.Debug.LogError($"Error connecting to server: {e.Message}");
        }
    }

    void Update()
    {
        // Check if there are any pose values in the queue
        if (poseQueue.TryDequeue(out (Vector3 position, Vector3 rotation) pose))
        {
            // Update the GameObject's position and rotation with the received pose values
            transform.localPosition = pose.position;
            transform.localRotation = Quaternion.Euler(pose.rotation);
        }

    }

    //Save ping values to CSV
    private void SavePingValuesToFile(string filePath)
    {
        using (StreamWriter writer = new StreamWriter(filePath))
        {
            writer.WriteLine("Ping (ms)"); // Write the header for the CSV file

            foreach (double ping in pingValues)
            {
                writer.WriteLine(ping);
            }
        }
    }


    //Save pose to CSV
    private void SavePosetoFile(string filePath)
    {
        using (StreamWriter writer = new StreamWriter(filePath))
        {
            writer.WriteLine("Pose and Orientation Values: "); // Write the header for the CSV file

            foreach (KeyValuePair<Vector3, Vector3> pose in PosAndOriValues)
            {
                writer.WriteLine(pose);
            }
        }
    }


    


    // Cleanup when the script is disabled or the application is closed
    void OnDisable()
    {
        
        //Create the CSV files
        SavePingValuesToFile("ping_values.csv");
        SavePosetoFile("pose_values.csv");


        if (tcpClient != null)
        {
            isConnected = false;
            tcpClient.Close();
            tcpClient = null;
        }

        if (clientThread != null)
        {
            clientThread.Join();
            clientThread = null;
        }
    }
}

