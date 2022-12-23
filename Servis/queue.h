#pragma once
#include <winsock2.h>
struct Elem {
    unsigned short data;
    unsigned short port;
    IN_ADDR adrress;
};

struct Queue {
    unsigned short capactity;
    Elem* array;
};

//TO DO
//FUNCKIJA KOJA VRACA KREIRANI QUEUE
Queue* CreateQueue(unsigned short capacity);


//TO DO
//UBACI PODATAK U QUEUE NA POCETAK
void AddElem(unsigned short data, unsigned short port, IN_ADDR adrress);


//TO DO
//FUNKCIJA KOJA VRACA ZADNJI PODATAK IZ QUEUE
Elem GetElem();

//VALJDA NEKA FUNKCIJA ZA LOAD BALANCER KOJA VRACA PROCENAT POPUNJENOSTI QUEUE
double GetStatus();

