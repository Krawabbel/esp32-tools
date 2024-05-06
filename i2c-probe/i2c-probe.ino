#include <Wire.h>
#include <WiFi.h>

#include "wifi_access.hpp"

WiFiClient client;

void probe()
{
    client.println("Probing...");
    for (uint16_t addr = 0U; addr < (1U << 7); ++addr)
    {
        Wire.beginTransmission(addr);
        const uint8_t error = Wire.endTransmission();
        switch (error)
        {
        case 0:
            client.printf(" > device found at address %#02x\n", addr);
            break;
        case 4:
            client.printf("unknown error at address %#02x\n", addr);
            break;
        default:
            break;
        }
    }
}

bool check_client()
{
    if (!client.connected())
    {
        Serial.print("(re-)connecting to database server");
        while (!client.connect("mu.local", 8000))
        {
            Serial.print(".");
            delay(1000);
        }
        Serial.printf("\nconnected to database server\nIP: ");
        Serial.println(client.remoteIP());
    }
    return true;
}

void connect_wifi()
{
    Serial.printf("connecting to wireless network %s", SSID);
    WiFi.begin(SSID, PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.printf("\nconnected to wireless network\nIP:");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    connect_wifi();
    Wire.begin();
}

void loop()
{
    if (check_client())
    {
        probe();
    }
    delay(1000);
}
