#include "hw_analyzer.h"
#include "system.h"

#include <Arduino.h>
#include <WiFi.h>

// =====================================================
//  HW ANALYZER
// =====================================================

void HwAnalyzer::begin()
{}

// =====================================================
//  Atualiza métricas de hardware
// =====================================================

void HwAnalyzer::update()
{
    // =================================================
    // Captura o MAC do AP apenas quando estiver pronto
    // =================================================

    if (!macCaptured)
    {
        String mac = WiFi.softAPmacAddress();

        if (mac != "00:00:00:00:00:00")
        {
            macAddress = mac;
            macCaptured = true;
        }
    }

    // =================================================
    // Leitura da temperatura interna
    // =================================================

    temperature = temperatureRead();

    // =================================================
    // Heap livre
    // =================================================

    heap = ESP.getFreeHeap();

    // =================================================
    // Uptime (segundos desde boot)
    // =================================================

    uptimeSeconds = millis() / 1000;

#if SERIAL_MONITOR_ENABLED

    LOGLN("---- HARDWARE ----");

    LOG("MAC Router: ");
    LOGLN(macAddress);

    LOG("Temperatura: ");
    LOG(temperature);
    LOGLN(" C");

    LOG("Heap livre: ");
    LOG(heap / 1024);
    LOGLN(" KB");

    LOG("Uptime: ");
    LOGLN(getUptimeString());

#endif
}

// =====================================================
//  GETTERS
// =====================================================

float HwAnalyzer::getTemperature()
{
    return temperature;
}

uint32_t HwAnalyzer::getHeap()
{
    return heap;
}

String HwAnalyzer::getMac()
{
    return macAddress;
}

uint64_t HwAnalyzer::getUptime()
{
    return uptimeSeconds;
}

// =====================================================
//  Uptime formatado (d h m s)
// =====================================================

String HwAnalyzer::getUptimeString()
{
    uint64_t s = uptimeSeconds;

    uint32_t days = s / 86400;
    s %= 86400;

    uint32_t hours = s / 3600;
    s %= 3600;

    uint32_t minutes = s / 60;
    uint32_t seconds = s % 60;

    String out = "";

    if (days > 0)
    {
        out += String(days) + "d ";
    }

    if (hours > 0 || days > 0)
    {
        out += String(hours) + "h ";
    }

    if (minutes > 0 || hours > 0 || days > 0)
    {
        out += String(minutes) + "m ";
    }

    out += String(seconds) + "s";

    return out;
}