#include "router.h"
#include "web_server.h"
#include "net_analyzer.h"
#include "hw_analyzer.h"

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

extern "C" {
#include "lwip/lwip_napt.h"
#include "lwip/tcpip.h"
}

static NetAnalyzer* netPtr;
static HwAnalyzer* hwPtr;

// =====================================================
// POINTER PARA WEBSERVER
// =====================================================

WebServerModule* webPtr;

// =====================================================
// CONFIG
// =====================================================

// valores padrão caso não exista configuração salva
static String ap_ssid = "ESP32_ROUTER";
static String ap_pass = "12345678";

// NVS storage
Preferences routerPrefs;

// estado NAT
static bool nat_enabled = false;

// credenciais carregadas
static String sta_ssid;
static String sta_pass;

// =====================================================
// NAT CALLBACK
// =====================================================

static void enable_nat(void *arg)
{
    ip_napt_enable(IPAddress(192,168,4,1), 1);
    Serial.println("NAT habilitado");
}

// =====================================================
// STORAGE
// =====================================================

bool Router::configExists()
{
    routerPrefs.begin("router", true);

    sta_ssid = routerPrefs.getString("ssid", "");
    sta_pass = routerPrefs.getString("pass", "");

    ap_ssid = routerPrefs.getString("ap_ssid", "ESP32_ROUTER");
    ap_pass = routerPrefs.getString("ap_pass", "12345678");

    routerPrefs.end();

    return sta_ssid.length() > 0;
}

// =====================================================
// AP
// =====================================================

void Router::startAP()
{
    Serial.println("Iniciando AP...");

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(ap_ssid.c_str(), ap_pass.c_str());

    Serial.print("AP SSID: ");
    Serial.println(ap_ssid);

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

// =====================================================
// STA
// =====================================================

bool Router::connectSTA()
{
    Serial.print("Conectando STA -> ");
    Serial.println(sta_ssid);

    WiFi.begin(sta_ssid.c_str(), sta_pass.c_str());

    int retries = 0;

    while (WiFi.status() != WL_CONNECTED && retries < 20)
    {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nSTA conectado");

        Serial.print("IP WAN: ");
        Serial.println(WiFi.localIP());

        return true;
    }

    Serial.println("\nFalha STA");

    return false;
}

// =====================================================
// NAT
// =====================================================

void Router::enableNAT()
{
    if (!nat_enabled)
    {
        delay(2000);
        tcpip_callback(enable_nat, NULL);
        nat_enabled = true;
    }
}

// =====================================================
// PROVISION PORTAL
// =====================================================

void Router::startProvisionPortal()
{
    Serial.println("Modo provisionamento ativo");
    Serial.println("Conecte ao AP e configure o WiFi");

    if (webPtr)
    {
        webPtr->beginProvision();
    }
}

// =====================================================
// ROUTER MONITOR
// =====================================================

void Router::startRouterMonitor()
{
    Serial.println("Router monitor iniciado");

    if (webPtr)
    {
        webPtr->beginRouter(netPtr, hwPtr);
    }
}

// =====================================================
// BEGIN
// =====================================================

void Router::begin(WebServerModule* web, NetAnalyzer* net, HwAnalyzer* hw)
{
    webPtr = web;
    netPtr = net;
    hwPtr = hw;

    Serial.println("Inicializando router");

    startAP();

    if (!configExists())
    {
        startProvisionPortal();
        return;
    }

    if (connectSTA())
    {
        enableNAT();
        startRouterMonitor();
    }
    else
    {
        startProvisionPortal();
    }
}

// =====================================================
// CLIENTES
// =====================================================

int Router::getConnectedClients()
{
    return WiFi.softAPgetStationNum();
}