
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

// Pinos (ajuste se trocar a fiação)
const int PIN_TRIG = 4; // TRIG do sensor
const int PIN_ECHO = 5; // ECHO via divisor de tensão

const int SDA_PIN = 16;
const int SCL_PIN = 15;


HCSR04 hc(4, 5); // initialisation class HCSR04 (trig pin , echo pin)

WiFiClient conexao;
MQTTClient mqtt(1000);

unsigned long instanteAnterior = 0;
int count_vaga_vazia_distancia = 0;
int count_vaga_cheia_distancia = 0;
int count_vaga_cheia_mag = 0;
int count_vaga_vazia_mag = 0;


int count_vaga_vazia_magnetico = 0;
int count_vaga_cheia_magnetico = 0;

JsonDocument dados_da_vaga;
String dados_da_vaga_string;
String Distancia_String;
int ID_vaga = 1;
bool Ja_mandou_vazia = false;
bool Ja_mandou_cheia = false;

//Variáveis para o vetor campo magnético
int bx1; int by1; int bz1;
int bx2; int by2; int bz2;

int paridade_leitura_magnetica = 0;

QMC5883LCompass compass;

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

int Modulo_do_inteiro(int x)
{
  if(x <0 )
  {
    x= -x;
  }
  return x;
}

void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println("Olá mundo!!!");

  rgbLedWrite(RGB_BUILTIN, 0, 255, 0);

  reconectarWiFi();

  Wire.begin(SDA_PIN, SCL_PIN);

  compass.init();
  Serial.println("GY-271 inciado");

  mqtt.begin("mqtt.janks.dev.br", 1883, conexao);
  mqtt.onMessage(recebeuMensagem);
  mqtt.setKeepAlive(10);
  mqtt.setWill("tópico da desconexão", "conteúdo");
  reconectarMQTT();
  dados_da_vaga["disponivel"] = "Sim";
  dados_da_vaga["idvaga"] = ID_vaga;
}

void loop()
{
 
    

  unsigned long instanteAtual = millis();
  if (instanteAtual > instanteAnterior + 1000)
  {
    
    if (paridade_leitura_magnetica == 0)
    {
    compass.read();
    bx1 = compass.getX();
    by1 = compass.getY(); 
    bz1 = compass.getZ();
    paridade_leitura_magnetica = 1;

    Serial.println(" cm");
    Serial.print("X: ");
    Serial.print(bx1);
    Serial.print("  Y: ");
    Serial.print(by1);
    Serial.print("  Z: ");
    Serial.println(bz1);
    }
    else if(paridade_leitura_magnetica == 1)
    {
      compass.read();
    bx2 = compass.getX();
    by2 = compass.getY(); 
    bz2 = compass.getZ();
    paridade_leitura_magnetica = 0;

    Serial.println(" cm");
    Serial.print("X: ");
    Serial.print(bx2);
    Serial.print("  Y: ");
    Serial.print(by2);
    Serial.print("  Z: ");
    Serial.println(bz2);
    }

    
// Faz duas medidas do x,y e z e vê se o delta da maior do que 500 três vezes seguidas.


    int var_bx = bx1 - bx2;
    int var_by = by1 - by2;
    int var_bz = bz1 - bz2;
    
    var_bx =  Modulo_do_inteiro(var_bx);
    var_by =  Modulo_do_inteiro(var_by);
    var_bz =  Modulo_do_inteiro(var_bz);

   

    if(var_bx > 500 || var_by > 500 || var_bz > 500 )
    {
      count_vaga_cheia_mag +=1;
      count_vaga_vazia_mag = 0;
    }
    else
    {
      count_vaga_vazia_mag += 1;
      count_vaga_cheia_mag = 0;
    }

    Distancia_String = String(hc.dist());
    float Distancia_Float = Distancia_String.toFloat();

    Serial.println("+1 segundo");
    Serial.print(Distancia_String);  // return current distance (cm) in serial
    
     
  



    if (Distancia_Float < 100)
    {
      count_vaga_cheia_distancia += 1;
      count_vaga_vazia_distancia = 0;
    }
    else
    {
      count_vaga_cheia_distancia = 0;
      count_vaga_vazia_distancia += 1;
    }




    if ((count_vaga_cheia_distancia > 3 || count_vaga_cheia_mag >3) && Ja_mandou_cheia == false)
    {
      rgbLedWrite(RGB_BUILTIN, 255, 0, 0);
      
      dados_da_vaga["distancia"] = Distancia_Float;
      dados_da_vaga["disponivel"] = "Nao";
      serializeJson(dados_da_vaga, dados_da_vaga_string);
      mqtt.publish("distancia/01", dados_da_vaga_string);
      
      Ja_mandou_vazia = false;
      Ja_mandou_cheia = true;
    }

    if ((count_vaga_vazia_distancia > 3 || count_vaga_cheia_mag > 3) && Ja_mandou_vazia == false)
    {
      rgbLedWrite(RGB_BUILTIN, 0, 255, 0);
      
      dados_da_vaga["distancia"] = Distancia_Float;
      dados_da_vaga["disponivel"] = "Sim";
      serializeJson(dados_da_vaga, dados_da_vaga_string);
      mqtt.publish("distancia/01", dados_da_vaga_string);
     
      Ja_mandou_vazia = true;
      Ja_mandou_cheia =false;
    }

    instanteAnterior = instanteAtual;
  }

  
  reconectarWiFi();
  reconectarMQTT();
  mqtt.loop();
}
