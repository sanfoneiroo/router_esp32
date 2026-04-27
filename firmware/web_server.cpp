#include "web_server.h"
#include "net_analyzer.h"
#include "hw_analyzer.h"

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// =====================================================
// SERVER
// =====================================================

WebServer server(80);

// =====================================================
// MODULE POINTERS
// =====================================================

NetAnalyzer* netPtr;
HwAnalyzer* hwPtr;

Preferences webPrefs;

// =====================================================
// PROVISION PAGE
// =====================================================

void handleProvision()
{
    String html = R"rawliteral(

<html>
<head>
<title>ESP32 Router Setup</title>
</head>

<body>

<h1>Configurar Internet (WAN)</h1>

<form action="/connect" method="POST">

<b>Rede externa (WAN)</b><br><br>

SSID:<br>
<input name="ssid"><br><br>

Senha:<br>
<input name="pass" type="password"><br><br>

<hr>

<b>Rede do roteador (AP)</b><br><br>

SSID do roteador:<br>
<input name="ap_ssid" value="ESP32_ROUTER"><br><br>

Senha do roteador:<br>
<input name="ap_pass" type="password" value="12345678"><br><br>

<input type="submit" value="Salvar e Conectar">

</form>

</body>
</html>

)rawliteral";

    server.send(200,"text/html",html);
}

// =====================================================
// RECEIVE WIFI CONFIG
// =====================================================

void handleConnect()
{
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    String ap_ssid = server.arg("ap_ssid");
    String ap_pass = server.arg("ap_pass");

    // valida senha do AP (mínimo 8 caracteres)
    if(ap_pass.length() < 8)
{
    server.send(200,"text/html",
    "<html>"
    "<head>"
    "<meta http-equiv='refresh' content='3;url=/' />"
    "</head>"
    "<body>"
    "<h3>Senha precisa ter pelo menos 8 caracteres</h3>"
    "<p>Voltando para config...</p>"
    "</body>"
    "</html>");
    return;
}

    webPrefs.begin("router", false);

    webPrefs.putString("ssid", ssid);
    webPrefs.putString("pass", pass);

    webPrefs.putString("ap_ssid", ap_ssid);
    webPrefs.putString("ap_pass", ap_pass);

    webPrefs.end();

    server.send(200,"text/html","Configuracao salva! Reiniciando...");

    delay(1000);
    ESP.restart();
}

// =====================================================
// RESET WAN CONFIG
// =====================================================

void handleReconfigure()
{
    webPrefs.begin("router", false);

    webPrefs.remove("ssid");
    webPrefs.remove("pass");

    webPrefs.end();

    server.send(200,"text/html","Configuracao de Internet apagada. Reiniciando...");

    delay(1000);
    ESP.restart();
}

// =====================================================
// ROOT PAGE (ROUTER UI)
// =====================================================

void handleRoot()
{
    String html = R"rawliteral(

<html>
<head>

<title>ESP32 Router</title>

<script>

function updateStatus(){

fetch("/status")
.then(response => response.text())
.then(data => {

document.getElementById("status").innerHTML = data;

});

}

setInterval(updateStatus,2000);

function reconfigure(){

fetch("/reconfigure");

}

</script>

</head>

<body onload="updateStatus()">

<h1>ESP32 Router</h1>

<button onclick="reconfigure()">
Reconfigurar Internet (WAN)
</button>

<hr>

<div id="status">
carregando...
</div>

</body>
</html>

)rawliteral";

    server.send(200,"text/html",html);
}

// =====================================================
// STATUS PAGE
// =====================================================

void handleStatus()
{
    String html;

    int count = netPtr->getClientCount();
    ClientInfo* clients = netPtr->getClients();

    html += "<h2>Clientes conectados: ";
    html += count;
    html += "</h2>";

    for (int i = 0; i < count; i++)
    {
        html += "<b>Cliente ";
        html += i + 1;
        html += "</b><br>";

        char macStr[18];

        sprintf(macStr,
            "%02X:%02X:%02X:%02X:%02X:%02X",
            clients[i].mac[0],
            clients[i].mac[1],
            clients[i].mac[2],
            clients[i].mac[3],
            clients[i].mac[4],
            clients[i].mac[5]
        );

        html += "MAC: ";
        html += macStr;
        html += "<br>";

        html += "RSSI: ";
        html += clients[i].rssi;
        html += "<br><br>";
    }

    html += "<h2>Internet</h2>";
    html += "IP publico: ";
    html += netPtr->getPublicIP();
    html += "<br>";
    html += "Data / Hora: ";
    html += netPtr->getTime();
    html += "<br>";

    html += "<h2>Status do Roteador</h2>";

    html += "MAC Router: ";
    html += hwPtr->getMac();
    html += "<br>";

    html += "Uptime: ";
    html += hwPtr->getUptimeString();
    html += "<br>";

    html += "Temperatura: ";
    html += hwPtr->getTemperature();
    html += " C<br>";

    html += "Heap livre: ";
    html += hwPtr->getHeap() / 1024;
    html += " KB";

    server.send(200,"text/html",html);
}

// =====================================================
// PROVISION MODE
// =====================================================

void WebServerModule::beginProvision()
{
    server.on("/", handleProvision);
    server.on("/connect", HTTP_POST, handleConnect);

    server.begin();
}

// =====================================================
// ROUTER MODE
// =====================================================

void WebServerModule::beginRouter(NetAnalyzer* net, HwAnalyzer* hw)
{
    netPtr = net;
    hwPtr = hw;

    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/reconfigure", handleReconfigure);

    server.begin();
}

// =====================================================
// LOOP
// =====================================================

void WebServerModule::update()
{
    server.handleClient();
}