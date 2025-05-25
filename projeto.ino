// Sensor de pressão
const int pressureInput = 34;  // Pino analógico para o sensor de pressão (A0)
const int pressureZero = 309; // Valores zero e max para calibragem considerando pressão atmosférica no local em 14,73 PSI
const int pressureMax =  988;
const int pressuretransducermaxPSI = 100; // PSI máximo do sensor (100 PSI)
float pressureValue = 0;  // Variável para armazenar a pressão

// Sensor de água
const int sensorPin = 25;  // Pino do sensor de fluxo de água
volatile unsigned int pulseCount;  // Contador de pulsos para sensor de fluxo
float flowRate = 0.0;  // Vazão em L/min
unsigned int flowMilliLitres = 0;  // Quantidade de água passada (mL)
unsigned long totalMilliLitres = 0;  // Total de água passada (mL)
unsigned long oldTime = 0;  // Armazena o tempo anterior

// Relé que controla válvula solenoide
const int relePin = 23;

#include <WiFi.h>
#include <PubSubClient.h>

// Credenciais WiFi
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

// HiveMQ Cloud (Serverless Plan)
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando em ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop até conectar
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // Se quiser se inscrever em um tópico:
      client.subscribe("controle/valvula");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// Interrupção para contar pulsos do sensor de fluxo
void pulseCounter() {
  pulseCount++;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  if (String(topic) == "controle/valvula") {
    if (mensagem == "abrir") {
      digitalWrite(relePin, HIGH);  
      Serial.println("Válvula ABERTA");
    } 
    else if (mensagem == "fechar") {
      digitalWrite(relePin, LOW);   
      Serial.println("Válvula FECHADA");
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  pinMode(relePin, OUTPUT);
  
  // Inicializa o sensor de fluxo
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH); // Ativa pull-up no pino
  pulseCount = 0;
  
  // Inicializa interrupção para contar pulsos
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  
  // Inicializa variáveis de fluxo
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = 0;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Calcula o tempo decorrido
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - oldTime;

  // Se passou 2 segundos desde a última leitura
  if (elapsedTime > 2000) {
    
    // Calcula a vazão
    flowRate = (1000.0 / (elapsedTime)) * pulseCount;
    
    // Resetar contador de pulsos
    pulseCount = 0;
    oldTime = currentTime;
    
    // Calcula a quantidade de água passada (em mL)
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Adiciona ao total
    totalMilliLitres += flowMilliLitres;
    
    // Imprime os resultados de vazão
    Serial.print("Vazão: ");
    Serial.print(flowRate);
    Serial.print(" L/min - ");
    Serial.print("Quantidade de água: ");
    Serial.print(flowMilliLitres);
    Serial.print(" mL/segundo - Total: ");
    Serial.print(totalMilliLitres);
    Serial.println(" mL");
    
    // Leitura do sensor de pressão
    int rawPressure = analogRead(pressureInput);  // Leitura analógica do sensor de pressão
    Serial.print("Raw pressure: ");
    Serial.println(rawPressure, 1);

    // Converte a leitura analógica para PSI (0-100 PSI)
    pressureValue = ((rawPressure - pressureZero) * pressuretransducermaxPSI) / (pressureMax - pressureZero);
    
    // Exibe o valor de pressão no monitor serial
    Serial.print("Pressão: ");
    Serial.print(pressureValue, 1);  // Exibe com 1 casa decimal
    Serial.println(" PSI");

    String payload = "{";
    payload += "\"flowRate\":" + String(flowRate, 2) + ",";
    payload += "\"flowMilliLitres\":" + String(flowMilliLitres, 2) + ",";
    payload += "\"totalMilliLitres\":" + String(totalMilliLitres, 2) + ",";
    payload += "\"pressure\":" + String(pressureValue, 1);
    payload += "}";

    // Exemplo: publicar uma mensagem
    client.publish("fluxoagua/teste", payload.c_str());
  }
}
