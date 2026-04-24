#include "hw_analyzer.h"
#include "system.h"

#include <Arduino.h>
#include <WiFi.h>

// =====================================================
//  HW ANALYZER
// =====================================================

void HwAnalyzer::begin()
{
    // nada necessário aqui
    // MAC será capturado automaticamente quando o AP estiver ativo
}

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