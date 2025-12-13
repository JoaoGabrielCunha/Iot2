#include <Matter.h>
#include <GFButton.h>

RTC_DATA_ATTR int count = 0; // agora persiste entre sleeps

RTC_DATA_ATTR int pinoParaAcordar = 8;
RTC_DATA_ATTR GFButton sensor(8);

RTC_DATA_ATTR unsigned long instanteAnterior = 0;

void movimento(GFButton &sensor)
{
  Serial.println("Movimento detectado!");
}

void inercia(GFButton &sensor)
{
  Serial.println("Inércia detectada!");
}

void dormir()
{
  Serial.println("Fui dormir!");
  count++;
  Serial.println(count);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  esp_deep_sleep_start();
}

void setup()
{
  Serial.begin(115200);
  delay(500);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 255);


  // ou... acorda quando tiver HIGH no pino desejado
  pinMode(pinoParaAcordar, INPUT);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)pinoParaAcordar, HIGH);

  sensor.setPressHandler(inercia);
  sensor.setReleaseHandler(movimento);

  esp_sleep_enable_timer_wakeup(10e6);
}

void loop()
{
 

  unsigned long instanteAtual = millis();
  if (instanteAtual > instanteAnterior + 10000)
  {
    Serial.println("Estou acordado!");
    Serial.println("Acordei!");
    Serial.print("Contador: ");
    Serial.println(count);
    instanteAnterior = instanteAtual;
    dormir();
  }

  sensor.process();
}
