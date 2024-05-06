#include <Wire.h>

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    Serial.println("\nI2C Scanner");
}

void loop()
{

    Serial.print("Starting transmission...");

    Wire.beginTransmission(0x01);

    Wire.write(0x02);
    Wire.write(0x03);
    Wire.write(0x04);

    Wire.endTransmission();

    Serial.println(" done\n");

    delay(1000);
}
