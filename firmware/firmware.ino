#include <Arduino.h>
#include <WiFi.h>

#include "router.h"
#include "net_analyzer.h"
#include "hw_analyzer.h"
#include "web_server.h"

// =====================================================
//  Instâncias globais dos módulos do sistema
// =====================================================

Router router;
NetAnalyzer netAnalyzer;
HwAnalyzer hwAnalyzer;
WebServerModule web;

// =====================================================
//  Controle de temporização
// =====================================================

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 5000; // 5 segundos

// =====================================================
//  SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    netAnalyzer.begin(&router);
    hwAnalyzer.begin();

    router.begin(&web, &netAnalyzer, &hwAnalyzer);

    printBootReport(&netAnalyzer);
}

// =====================================================
//  LOOP PRINCIPAL
// =====================================================

void loop()
{
    // Mantém servidor web sempre responsivo
    web.update();

    unsigned long now = millis();

    // Atualizações periódicas
    if (now - lastUpdate >= updateInterval)
    {
        lastUpdate = now;

        netAnalyzer.update();
        hwAnalyzer.update();
    }
}

// =====================================================
//  FUNÇÃO DE DEBUG DO BOOT
// =====================================================

void printBootReport(NetAnalyzer* net)
{
    Serial.println("\n====================");
    Serial.println(" ESP32 LAB NODE BOOT ");
    Serial.println("====================\n");

    Serial.print("STA IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("GATEWAY: ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("DNS: ");
    Serial.println(WiFi.dnsIP());

    Serial.print("\nAP IP: ");
    Serial.println(WiFi.softAPIP());

    Serial.print("\nPUBLIC IP: ");
    Serial.println(net->getPublicIP());

    Serial.print("\nTIME (NTP): ");
    Serial.println(net->getTime());

    Serial.println("\n====================\n");
}