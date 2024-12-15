#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Defina as credenciais do Wi-Fi
const char* ssid = "Helder";  
const char* password = "87654321";  

// Defina as configurações do MQTT
const char* mqtt_server = "0abf64bf9e3c40a69fddb2f37f3073d8.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;  
const char* mqtt_user = "hivemq.webclient.1733795922610";  
const char* mqtt_pass = "QV.Ex45#&asp,dGP3o6M";  

// Tópicos para publicar e assinar
const char* statusTopic = "dispenser/status";
const char* distanceTopic = "dispenser/distance"; // Tópico para distância do sensor

WiFiClientSecure espClient;  // Usando WiFiClientSecure para TLS
PubSubClient client(espClient);

// Variáveis de controle de tempo
unsigned long lastMessageTime = 0;
unsigned long messageInterval = 10000; 

String lastStatus = ""; 

void setup() {
  Serial.begin(19200); // Monitor serial
  Serial2.begin(19200, SERIAL_8N1, 16, 17); // Serial2 no ESP32 (RX2: GPIO 16, TX2: GPIO 17)

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Wi-Fi conectado!");

  // Configuração do cliente MQTT com TLS
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  // Conexão ao MQTT com autenticação
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("Conectado ao MQTT!");
      client.subscribe("dispenser/command");
      client.publish(statusTopic, "Pronto");
    } else {
      Serial.print("Falha ao conectar, estado: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  client.loop();

  // Verifica se há dados na porta serial (enviado pelo Arduino)
  if (Serial2.available() > 0) {
    String message = Serial2.readStringUntil('\n');  
    message.trim();  


    unsigned long currentMillis = millis();
    if (currentMillis - lastMessageTime >= messageInterval && message != lastStatus) {

      Serial.print("Status recebido: ");
      Serial.println(message);

      if (message == "Liberado") {
        client.publish(statusTopic, "Liberado");
      } else if (message == "Bloqueado") {
        client.publish(statusTopic, "Bloqueado");
      } else {
        client.publish(statusTopic, "aqui");
      }

      // Atualiza o tempo do último envio e o status
      lastMessageTime = currentMillis;
      lastStatus = message;
    }
  }

  // Garantir que a conexão MQTT não seja perdida
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.println("Reconectando ao MQTT...");
      if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
        Serial.println("Reconectado ao MQTT!");
        client.subscribe("dispenser/command");
        client.publish(statusTopic, "Reiniciado");
      } else {
        Serial.print("Falha ao reconectar, estado: ");
        Serial.println(client.state());
        delay(2000);
      }
    }
  }
}
