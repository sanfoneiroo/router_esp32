#ifndef WEB_SERVER_MODULE_H
#define WEB_SERVER_MODULE_H

class NetAnalyzer;
class HwAnalyzer;

class WebServerModule
{
public:

    void beginProvision();
    void beginRouter(NetAnalyzer* net, HwAnalyzer* hw);

    void update();
};

#endif