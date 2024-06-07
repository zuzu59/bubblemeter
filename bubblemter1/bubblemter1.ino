// Petit projet pour mesurer les bulles d'un barboteur lors d'une fermentation de bière
//
// ATTENTION, ce code a été testé sur un esp32-c3. Pas testé sur les autres boards !
//
#define zVERSION        "zf240607.1736"
#define zHOST           "bblmter1"              // ATTENTION, tout en minuscule
#define zDSLEEP         0                       // 0 ou 1 !
#define TIME_TO_SLEEP   120                     // dSleep en secondes 
int zDelay1Interval =   2000;                   // Délais en mili secondes pour la boucle loop

/*
Utilisation:

Astuce:

Installation:

Pour les esp32-c3 super mini, il faut:
 * choisir comme board ESP32C3 Dev Module
 * disabled USB CDC On Boot et utiliser USBSerial. au lieu de Serial. pour la console !
 * changer le schéma de la partition à Minimal SPIFFS (1.9MB APP with OTA/190kB SPIFFS)

Pour le WiFiManager, il faut installer cette lib depuis le lib manager sur Arduino:
https://github.com/tzapu/WiFiManager

Pour le senseur DS18B20 il faut installer ces lib: 
https://github.com/PaulStoffregen/OneWire                             OneWire
https://github.com/milesburton/Arduino-Temperature-Control-Library    DallasTemperature

Pour MQTT, il faut installer la lib (home-assistant-integration):
https://github.com/dawidchyrzynski/arduino-home-assistant

Pour JSON, il faut installer cette lib:
https://github.com/bblanchon/ArduinoJson

Sources:
https://www.reddit.com/r/esp32/comments/1crwakg/built_in_temperature_sensor_on_esp32c3_red_as/?rdt=63263
https://forum.fritzing.org/t/need-esp32-c3-super-mini-board-model/20561
https://www.aliexpress.com/item/1005006005040320.html
https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino
https://dronebotworkshop.com/wifimanager/
https://lastminuteengineers.com/esp32-ota-web-updater-arduino-ide/
https://github.com/dawidchyrzynski/arduino-home-assistant/blob/main/examples/sensor-integer/sensor-integer.ino
https://chat.mistral.ai/    pour toute la partie API REST et wifiAuto ᕗ
*/




// #define DEBUG true
// #undef DEBUG



// General
const int ledPin = 8;               // the number of the LED pin
const int buttonPin = 9;            // the number of the pushbutton pin

const int pulsePin = 0;             // la pin pour le détecteur de proximité (interruption)
int pulsesCounter = 0;              // compteur de pulses


// Sonar Pulse
#include "zSonarpulse.h"


// WIFI
#include "zWifi.h"


// OTA WEB server
#include "otaWebServer.h"


// MQTT
#include "zMqtt.h"


// Temperature sensor
#include "zTemperature.h"

#if zDSLEEP == 1
  // Deep Sleep
  #define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
  // #define TIME_TO_SLEEP  300      /* Time ESP32 will go to sleep (in seconds) */
  RTC_DATA_ATTR int bootCount = 0;
#endif







//variables to keep track of the timing of recent interrupts
unsigned long pulseRebondMillis = 250;  
unsigned long pulseNextMillis = 0; 


void IRAM_ATTR pulseInterrupt() {
  if (millis() > pulseNextMillis){
    ++pulsesCounter;
    pulseNextMillis = millis() + pulseRebondMillis;
    USBSerial.print(pulsesCounter);
    USBSerial.println(", Y'a une bulle !");
  }
}









void setup() {
  // Il faut lire la température tout de suite au début avant que le MCU ne puisse chauffer !
  initDS18B20Sensor();
  delay(200);
  readSensor();

  // Pulse deux fois pour dire que l'on démarre
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); delay(zSonarPulseOn); digitalWrite(ledPin, HIGH); delay(zSonarPulseOff);
  digitalWrite(ledPin, LOW); delay(zSonarPulseOn); digitalWrite(ledPin, HIGH); delay(zSonarPulseOff);
  delay(zSonarPulseWait);

  // Start serial console
  USBSerial.begin(19200);
  USBSerial.setDebugOutput(true);       //pour voir les messages de debug des libs sur la console série !
  delay(3000);                          //le temps de passer sur la Serial Monitor ;-)
  USBSerial.println("\n\n\n\n**************************************\nCa commence !"); USBSerial.println(zHOST ", " zVERSION);

  #if zDSLEEP == 1
    //Increment boot number and print it every reboot
    ++bootCount;
    sensorValue4 = bootCount;
    USBSerial.println("Boot number: " + String(bootCount));
    // Configuration du dsleep
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    USBSerial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  #endif

  // Start WIFI
  zStartWifi();
  sensorValue3 = WiFi.RSSI();

  // Start OTA server
  otaWebServer();

  // Connexion au MQTT
  USBSerial.println("\n\nConnect MQTT !\n");
  ConnectMQTT();

  // go go go
  USBSerial.println("\nC'est parti !\n");

  // Envoie toute la sauce !
  zEnvoieTouteLaSauce();
  USBSerial.println("\nC'est envoyé !\n");

  #if zDSLEEP == 1
    // Partie dsleep. On va dormir !
    USBSerial.println("Going to sleep now");
    delay(200);
    USBSerial.flush(); 
    esp_deep_sleep_start();
    USBSerial.println("This will never be printed");
  #endif

	pinMode(pulsePin, INPUT_PULLUP);
	attachInterrupt(pulsePin, pulseInterrupt, FALLING);
}








void loop() {
  // Envoie toute la sauce !
  zEnvoieTouteLaSauce();

  // Délais non bloquant pour le sonarpulse et l'OTA
  zDelay1(zDelay1Interval);
}


// Envoie toute la sauce !
void zEnvoieTouteLaSauce(){

  // Lit les températures
  // readSensor();

  // Envoie les mesures au MQTT
  // sendSensorMqtt();

  // Graphe sur l'Arduino IDE les courbes des mesures
  // USBSerial.print("sensor1:");
  // USBSerial.print(sensorValue1);
  // USBSerial.print(",tempInternal1:");
  // USBSerial.print(tempInternal1);
  // USBSerial.print(",tempInternal2:");
  // USBSerial.print(tempInternal2);

  // USBSerial.print(",sensor2:");
  // USBSerial.print(sensorValue2);
  // USBSerial.print(",sensor3:");
  // USBSerial.print(sensorValue3);
  // USBSerial.print(",sensor4:");
  // USBSerial.print(sensorValue4);
  // USBSerial.print(",sensor5:");
  // USBSerial.print(sensorValue5);

  // USBSerial.println("");
}


// Délais non bloquant pour le sonarpulse et l'OTA
void zDelay1(long zDelayMili){
  long zDelay1NextMillis = zDelayMili + millis(); 
  while(millis() < zDelay1NextMillis ){
    // OTA loop
    server.handleClient();
    // Un petit coup sonar pulse sur la LED pour dire que tout fonctionne bien
    sonarPulse();
  }
}

