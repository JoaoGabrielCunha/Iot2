#include <Matter.h>
#include <GFButton.h>

RTC_DATA_ATTR int count = 0;  // agora persiste entre sleeps

void dormir() {
  Serial.println("Fui dormir!");
  count++;
  Serial.println(count);
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Acordar em 10 segundos
  esp_sleep_enable_timer_wakeup(10e6);

 
}

void loop() {
  rgbLedWrite(RGB_BUILTIN, 0, 0, 255);
  Serial.println("Estou acordado!");
  Serial.println("Acordei!");
  Serial.print("Contador: ");
  delay(5000);
  
;
  Serial.println(count);
  
  dormir();
}
