#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "queue.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_PORT 5059
#define BUFFER_SIZE 256

struct Queue* queue;

CRITICAL_SECTION csQ;
CRITICAL_SECTION csW;


int main() {
    /* 
    DWORD print0ID;
    HANDLE LoadBalancerT;
    LoadBalancerT = CreateThread(NULL, 0, , NULL, 0, &print0ID);

    DWORD print1ID, print2ID, print3ID, print4ID, print5ID;
    HANDLE Worker1, Worker2, Worker3, Worker4, Worker5;

    Worker1 = CreateThread(NULL, 0, &print1, NULL, 0, &print1ID);
    Worker2 = CreateThread(NULL, 0, &print2, NULL, 0, &print2ID);
    Worker3 = CreateThread(NULL, 0, &print3, NULL, 0, &print3ID);
    Worker4 = CreateThread(NULL, 0, &print3, NULL, 0, &print4ID);
    Worker5 = CreateThread(NULL, 0, &print3, NULL, 0, &print5ID);
    */

    InitializeCriticalSection(&csQ);
    InitializeCriticalSection(&csW);

    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;

    // Socket used for communication with client
    SOCKET acceptedSocket = INVALID_SOCKET;

    // Variable used to store function return value
    int iResult;

    // Buffer used for storing incoming data
    char dataBuffer[BUFFER_SIZE];

    // WSADATA data structure that is to receive details of the Windows Sockets implementation
    WSADATA wsaData;

    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Initialize serverAddress structure used by bind
    sockaddr_in serverAddress;
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;				// IPv4 address family
    serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
    serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port


    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET,      // IPv4 address family
        SOCK_STREAM,  // Stream socket
        IPPROTO_TCP); // TCP protocol

    // Check if socket is successfully created
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Check if socket is successfully binded to address and port from sockaddr_in structure
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Servis socket is set to listening mode. Waiting for new connection requests.\n");

    do
    {
        // Struct for information about connected client
        sockaddr_in clientAddr;

        int clientAddrSize = sizeof(struct sockaddr_in);

        // Accept new connections from clients 
        acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        // Check if accepted socket is valid 
        if (acceptedSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("\nNew client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        do
        {
            // Receive data until the client shuts down the connection
            iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);

            if (iResult > 0)	// Check if message is successfully received
            {
                dataBuffer[iResult] = '\0';
                printf("Client sent: %s.\n", dataBuffer);

                //TO DO
                //TREBA DA SE POZOVE FUNKCIJA ZA UPIS U QUEUE
     

            }
            else if (iResult == 0)	// Check if shutdown command is received
            {
                // Connection was closed successfully
                printf("Connection with client closed.\n");
                closesocket(acceptedSocket);
            }
            else	// There was an error during recv
            {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(acceptedSocket);
            }

        } while (iResult > 0);

        // Here is where server shutdown loguc could be placed

    } while (true);

    // Shutdown the connection since we're done
    iResult = shutdown(acceptedSocket, SD_BOTH);

    // Check if connection is succesfully shut down.
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(acceptedSocket);
        WSACleanup();
        return 1;
    }

    //Close listen and accepted sockets
    closesocket(listenSocket);
    closesocket(acceptedSocket);

    // Deinitialize WSA library
    WSACleanup();

    DeleteCriticalSection(&csQ);
    DeleteCriticalSection(&csW);

	return 0;
}

DWORD WINAPI Worker(LPVOID lpParam) {
    //TO DO
    //TREBA DA SE POZOVE FUNKCIJA ZA DA SE UZME PODADTAK IZ QUEUE
    //PA POSLE TREBA DA SE UPISE U TEXT FAJL TI PODACI
    //TREBA LOCK KOD UPISA U FAJL

    EnterCriticalSection(&csW);

    struct Elem el = GetElem();

    FILE* f = fopen("data.txt", "w");

    if (f == NULL){
        printf("Error opening file!\n");
        return 1;
    }


    fprintf(f, "%hu %hu %s", el.data, el.port, el.adrress);

    fclose(f);

    LeaveCriticalSection(&csW);

    return 0;
}
//TO DO
//TREBA DA SE NAPRAVI FUNCKIJA ZA LOADBALANCER 