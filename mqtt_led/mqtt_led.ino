#include <PubSubClient.h>

#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define POWER D2

/*
 * Endereço do broker e
 * suas credenciais
 */
const char *broker = "m13.cloudmqtt.com";
const char *user = "cadztvug";
const char *pass = "GD-y5WMun5kV";
const int   port = 13904;

/*
 * Inicializar cliente MQTT
 */
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);
void reconectar();

bool estado = false;

void setup() {
 /*
  * Configuração da conexão WiFi
  */
  Serial.begin(115200);
  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.autoConnect("MQTT_FLISoL", "flisol2017");
  Serial.println("Conectado :)");
  /*
   * Conexão com o broker e subscribe no 
   * tópico para ligar e desligar o led
   */
   client.setServer(broker, port);
   client.setCallback(callback);
   client.subscribe("/led1/acionamento");
   /*
    * Definir pino de power
    * como saída
    */
   pinMode(POWER, OUTPUT);
}

void loop() {
    /*
     * Verificar se está conectado
     * ao broker
     */
    if(!client.connected())
    {
      reconectar();
    }
    client.loop();
    /*
     * Verifica o estado do LED
     * e publica no respectivo tópico
     */
    if(estado)
    {
      client.publish("/led1/estado", "LED Ligado");
    }
    else
    {
      client.publish("/led1/estado", "LED Desligado");
    }
    delay(2000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(length != 1)
  {
    Serial.println("Comando inválido! Digite '1' para ligar e '0' para desligar.");
  }
  else {
    /*
     * Ao receber uma mensagem no tópico "/led1/acionamento",
     * verificará se é para ligar ou desligar o LED
     */
    switch((char) payload[0]) {
      case '1':
      {
        digitalWrite(POWER, HIGH);
        estado = true;
        break;
      }
      case '0':
      {
        digitalWrite(POWER, LOW);
        estado = false;
        break;  
      }
      default:
      {
        Serial.println("Comando inválido!");
      }
    }
  }
}

/*
 * Função para conectar/reconectar
 * ao broker
 */
void reconectar() {
  while(!client.connected()) {
    Serial.println("Conectando ao Broker MQTT.");
    if(client.connect("ESP8266", user, pass)) 
    {
      Serial.println("Conectado com Sucesso ao Broker");
      client.subscribe("/led1/acionamento");
    } 
    else 
    {
      Serial.print("Falha ao Conectador, rc=");
      Serial.print(client.state());
      Serial.println(" tentando se reconectar...");
      delay(3000);
    }
  }
}


