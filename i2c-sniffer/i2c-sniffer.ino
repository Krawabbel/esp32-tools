#include <Wire.h>
#include <WiFi.h>

#include "i2c_emulator.hpp"
#include "i2c_history.hpp"
#include "wifi_access.hpp"

constexpr uint8_t pin_scl = 22;
constexpr uint8_t pin_sda = 21;

volatile History history;
WiFiClient client;

void scl_interrupt_on_change()
{
    const byte scl = digitalRead(pin_scl);
    const Event event = scl == HIGH ? Event::SCL_RISE : Event::SCL_FALL;
    history.write(event);
}

void sda_interrupt_on_change()
{
    const byte sda = digitalRead(pin_sda);
    const Event event = sda == HIGH ? Event::SDA_RISE : Event::SDA_FALL;
    history.write(event);
}

void connect_server()
{
    if (!client.connected())
    {
        Serial.print("(re-)connecting");
        while (!client.connect("mu.local", 8001))
        {
            Serial.print(".");
            delay(1000);
        }
        Serial.printf("\nconnected to server with IP ");
        Serial.println(client.remoteIP());
    }
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
    Serial.printf("\nconnected to wireless network IP: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(9600);
    connect_wifi();

    pinMode(pin_scl, INPUT_PULLUP);
    pinMode(pin_sda, INPUT_PULLUP);
}

void listen()
{
    connect_server();
    client.println();
    client.print("Sniffing");

    attachInterrupt(digitalPinToInterrupt(pin_scl), scl_interrupt_on_change, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin_sda), sda_interrupt_on_change, CHANGE);

    while (history.is_writable())
    {
        delay(100);
        client.print(".");
    }

    detachInterrupt(digitalPinToInterrupt(pin_scl));
    detachInterrupt(digitalPinToInterrupt(pin_sda));

    client.println(" done");
}

void replay()
{
    Emulator emu(&history);
    for (int frame = 0; emu.next(); frame++)
    {
        connect_server();
        client.printf("\nFrame %i: %30s >> %40s >> %10s %5s %s", frame, emu.event_log().c_str(), emu.signal_log().c_str(), emu.data_log().c_str(), emu.ack_log().c_str(), emu.add_log().c_str());
    }
}

void loop()
{
    history.reset();
    listen();
    replay();
    delay(5000);
}
