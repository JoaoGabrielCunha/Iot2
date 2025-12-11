
#include <GFButton.h>
#include <Adafruit_BME680.h>
#include <HCSR04.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "certificados.h"
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
#include <esp_now.h>


// Pinos (ajuste se trocar a fiação)
const int PIN_TRIG = 4; // TRIG do sensor
const int PIN_ECHO = 5; // ECHO via divisor de tensão

//Pinos do sensor magnético
const int SDA_PIN = 16;
const int SCL_PIN = 15;

QMC5883LCompass compass; 

uint8_t broadcastAddress[] = {0x26, 0xEC, 0x4A, 0x00, 0x79, 0x64}; //26:EC:4A:00:79:64

typedef struct struct_message {
  bool ocupado;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}



HCSR04 hc(4, 5); // initialisation class HCSR04 (trig pin , echo pin)

WiFiClient conexao;
MQTTClient mqtt(1000);

unsigned long instanteAnterior = 0;
int count_vaga_vazia = 0;
int count_vaga_cheia = 0;
JsonDocument dados_da_vaga;
String dados_da_vaga_string;
String Distancia_String;
float Distancia_Float;
int ID_vaga = 1;
bool Ja_mandou_vazia = false;
bool Ja_mandou_cheia = false;

bool sensor_distancia_falhou = false;

// variáveis do campo magnético
int bx0 = 0;
int by0 = 0;
int bz0 = 0;



void recebeuMensagem(String topico, String conteudo)
{
  Serial.println(topico + ": " + conteudo);
}

void reconectarWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin("Projeto", "2022-11-07");
    Serial.print("Conectando ao WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.print("conectado!\nEndereço IP: ");
    Serial.println(WiFi.localIP());
  }
}

void reconectarMQTT()
{
  if (!mqtt.connected())
  {
    Serial.print("Conectando MQTT...");
    while (!mqtt.connected())
    {
      mqtt.connect("JG", "aula", "zowmad-tavQez");
      Serial.print(".");
      delay(1000);
    }
    Serial.println(" conectado!");

    mqtt.subscribe("topico1");                // qos = 0
    mqtt.subscribe("topico2/+/parametro", 1); // qos = 1
  }
}

int modulo_inteiro(int x)
{
  if(x<0)
  {
    x = -x;
  }
  return x;
}

void Faz_checkagem_e_envio_pelo_sensor_de_DISTANCIA()
{
  // Faz verificação por distância e envia o dado se o status da vaga falhar.
    Distancia_String = String(hc.dist());
    Distancia_Float = Distancia_String.toFloat();

     sensor_distancia_falhou = (isnan(Distancia_Float)) || Distancia_Float <= 1 || Distancia_Float> 500;

    if(Distancia_Float !=0)
    {
   
    Serial.print(Distancia_String); // return current distance (cm) in serial
    Serial.println(" cm");

    if (Distancia_Float < 100)
    {
      count_vaga_cheia += 1;
      count_vaga_vazia = 0;
    }
    else
    {
      count_vaga_cheia = 0;
      count_vaga_vazia += 1;
    }

    if (count_vaga_cheia > 3 && Ja_mandou_cheia == false)
    {
      rgbLedWrite(RGB_BUILTIN, 255, 0, 0);
      
      dados_da_vaga["distancia"] = Distancia_Float;
      dados_da_vaga["disponivel"] = "Nao";
      serializeJson(dados_da_vaga, dados_da_vaga_string);
      mqtt.publish("distancia/01", dados_da_vaga_string);

      myData.ocupado = true;

          
      Ja_mandou_vazia = false;
      Ja_mandou_cheia = true;
    }

    if (count_vaga_vazia > 3 && Ja_mandou_vazia == false)
    {
      rgbLedWrite(RGB_BUILTIN, 0, 255, 0);
      
      dados_da_vaga["distancia"] = Distancia_Float;
      dados_da_vaga["disponivel"] = "Sim";
      serializeJson(dados_da_vaga, dados_da_vaga_string);
      mqtt.publish("distancia/01", dados_da_vaga_string);

      myData.ocupado = false;

      
     
      Ja_mandou_vazia = true;
      Ja_mandou_cheia =false;
    }

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }

    }
    

}

void Faz_checkagem_e_envio_pelo_sensor_MAGNETICO()
{
  
  if ((Ja_mandou_cheia == false && Ja_mandou_cheia == false) || (bx0 == 0 && by0 == 0 && bz0 == 0))
  { compass.read();
    bx0 = compass.getX();
    by0 = compass.getY();                 // Primeira leitura caso não haja leitura nenhuma ainda
    bz0 = compass.getZ();
  }

  compass.read();
  int bx1 = compass.getX();
  int by1 = compass.getY();
  int bz1 = compass.getZ();

  int delta_x;
  int delta_y;
  int delta_z;

  delta_x = bx0 - bx1;
  delta_y = by0 - by1;
  delta_z = bz0 - bz1;

  delta_x = modulo_inteiro(delta_x);
  delta_y = modulo_inteiro(delta_y);
  delta_z = modulo_inteiro (delta_z);

  Serial.print("Leitura campo magnético x: ");
  Serial.println(bx1);
  Serial.print("Leitura campo magnético y: ");
  Serial.println(by1);
  Serial.print("Leitura campo magnético z: ");
  Serial.println(bz1);

  Serial.print("Variação do campo em x: ");
  Serial.println(delta_x);
  Serial.print("Variação do campo em y: ");
  Serial.println(delta_y);
  Serial.print("Variação do campo em z:  ");
  Serial.println(delta_z);




  if (delta_x >400 || delta_y > 400 || delta_z > 400)
  {
    bx0 = bx1;
    by0 = by1;
    bz0 = bz1;

    if(Ja_mandou_cheia == false && Ja_mandou_vazia == true)
    {
      rgbLedWrite(RGB_BUILTIN, 255, 0, 0);

      dados_da_vaga["distancia"] = Distancia_Float;
      dados_da_vaga["disponivel"] = "Nao";
      serializeJson(dados_da_vaga, dados_da_vaga_string);
      mqtt.publish("distancia/01", dados_da_vaga_string);
      

      Ja_mandou_vazia == false;
      Ja_mandou_cheia = true;

    }

    if (Ja_mandou_cheia == true && Ja_mandou_vazia == false)
    {
      rgbLedWrite(RGB_BUILTIN, 0, 255, 0);

      dados_da_vaga["distancia"] = Distancia_Float;
      dados_da_vaga["disponivel"] = "Sim";
      serializeJson(dados_da_vaga, dados_da_vaga_string);
      mqtt.publish("distancia/01", dados_da_vaga_string);
     
      Ja_mandou_vazia = true;
      Ja_mandou_cheia =false;


    }

  }
}


void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println("Olá mundo!!!");

  WiFi.mode(WIFI_STA);

  rgbLedWrite(RGB_BUILTIN, 0, 255, 0);

  reconectarWiFi();

  mqtt.begin("mqtt.janks.dev.br", 1883, conexao);
  mqtt.onMessage(recebeuMensagem);
  mqtt.setKeepAlive(10);
  mqtt.setWill("tópico da desconexão", "conteúdo");
  reconectarMQTT();
  dados_da_vaga["disponivel"] = "Sim";
  dados_da_vaga["idvaga"] = ID_vaga;

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  // Inicializa I2C nos pinos escolhidos
  Wire.begin(SDA_PIN, SCL_PIN);

  // Inicializa o magnetômetro
  compass.init();

  Serial.println("GY-271 iniciado");



}

void loop()
{

  unsigned long instanteAtual = millis();
  if (instanteAtual > instanteAnterior + 1000)
  {
    Serial.println("+1 segundo");
    Faz_checkagem_e_envio_pelo_sensor_de_DISTANCIA();

    // Se a distância falhar vamos para a checkagem e envio magnético, falha quando a distância lida é zero.

    if ( sensor_distancia_falhou == true)
    {
      Faz_checkagem_e_envio_pelo_sensor_MAGNETICO();
    }

    instanteAnterior = instanteAtual;
  }

  
  reconectarWiFi();
  reconectarMQTT();
  mqtt.loop();
}
