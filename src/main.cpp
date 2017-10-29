
#include <LedRing.h>
#include <L9110.h>
#include <HC-SR04.h>
#include <Siren.h>

#include <WiFiManager.h>
#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "auth.h"

#define BLYNK_PRINT Serial

#define NEOPIXEL_PIN D7
#define BUZZER_PIN D6
#define TRIGGER_PIN D0
#define ECHO_PIN D5

#define A_IA D3
#define A_IB D4
#define B_IA D2
#define B_IB D1

LedRing ledring(NEOPIXEL_PIN);
HCSR04 sonar(TRIGGER_PIN, ECHO_PIN);
L9110 motor(A_IA, A_IB, B_IA, B_IB);
Siren siren(BUZZER_PIN);

void setup()
{
    EEPROM.begin(512);
    Serial.begin(9600);
    WiFiManager wifiManager;
    //wifiManager.resetSettings(); //Uncomment this to wipe WiFi settings from EEPROM on boot.
    wifiManager.autoConnect("Codie");

    Blynk.config(auth);
}

void loop()
{
    Blynk.run();
}

// Joystick
BLYNK_WRITE(V0)
{
    const int X = param[0].asInt();
    const int Y = param[1].asInt();
    int V = (100 - abs(X)) * (Y / 100.f) + Y;
    int W = (100 - abs(Y)) * (X / 100.f) + X;
    int R = (V + W) / 2;
    int L = (V - W) / 2;
    motor.move(R * 1023, L * 1023);
}

// Battery
BLYNK_READ(V1)
{
    const float voltage = 4.2 * analogRead(A0) / 1023.f;

    Blynk.virtualWrite(1, voltage);
}

// Music Button
BLYNK_WRITE(V2)
{
    const int pinValue = param.asInt();
    if (pinValue > 0) {
        ledring.start();
        if (pinValue == 1)
            siren.start(Siren::WAIL);
        if (pinValue == 2)
            siren.start(Siren::YELP);
        if (pinValue == 3)
            siren.start(Siren::PIER);
    } else {
        siren.stop();
        ledring.stop();
    }
}

// Sonar
BLYNK_READ(V3)
{
    const unsigned int distance = sonar.medianDistance();

    Blynk.virtualWrite(3, distance);
}

// NeoPixel
BLYNK_WRITE(V4)
{
    const int pinValue = param.asInt();
    if (pinValue) {
        ledring.start();
    } else {
        ledring.stop();
    }
}