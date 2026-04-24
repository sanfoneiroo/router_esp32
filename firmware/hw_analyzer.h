#ifndef HW_ANALYZER_H
#define HW_ANALYZER_H

#include <Arduino.h>

class HwAnalyzer
{
public:

    void begin();
    void update();

    float getTemperature();
    uint32_t getHeap();
    String getMac();

private:

    float temperature = 0;
    uint32_t heap = 0;

    String macAddress = "";

    bool macCaptured = false;
};

#endif