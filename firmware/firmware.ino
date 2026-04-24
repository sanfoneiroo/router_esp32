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
//  SETUP
//  Inicializa todos os subsistemas do LabNode
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
//  Atualiza análise e interface de forma periódica
// =====================================================

void loop()
{
    // Atualiza estado da rede (clientes, RSSI, etc.)
    netAnalyzer.update();

    // Atualiza métricas de hardware (temp, heap)
    hwAnalyzer.update();

    // Mantém servidor web responsivo
    web.update();

    // Evita sobrecarga de CPU e rede
    delay(5000);
}

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