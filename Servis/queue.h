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

Queue* CreateQueue(unsigned short capacity);

void AddElem(unsigned short data, unsigned short port, IN_ADDR adrress);

void GetElem();

double GetStatus();

