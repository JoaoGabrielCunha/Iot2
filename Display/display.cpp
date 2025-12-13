# include <Arduino.h>
# include <GxEPD2_BW.h>
# include <QRCodeGFX.h>
# include <U8g2_for_Adafruit_GFX.h>
# include <WiFi.h>
# include <esp_wifi.h>
# include <esp_now.h>
# include <time.h>

U8G2_FOR_ADAFRUIT_GFX fontes;
GxEPD2_290_T94_V2 modeloTela(10, 14, 15, 16);
GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> tela(modeloTela);

QRCodeGFX qrcode(tela);

String codigo = "00000011";
String matricula = "0000000";

long timezone = -3;
byte daysavetime = 1;

bool ocupado = false;
int hora = 0;
int minuto = 0;
int segundo = 0;

typedef struct struct_message {
    bool ocupado;
} struct_message;

struct_message myData;

bool is_busy = 0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.println(is_busy);
  if (myData.ocupado != is_busy){
    Serial2.println("AT+SEND=1:o-"+matricula);
    if(is_busy){
      Serial1.print("~M00210001.");

      ocupado = 0;
      
      tela.fillScreen(GxEPD_WHITE);
      tela.display(true);
      tela.fillScreen(GxEPD_WHITE);
      tela.display(true);
      tela.fillScreen(GxEPD_WHITE);
      tela.display(true);
      tela.fillScreen(GxEPD_WHITE);
      tela.display(true);

      fontes.setFont( u8g2_font_helvB24_te );
      fontes.setFontMode(1);
      fontes.setCursor(60, 70);
      fontes.print("Vaga Livre");
      tela.display(true);

      matricula = "0000000";
    }
  }
  is_busy = myData.ocupado;
}

void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void setup() {
  Serial.begin(115200); delay(500);
  Serial1.begin(9600, SERIAL_8N1, 35, 36); //Leitor QR

  Serial1.println("~M00910001.");
  delay(100);
  Serial1.println("~M00210001.");
  delay(100);
  Serial1.println("~M00B00014.");
  delay(100);
  Serial1.println("~M01030000.");

  Serial2.begin(9600, SERIAL_8N1, 48, 47); //Lora

  tela.init();
  tela.setRotation(3);
  tela.fillScreen(GxEPD_WHITE);
  tela.display(true);

  fontes.begin(tela);
  fontes.setForegroundColor(GxEPD_BLACK);

  fontes.setFont( u8g2_font_helvB24_te );
  fontes.setFontMode(1);
  fontes.setCursor(60, 70);
  fontes.print("Vaga Livre");
  tela.display(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin();

  Serial.print("Endereço MAC:\t");readMacAddress();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));


  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");

  Serial2.println("AT+JOIN");
}


void displayTime(){
  struct tm tmstruct;
  getLocalTime(&tmstruct);

  char time[6];
  sprintf(time, "%02d:%02d", tmstruct.tm_hour, tmstruct.tm_min);
  Serial.println(time);

  tela.fillScreen(GxEPD_WHITE);
  fontes.setFont( u8g2_font_helvR18_te );
  fontes.setFontMode(1);
  fontes.setCursor(40, 40);
  fontes.print("Horario de entrada:");
  fontes.setFont( u8g2_font_helvR24_te );
  fontes.setFontMode(1);
  fontes.setCursor(100, 80);
  fontes.print(time);
  tela.display(true);


  ocupado = true;
  hora = 0;
  minuto = 0;
  segundo = 0;
}

String hexadecimalParaTexto(String textoHex) {
  String resultado = "";
  textoHex.replace(" ", "");

  for (int i = 0; i < textoHex.length(); i += 2) {
    String par = textoHex.substring(i, i + 2);
    char caractere = (char)strtol(par.c_str(), NULL, 16);
    resultado += caractere;
  }
  return resultado;
}

unsigned long previous = 0;

void loop() {

  if (ocupado){
    unsigned long now = millis();
    if(now > previous + 1000){
      segundo++;
      minuto += segundo/60;
      hora += minuto/60;
      minuto %= 60;
      segundo %= 60;

      char time[9];
      sprintf(time, "%02d:%02d:%02d", hora, minuto, segundo);

      tela.fillRect(100, 90, 100, 30, GxEPD_WHITE);
      fontes.setFont( u8g2_font_helvR18_te );
      fontes.setFontMode(1);
      fontes.setCursor(100, 110);
      fontes.print(time);
      tela.display(true);

      previous = now;
    }
  }

  if (Serial1.available() > 0) {
    String texto = Serial1.readStringUntil('\n');
    texto.trim();
    if (texto.length() > 5) {
        Serial.println("Resposta do leitor: " + texto);
        Serial2.println("AT+SEND=1:m-"+texto);
    }
  }

  if (Serial2.available() > 0) {
    String texto = Serial2.readStringUntil('\n');
    texto.trim();
    Serial.println("Resposta do módulo LoRaWAN: " + texto);

    int inicio = texto.indexOf("RX:") + 3;
    int fim = texto.indexOf(":", inicio);

    texto = texto.substring(inicio, fim);
    texto = hexadecimalParaTexto(texto);

    Serial.println("Resposta do módulo LoRaWAN: " + texto);

    if(texto.startsWith("m-")){
      matricula = texto.substring(2);
      Serial.println("Resposta do módulo LoRaWAN: " + matricula);

      Serial1.print("~M00210003.");
      displayTime();
    }
  }
}

