#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
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
#define MAX_CLIENTS 15




struct Queue* queue;

CRITICAL_SECTION csW;
CRITICAL_SECTION csQ;

//FUNKCIJA ZA LOAD BALANCER
DWORD WINAPI LoadBalancer(LPVOID lpParam);
//FUNKCIJA ZA WORKER
DWORD WINAPI Worker(LPVOID lpParam);
//FUNKCIJA ZA DODAVANJE PODATKA U QUEUE
bool AddItem(struct Queue* queue, int data);
//FUNKCIJA ZA DOBIJANJE PODATKA
int GetItem(struct Queue* queue);

int main() {
     
    queue = CreateQueue(15);

    DWORD print0ID;
    HANDLE LoadBalancerT;
    LoadBalancerT = CreateThread(NULL, 0, &LoadBalancer, NULL, 0, &print0ID);
    
    DWORD print1ID, print2ID, print3ID, print4ID, print5ID;
    HANDLE Worker1, Worker2, Worker3, Worker4, Worker5;

    Worker1 = CreateThread(NULL, 0, &Worker, NULL, 0, &print1ID);
   // Worker2 = CreateThread(NULL, 0, &Worker, NULL, 0, &print2ID);
   // Worker3 = CreateThread(NULL, 0, &Worker, NULL, 0, &print3ID);
   // Worker4 = CreateThread(NULL, 0, &Worker, NULL, 0, &print4ID);
   // Worker5 = CreateThread(NULL, 0, &Worker, NULL, 0, &print5ID);
    

    InitializeCriticalSection(&csW);
    InitializeCriticalSection(&csQ);

    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;

    // Variable used to store function return value
    int iResult;

    // Buffer used for storing incoming data
    char dataBuffer[BUFFER_SIZE];

    fd_set readfds;

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

    char multiple = !0;
    int res = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &multiple, sizeof(multiple));
    if (res < 0) {
        closesocket(listenSocket);
        WSACleanup();
        return 0;
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

    fd_set socketSet;
    SOCKET clients[MAX_CLIENTS];
    SOCKET acceptedSocket = INVALID_SOCKET, sdmax = INVALID_SOCKET;
    int curentNumberOfSockets = 0;

    memset(clients, 0, MAX_CLIENTS * sizeof(SOCKET));

    while (true) {
        FD_ZERO(&socketSet);
        FD_SET(listenSocket, &socketSet);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            acceptedSocket = clients[i];

            if (acceptedSocket > 0) {
                FD_SET(acceptedSocket, &socketSet);
            }
            if (acceptedSocket > sdmax) {
                sdmax = acceptedSocket;
            }
        }

        int activity = select(sdmax + 1, &socketSet, NULL, NULL, NULL);
        if (activity < 0)
            continue;
        if (FD_ISSET(listenSocket, &socketSet)) {
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

            if (curentNumberOfSockets >= MAX_CLIENTS) {
                printf("Full!\n");
                shutdown(acceptedSocket, SD_BOTH);
                closesocket(acceptedSocket);
            }
            else {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!clients[i]) {
                        clients[i] = acceptedSocket;
                        curentNumberOfSockets++;
                        break;
                    }
                }
            }

        }
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i]) {
                continue;
            }

            acceptedSocket = clients[i];
            if (FD_ISSET(acceptedSocket, &socketSet)) {
                // Receive data until the client shuts down the connection
                iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);

                if (iResult > 0)	// Check if message is successfully received
                {
                    dataBuffer[iResult] = '\0';
                    printf("Client sent: %s.\n", dataBuffer);

                    AddItem(queue, atoi(dataBuffer));


                }
                else if (iResult == 0)	// Check if shutdown command is received
                {
                    // Connection was closed successfully
                    printf("Connection with client closed.\n");
                    shutdown(acceptedSocket, SD_BOTH);
                    closesocket(acceptedSocket);
                    clients[i] = 0;
                    curentNumberOfSockets--;
                }
                else	// There was an error during recv
                {
                    printf("recv failed with error: %d\n", WSAGetLastError());
                    shutdown(acceptedSocket, SD_BOTH);
                    closesocket(acceptedSocket);
                    clients[i] = 0;
                    curentNumberOfSockets--;
                }
            }
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        shutdown(clients[i], SD_BOTH);
        closesocket(clients[i]);
        clients[i] = 0;
    }

    //Close listen and accepted sockets
    closesocket(listenSocket);
    
    // Deinitialize WSA library
    WSACleanup();

    DeleteCriticalSection(&csQ);
    DeleteCriticalSection(&csW);
    CloseHandle(LoadBalancerT);
    FreeQueue(queue);

	return 0;
}

DWORD WINAPI Worker(LPVOID lpParam) {
    //TO DO
    //TREBA DA SE POZOVE FUNKCIJA ZA DA SE UZME PODADTAK IZ QUEUE
    //PA POSLE TREBA DA SE UPISE U TEXT FAJL TI PODACI
    //TREBA LOCK KOD UPISA U FAJL

    
    while (true) {
        int data = GetItem(queue);
        if (data == -1)
            continue;
        EnterCriticalSection(&csW);

        FILE* f = fopen("data.txt", "a");

        if (f == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        fprintf(f, "%i", data);
        fprintf(f, "\n");
        

        fclose(f);

        LeaveCriticalSection(&csW);

        Sleep(2000);
    }

    return 0;
}
//TO DO
//TREBA DA SE NAPRAVI FUNCKIJA ZA LOADBALANCER 
DWORD WINAPI LoadBalancer(LPVOID lpParam) {

    while (true) {
        double capacity = (double)(queue->size) / (double)(queue->capactity) * 100;
        printf("The current capacity of the queue is %lf % \n", capacity);
        if (capacity == 100) {
            //ONEMOGUCITI SLANJE NOVIH PORUKA OD KLIJENATA
        }
        else if (capacity > 70) {
            //PALITI NOVE NIT
        }
        else if(capacity < 30)
        {
            //GASITI NITI
        }
   

        Sleep(3000);
    }
    return 0;
}
bool AddItem(struct Queue* queue, int data) {
    bool rez;
    EnterCriticalSection(&csQ);
    if (AddElem(queue, data)) 
        rez = true;
    else
        rez = false;
    LeaveCriticalSection(&csQ);
    return rez;
}
int GetItem(struct Queue* queue) {

    EnterCriticalSection(&csQ);
    int data = GetData(queue);
    LeaveCriticalSection(&csQ);

    return data;
}