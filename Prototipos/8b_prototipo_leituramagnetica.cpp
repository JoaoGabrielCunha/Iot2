#include <Wire.h>
#include <QMC5883LCompass.h>

const int SDA_PIN = 16;   // SDA no GPIO 16
const int SCL_PIN = 15;   // SCL no GPIO 15

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializa I2C nos pinos escolhidos (SDA, SCL)
  Wire.begin(SDA_PIN, SCL_PIN);

  // Inicializa o magnetômetro
  compass.init();

  Serial.println("GY-271 iniciado");
}

void loop() {
  int x, y, z;

  // Faz leitura
  compass.read();
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  Serial.print("X: ");
  Serial.print(x);
  Serial.print("  Y: ");
  Serial.print(y);
  Serial.print("  Z: ");
  Serial.println(z);

  delay(500);
}
