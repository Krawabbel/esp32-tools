#include <Wire.h>
#include <WiFi.h>

#include "i2c-recorder.hpp"
#include "i2c-decoder.hpp"
#include "wifi_access.hpp"

constexpr uint8_t pin_scl = 22;
constexpr uint8_t pin_sda = 21;

volatile Recorder recorder;
WiFiClient client;

void scl_interrupt_on_rise()
{
    byte sda = digitalRead(pin_sda);
    const Signal signal = sda == HIGH ? Signal::BIT_HIGH : Signal::BIT_LOW;
    recorder.write(signal);
}

void sda_interrupt_on_change()
{
    const byte scl = digitalRead(pin_scl);
    if (scl == HIGH)
    {
        const byte sda = digitalRead(pin_sda);
        const Signal signal = sda == HIGH ? Signal::STOP : Signal::START;
        recorder.write(signal);
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

void setup()
{
    Serial.begin(9600);
    connect_wifi();

    pinMode(pin_scl, INPUT_PULLUP);
    pinMode(pin_sda, INPUT_PULLUP);
}

void listen()
{
    recorder.reset();

    connect_server();
    client.print("Recording");

    attachInterrupt(digitalPinToInterrupt(pin_scl), scl_interrupt_on_rise, RISING);
    attachInterrupt(digitalPinToInterrupt(pin_sda), sda_interrupt_on_change, CHANGE);

    while (recorder.is_writable())
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
    client.printf("\n*** Replay ***\n");
    connect_server();
    client.printf("\n%s\n", recorder.snippet(0, recorder.length()).c_str());
}

void decode()
{
    client.println("\n*** Analysis ***\n");

    Decoder decoder(recorder);

    for (int frame = 0; !decoder.eof(); frame++)
    {
        const Message msg = decoder.next();
        connect_server();

        const byte addr = msg.data >> 1;
        const byte rw = msg.data & 1;

        client.printf("Frame %3i: %-50s >> %-10s [%i+1 bit(s)] 0x%02X (0x%02X %s) %-5s\n", frame, msg.raw_snippet.c_str(), msg.is_valid ? "OK" : "CORRUPT", msg.n_bits, msg.data, addr, rw > 0 ? "R" : "W", msg.ack ? "ACK" : "NACK");
    }
    client.println();
}

void loop()
{
    listen();
    replay();
    decode();
}
