#ifndef ROUTER_H
#define ROUTER_H

class WebServerModule;
class NetAnalyzer;
class HwAnalyzer;

class Router
{
public:

    void begin(WebServerModule* web, NetAnalyzer* net, HwAnalyzer* hw);

    int getConnectedClients();

private:

    bool configExists();

    void startAP();
    bool connectSTA();
    void enableNAT();

    void startProvisionPortal();
    void startRouterMonitor();
};

#endif