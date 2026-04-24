# ESP32 Lab Router
### Um mini-roteador projetado para laboratórios de rede

## Visão do projeto

Este projeto utiliza um ESP32 como um nó de rede programável para fins educacionais e experimentais em laboratórios de redes.

O objetivo não é construir um roteador doméstico funcional, mas sim um dispositivo de observação e experimentação de tráfego em redes locais, permitindo visualizar e analisar o comportamento de protocolos nas camadas 2 e 3 de forma simplificada e acessível.

O sistema atua como um pequeno gateway entre uma rede WiFi local (LAN) criada pelo ESP32 e uma rede externa (WAN), permitindo observar dispositivos conectados e métricas básicas de rede através de uma interface web.

---

## Objetivo

Este projeto propõe a criação de um nó de rede programável baseado em ESP32, projetado como uma ferramenta de laboratório para observação e experimentação de redes. Em vez de implementar um roteador completo de uso geral, o sistema expõe e instrumenta o fluxo de comunicação em uma rede local, permitindo analisar de forma prática o comportamento de dispositivos, conexões e protocolos nas camadas 2 e 3.

A proposta é servir como um **dispositivo de apoio para estudos de redes**, permitindo observar interações reais entre clientes e infraestrutura de rede em um ambiente controlado.

---

## Arquitetura

O sistema é dividido em módulos independentes:

- **Main**  
  Inicialização do sistema e loop principal.

- **System**  
  Flags de compilação e macros para controle de logs no serial monitor.

- **Hardware_Analyzer**  
  Coleta métricas internas do dispositivo:
  - temperatura do chip
  - memória heap disponível
  - MAC address do router

- **Net_Analyzer**  
  Responsável por observar o estado da rede:
  - clientes conectados ao AP
  - MAC address dos dispositivos
  - intensidade de sinal (RSSI)
  - IP público obtido via WAN
  - horário sincronizado via NTP

- **Router**  
  Configuração do modo dual do ESP32:
  - Access Point (LAN)
  - Station (WAN)
  - NAT utilizando lwIP NAPT

- **Web_Server**  
  Interface web para configurações e visualização das métricas do sistema e estado da rede.

---

## Conceitos abordados

### Camada 1 (Physical Layer / Hardware)

- métricas internas do dispositivo
- temperatura do chip
- memória heap disponível

### Camada 2 (Link Layer)

- associação de clientes ao Access Point
- intensidade de sinal (RSSI)
- identificação de dispositivos via MAC address

### Camada 3 (Network Layer)

- NAT entre redes (STA ↔ AP)
- endereçamento IP local e externo
- fluxo de pacotes através do ESP32

### Provisionamento de rede (WAN)

- configuração da conexão externa (WAN) via interface web
- armazenamento persistente de credenciais WiFi
- reconfiguração da WAN diretamente pelo dashboard
- separação conceitual entre rede local (LAN) e rede externa (WAN)

### Persistência de configuração

- uso do **Preferences (NVS)** do ESP32
- armazenamento de SSID e senha da WAN
- restauração automática das configurações após reboot
- reset de configuração via interface web

---

## Fluxo de Boot

Ao iniciar, o firmware verifica se já existem credenciais de rede armazenadas no NVS.

O comportamento do sistema depende desse estado:
Boot
 │
 ├── credenciais WAN armazenadas?
 │
 ├── NÃO
 │     │
 │     └── Provision Mode
 │           - ESP32 cria Access Point
 │           - interface web de configuração
 │           - usuário informa SSID e senha da WAN
 │           - credenciais salvas no NVS
 │           - dispositivo reinicia
 │
 └── SIM
       │
       └── Router Mode
             - ESP32 conecta na rede externa (STA)
             - cria rede local via Access Point
             - ativa NAT (NAPT)
             - inicia dashboard de monitoramento


Caso o usuário deseje alterar a rede externa, é possível apagar as credenciais da WAN diretamente pela interface web. Após isso, o dispositivo reinicia e retorna ao modo de provisionamento.

## Interface Web

O ESP32 expõe uma interface leve acessível via: http://192.168.4.1


A interface permite configurar a internet (WAN) e visualizar métricas da rede e do sistema em tempo real.

### Dados exibidos

**Clientes conectados**

- número de dispositivos conectados
- MAC address
- intensidade de sinal (RSSI)

**Internet (WAN)**

- IP público obtido via consulta externa
- estado de conectividade

**Relógio**

- horário sincronizado via NTP

**Hardware**

- MAC address do router
- temperatura interna do chip
- memória heap livre

A página é atualizada automaticamente a cada poucos segundos utilizando **AJAX**, evitando recarregamento completo da interface.

Também é possível **resetar a configuração da WAN**, permitindo reconfigurar a conexão com a rede externa.

---

## Limitações do modelo

Este sistema não pretende ser um roteador completo.

Limitações conhecidas:

- NAT simplificado baseado em lwIP
- ausência de firewall stateful
- não garante suporte completo a ICMP (ex: traceroute)
- throughput limitado pelo hardware WiFi do ESP32
- ausência de QoS ou roteamento avançado
- descoberta limitada de IPs dos clientes conectados
- DHCP simplificado

O foco do projeto é **observação e experimentação**, não desempenho.

---

## Motivação técnica

A maioria dos estudos de redes ocorre em ambientes virtuais (simuladores ou emuladores).

Este projeto propõe complementar essa abordagem com:

**observação de comportamento real em hardware mínimo**

Isso permite explorar aspectos práticos como:

- associação real de dispositivos
- comportamento de RSSI
- NAT em hardware embarcado
- latência e conectividade reais
- limitações práticas de sistemas embarcados em redes

---

## Tecnologias utilizadas

- ESP32 (WiFi AP + STA)
- lwIP (NAPT / NAT)
- WebServer (HTTP interface)
- NTP (sincronização de tempo)
- HTTPClient (consulta externa de IP)
- Preferences (armazenamento persistente de credenciais)
- Portal de provisionamento WiFi implementado manualmente
- Arduino framework