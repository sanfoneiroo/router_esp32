#include "net_analyzer.h"
#include "router.h"
#include "system.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

extern "C" {
#include "esp_wifi.h"
}

// =====================================================
// TIME INIT (NTP)
// =====================================================

static void initTime()
{
    configTime(-3 * 3600, 0, "pool.ntp.org");
}

// =====================================================
// BEGIN
// =====================================================

void NetAnalyzer::begin(Router* r)
{
    router = r;
    initTime();
}

// =====================================================
// UPDATE
// =====================================================

void NetAnalyzer::update()
{
    // =================================================
    // WIFI CLIENTS (MAC + RSSI)
    // =================================================

    wifi_sta_list_t sta_list;

    clientCount = 0;

    if (esp_wifi_ap_get_sta_list(&sta_list) == ESP_OK)
    {
        clientCount = sta_list.num;

        for (int i = 0; i < clientCount && i < 10; i++)
        {
            wifi_sta_info_t station = sta_list.sta[i];

            memcpy(clients[i].mac, station.mac, 6);
            clients[i].rssi = station.rssi;
        }
    }

    // =================================================
    // PUBLIC IP
    // =================================================

    static unsigned long lastIPUpdate = 0;

    if (millis() - lastIPUpdate > 60000 || publicIP == "")
    {
        lastIPUpdate = millis();

        HTTPClient http;

        http.begin("http://api.ipify.org");

        int code = http.GET();

        if (code == 200)
        {
            publicIP = http.getString();
        }

        http.end();
    }

#if SERIAL_MONITOR_ENABLED

    LOGLN("---- NET ANALYZER ----");

    LOG("Clientes conectados: ");
    LOGLN(clientCount);

    for (int i = 0; i < clientCount; i++)
    {
        char macStr[18];

        sprintf(macStr,
        "%02X:%02X:%02X:%02X:%02X:%02X",
        clients[i].mac[0],
        clients[i].mac[1],
        clients[i].mac[2],
        clients[i].mac[3],
        clients[i].mac[4],
        clients[i].mac[5]);

        LOG("MAC: ");
        LOGLN(macStr);

        LOG("RSSI: ");
        LOGLN(clients[i].rssi);

        LOGLN("");
    }

    LOG("IP publico: ");
    LOGLN(publicIP);

    LOG("Hora: ");
    LOGLN(getTime());

#endif
}

// =====================================================
// CLIENT API
// =====================================================

int NetAnalyzer::getClientCount()
{
    return clientCount;
}

ClientInfo* NetAnalyzer::getClients()
{
    return clients;
}

// =====================================================
// PUBLIC IP
// =====================================================

String NetAnalyzer::getPublicIP()
{
    return publicIP;
}

// =====================================================
// TIME
// =====================================================

String NetAnalyzer::getTime()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return "sync...";

    char buffer[64];

    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);

    return String(buffer);
}