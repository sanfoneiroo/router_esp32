#ifndef NET_ANALYZER_H
#define NET_ANALYZER_H

#include <Arduino.h>

class Router;

struct ClientInfo
{
    uint8_t mac[6];
    int8_t rssi;
};

class NetAnalyzer {

public:

    void begin(Router* r);
    void update();

    int getClientCount();
    ClientInfo* getClients();

    String getPublicIP();
    String getTime();

private:

    Router* router;

    ClientInfo clients[10];
    int clientCount = 0;

    String publicIP;
    bool timeInitialized = false;

};

#endif