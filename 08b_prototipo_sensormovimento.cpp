#include <Matter.h>
#include <GFButton.h>

GFButton sensor(8);

void movimento (GFButton& sensor) {
Serial.println("Movimento detectado!");
}
void inercia (GFButton& sensor) {
Serial.println("Inércia detectada!");
}

void setup() 
{

Serial.begin(115200); delay(500);
sensor.setPressHandler(inercia);
sensor.setReleaseHandler(movimento);


}

void loop() 
{
sensor.process();
}
