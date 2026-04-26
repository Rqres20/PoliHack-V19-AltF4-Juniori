#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BH1750.h>
#include <TinyGPS.h> 
#include "DHT.h"

// Pini LED-uri Semafor
#define LED_VERDE 31
#define LED_GALBEN 32
#define LED_ROSU 51
#define BUZZER_PIN 11

// Senzori
#define WATER_LVL_PIN A3
#define SOIL_ANALOG A11
#define SOIL_DIGITAL 8
#define LDR_PIN A13
#define GAS_PIN A4
#define TRIG_PIN 10
#define ECHO_PIN 9

DHT dht1(2, DHT11);
DHT dht2(3, DHT11);
Adafruit_MPU6050 mpu;
BH1750 lightMeter;
TinyGPS gps;

void setup() {
  Serial.begin(115200);   
  Serial1.begin(9600);    // Comunicare cu ESP32
  Serial3.begin(9600);    // Comunicare cu Modulul GPS (Pin 14 TX, 15 RX pe Mega)
  
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_GALBEN, OUTPUT);
  pinMode(LED_ROSU, OUTPUT);
  pinMode(SOIL_DIGITAL, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.begin();
  dht1.begin(); 
  dht2.begin(); 
  mpu.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  
  Serial.println("Sistem pornit. Asteptare semnal GPS...");
}

void loop() {
  // LOGICA GPS: Citim caracterele de la satelit in fiecare milisecunda
  while (Serial3.available()) {
    char c = Serial3.read();
    gps.encode(c); 
  }

  static unsigned long lastSend = 0;
  if (millis() - lastSend > 1000) {
    // Citiri Temperaturi si Umiditati
    float t1 = dht1.readTemperature();
    float t2 = dht2.readTemperature();
    float h1 = dht1.readHumidity();
    float h2 = dht2.readHumidity();

    // Calcule Medii
    float tMedie = (isnan(t1) || isnan(t2)) ? 0 : (t1 + t2) / 2.0;
    float hMedie = (isnan(h1) || isnan(h2)) ? 0 : (h1 + h2) / 2.0;
    
    int gas = analogRead(GAS_PIN);
    int water = analogRead(WATER_LVL_PIN);
    
    // Citire senzor sol
    digitalWrite(SOIL_DIGITAL, HIGH); delay(5);
    int soil = analogRead(SOIL_ANALOG);
    digitalWrite(SOIL_DIGITAL, LOW);

    // LOGICA SEMAFOR VREME
    if (gas > 800 || tMedie > 55 || water > 500) {
      digitalWrite(LED_ROSU, HIGH); digitalWrite(LED_GALBEN, LOW); digitalWrite(LED_VERDE, LOW);
    } else if (gas > 500 || tMedie > 40 || water > 200) {
      digitalWrite(LED_ROSU, LOW); digitalWrite(LED_GALBEN, HIGH); digitalWrite(LED_VERDE, LOW);
    } else {
      digitalWrite(LED_ROSU, LOW); digitalWrite(LED_GALBEN, LOW); digitalWrite(LED_VERDE, HIGH);
    }

    // EXTRAGERE COORDONATE LIVE
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);

    // PACHET 13 VALORI CATRE ESP32
    Serial1.print("DATA:");
    Serial1.print(tMedie); Serial1.print(",");      // 0
    Serial1.print(gas); Serial1.print(",");         // 1
    Serial1.print(water); Serial1.print(",");       // 2
    Serial1.print(soil); Serial1.print(",");        // 3
    Serial1.print(analogRead(LDR_PIN)); Serial1.print(","); // 4
    Serial1.print(t1); Serial1.print(",");          // 5
    Serial1.print(t2); Serial1.print(",");          // 6
    Serial1.print(lightMeter.readLightLevel()); Serial1.print(","); // 7
    
    // Verificare validitate coordonate
    if (age == TinyGPS::GPS_INVALID_AGE || age > 5000) {
      Serial1.print("0.000000"); Serial1.print(","); // 8 (Latitudine invalida)
      Serial1.print("0.000000"); Serial1.print(","); // 9 (Longitudine invalida)
    } else {
      Serial1.print(flat, 6); Serial1.print(",");    // 8
      Serial1.print(flon, 6); Serial1.print(",");    // 9
    }
    
    Serial1.print(h1); Serial1.print(",");          // 10
    Serial1.print(h2); Serial1.print(",");          // 11
    Serial1.println(hMedie);                        // 12 (Ultima valoare cu println)

    lastSend = millis();
  }
}