///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2022 Force Dimension, Switzerland.
//  All Rights Reserved.
//
//  Force Dimension SDK 3.15.0
//
///////////////////////////////////////////////////////////////////////////////



#include <stdio.h>

#include "dhdc.h"

#include <iostream>
#include <fstream>

#include <chrono>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define REFRESH_INTERVAL  0.1   // sec



//Originally the method for Solution 1

extern "C" __declspec(dllexport) int toCSV(std::ofstream& name)
{
    double px, py, pz;
    double oa, ob, og;
    double fx, fy, fz;
    double t1, t0 = dhdGetTime();
    int    done = 0;

    // message
    printf("Force Dimension - Gravity Compensation Example %s\n", dhdGetSDKVersionStr());
    printf("Copyright (C) 2001-2022 Force Dimension\n");
    printf("All Rights Reserved.\n\n");

    // open the first available device
    if (dhdOpen() < 0) {
        printf("error: cannot open device (%s)\n", dhdErrorGetLastStr());
        dhdSleep(2.0);
        return -1;
    }

    // identify device
    printf("%s device detected\n\n", dhdGetSystemName());

    // display instructions
    printf("press 'q' to quit\n\n");

    // enable force
    dhdEnableForce(DHD_ON);

    //std::ofstream myfile("test.csv");

    



    // haptic loop
    while (!done) {

        // apply zero force
        if (dhdSetForceAndTorqueAndGripperForce(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) < DHD_NO_ERROR) {
            printf("error: cannot set force (%s)\n", dhdErrorGetLastStr());
            done = 1;
        }

        // display refresh rate and position at 10Hz
        t1 = dhdGetTime();
        if ((t1 - t0) > REFRESH_INTERVAL) {

            // update timestamp
            t0 = t1;

            // retrieve position
            if (dhdGetPositionAndOrientationDeg(&px, &py, &pz, &oa, &ob, &og) < DHD_NO_ERROR) {
                printf("error: cannot read position (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }

            // retrieve force
            if (dhdGetForce(&fx, &fy, &fz) < DHD_NO_ERROR) {
                printf("error: cannot read force (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }

            // display status
            printf("p (%+0.03f %+0.03f %+0.03f %+0.03f %+0.03f %+0.03f) m  |  f (%+0.01f %+0.01f %+0.01f) N  |  freq %0.02f kHz\r", px, py, pz, oa, ob, og, fx, fy, fz, dhdGetComFreq());

            name << px;
            name << ",";
            name << py;
            name << ",";
            name << pz;
            name << ",";
            name << oa;
            name << ",";
            name << ob;
            name << ",";
            name << og;
            name << "\n";


            // user input
            if (dhdKbHit() && dhdKbGet() == 'q') done = 1;
        }
    }

    //outputFile.close();

    // close the connection
    dhdClose();

    // happily exit
    printf("\ndone.\n");
    return 0;
}



extern "C" __declspec(dllexport) void openDevice()
{
    int done;

    if (dhdOpen() < 0) {
        printf("error: cannot open device (%s)\n", dhdErrorGetLastStr());
        dhdSleep(2.0);
        done = 1;
    }

}

extern "C" __declspec(dllexport) void closeDevice()
{
    dhdClose();

}

extern "C" __declspec(dllexport) double(&returnPose(double(&arr)[6]))[6]
{
    double px, py, pz;
    double oa, ob, og;
    double fx, fy, fz;
    double t1, t0 = dhdGetTime();
    int    done = 0;


    /***
    // open the first available device
    if (dhdOpen() < 0) {
        //printf("error: cannot open device (%s)\n", dhdErrorGetLastStr());
        dhdSleep(2.0);
        done = 1;
    }
    ***/
   

    // enable force
    dhdEnableForce(DHD_ON);

    // haptic loop
    
    while (!done) {

        // apply zero force
        if (dhdSetForceAndTorqueAndGripperForce(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) < DHD_NO_ERROR) {
            //printf("error: cannot set force (%s)\n", dhdErrorGetLastStr());
            done = 1;
        }

        // display refresh rate and position at 10Hz
        t1 = dhdGetTime();
        if ((t1 - t0) > REFRESH_INTERVAL) {

            // update timestamp
            t0 = t1;

            // retrieve position
            if (dhdGetPositionAndOrientationDeg(&px, &py, &pz, &oa, &ob, &og) < DHD_NO_ERROR) {
                //printf("error: cannot read position (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }

            // retrieve force
            if (dhdGetForce(&fx, &fy, &fz) < DHD_NO_ERROR) {
                //printf("error: cannot read force (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }

            // display status
            arr[0] = px;
            arr[1] = py;
            arr[2] = pz;
            arr[3] = oa;
            arr[4] = ob;
            arr[5] = og;

            //printf("%f, %f, %f, %f, %f, %f", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);

            printf("p (%+0.03f %+0.03f %+0.03f %+0.03f %+0.03f %+0.03f) m  |  f (%+0.01f %+0.01f %+0.01f) N  |  freq %0.02f kHz\r", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], fx, fy, fz, dhdGetComFreq());

            return arr;

            // user input
            if (dhdKbHit() && dhdKbGet() == 'q') done = 1;
        }
    }
    

    // close the connection
    dhdClose();

    return arr;;
}



int
main(int  argc,
    char** argv) {
    
    double arr[6];
    openDevice();
  
    //returnPose(arr);
    //closeDevice();


    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Set up server address information
    struct addrinfo* addr = nullptr, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    result = getaddrinfo(nullptr, "12345", &hints, &addr);
    if (result != 0)
    {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a SOCKET object to listen for client connections
    SOCKET listenSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    // Bind the socket to the local address and port
    result = bind(listenSocket, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Free the address information structure
    freeaddrinfo(addr);

    // Set the socket to listen for incoming connections
    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for a client connection..." << std::endl;

    // Accept a client connection
    SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Close the listening socket
    closesocket(listenSocket);


    while (true) {

        
        
        returnPose(arr);

        float posX = arr[0], posY = arr[1], posZ = arr[2], rotX = arr[3], rotY = arr[4], rotZ = arr[5];

        send(clientSocket, reinterpret_cast<char*>(&posX), sizeof(posX), 0);
        send(clientSocket, reinterpret_cast<char*>(&posY), sizeof(posY), 0);
        send(clientSocket, reinterpret_cast<char*>(&posZ), sizeof(posZ), 0);
        send(clientSocket, reinterpret_cast<char*>(&rotX), sizeof(rotX), 0);
        send(clientSocket, reinterpret_cast<char*>(&rotY), sizeof(rotY), 0);
        send(clientSocket, reinterpret_cast<char*>(&rotZ), sizeof(rotZ), 0);


        
        // Receive the timestamp (as a long long) from the client
        long long timestamp;
        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&timestamp), sizeof(timestamp), 0);

        if (bytesReceived > 0)
        {
            // Echo the received timestamp back to the client
            send(clientSocket, reinterpret_cast<char*>(&timestamp), sizeof(timestamp), 0);
        }
        
        
        
        
    }


    

}
