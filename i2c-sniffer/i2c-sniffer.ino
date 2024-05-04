#include <Wire.h>
#include <WiFi.h>

#include "wifi_access.h"

void setup()
{
    Wire.begin();
    Serial.begin(9600);

    WiFi.begin(SSID, PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("...");
    }
    Serial.print("WiFi connected with IP:");
    Serial.println(WiFi.localIP());
}

void loop()
{

    WiFiClient client;
    if (!client.connect("go.local", 8001))
    {
        Serial.println("Connection to host failed");
        return;
    }

    client.println("Scanning...");
    for (uint16_t addr = 0U; addr < (1U << 7); ++addr)
    {
        Wire.beginTransmission(addr);
        const uint8_t error = Wire.endTransmission();
        switch (error)
        {
        case 0:
            client.printf("device found at address %#02x\n", addr);
            break;
        case 4:
            client.printf("unknown error at address %#02x\n", addr);
            break;
        default:
            break;
        }
    }

    client.stop();

    for (int i = 5; i > 0; --i)
    {
        client.printf("Restarting in %is...\n", i);
        delay(1000);
    }
}
