# Sistema de Monitoramento e Controle de Vazamento com ESP32 e MQTT

## 📌 Visão Geral

Este projeto tem como objetivo demonstrar a aplicação prática da Internet das Coisas (IoT) no monitoramento de fluxo de água e pressão em sistemas hidráulicos residenciais. Utilizando um ESP32 NodeMCU, sensores de vazão e pressão, e uma válvula solenoide controlada por relé, é possível detectar vazamentos automaticamente e interromper o fluxo de água em tempo real via MQTT.

Estudo feito para a disciplina de _Objetos Inteligentes Conectados_ da faculdade de Análise e Desenvolvimento de Sistemas da Universidade Presbiteriana Mackenzie.

## ⚙️ Funcionamento

- **Sensores:** Capturam o fluxo e a pressão da água em tempo real.
- **ESP32 NodeMCU:** Processa os dados, envia via MQTT e recebe comandos para controlar a válvula.
- **MQTT + Interface Web:** Visualiza os dados, inicia a coleta de baseline e envia comandos para abrir/fechar a válvula.

## 🧠 Software

### ESP32 NodeMCU (Arduino IDE)

- Leitura do sensor de fluxo (pino 25).
- Leitura do sensor de pressão (pino 34).
- Controle do relé (pino 23).
- Comunicação via MQTT com o broker público `test.mosquitto.org`.
- Bibliotecas necessárias:
```
#include <WiFi.h>
#include <PubSubClient.h>
```

**Publica em:**
- `fluxoagua/teste`

**Recebe comandos:**
- `controle/valvula` (`abrir` ou `fechar`)

### HTML/JS

- Interface com inputs dinâmicos de tempo de aprendizado e tolerâncias.
- Detecção automática de vazamentos comparando baseline com dados atuais.
- Envio de comandos para controle da válvula via MQTT.

## 🔧 Hardware

- **Plataforma:** ESP32 NodeMCU
- **Sensor de fluxo:** YF-S201b
- **Sensor de pressão:** USP-G41
- **Relé:** Módulo relé 5V 1 canal
- **Válvula solenoide:** 220V (normalmente fechada)
- **Fonte ajustável de protoboard:** Para alimentação externa dos sensores e controle da corrente elétrica e tensão
- **Protoboard e jumpers**

## 📡 Comunicação e Protocolos

- **WiFi:** ESP32 conectado à rede local
- **MQTT:** Utilização do broker público Mosquitto (`test.mosquitto.org`)
  - **Tópicos utilizados:** `fluxoagua/teste`, `controle/valvula`
- **WebSocket:** Usa a biblioteca `Paho MQTT` via WebSocket seguro (porta 8081) para comunicação com broker MQTT

## ✅ Requisitos para reproduzir

- Usar o board `NodeMCU-32S` na Arduido IDE (disponível em https://espressif.github.io/arduino-esp32/package_esp32_index.json)
- Sensores e atuador conforme listagem
- Fonte 12V e conexões estáveis
- Broker MQTT acessível
- Navegador com suporte a WebSockets (Chrome, Firefox)

## 📁 Organização dos Arquivos

- `index.html` — Interface de controle e visualização
- `main.ino` — Código-fonte para ESP32 (Arduino IDE)
- `README.md` — Documentação completa do projeto


