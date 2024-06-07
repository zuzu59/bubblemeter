// zf240528.2347

// Choix de la connexion WIFI, qu'une seule possibilité !
// #define zWifiNormal true
// #define zWifiManager true
#define zWifiAuto true


// WIFI
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"
WiFiClient client;
HTTPClient http;
float rrsiLevel = 0;      // variable to store the RRSI level



void zWifiTrouble(){
  USBSerial.println("\nOn a un problème avec le WIFI !");
  delay(200);
  USBSerial.flush(); 
  // On part en dsleep pour économiser la batterie !
  esp_deep_sleep_start();
    // ESP.restart();
}



#ifdef zWifiAuto
  #include <vector>
  struct WifiCredentials {
    String ssid;
    String password;
  };
  std::vector<WifiCredentials> wifi_creds;

  void connectToBestWifi() {
      // Ajouter vos informations d'identification Wi-Fi qui se trouvent dans secrets.h autant de fois que vous le souhaitez
    WifiCredentials creds1 = {WIFI_SSID1, WIFI_PASSWORD1};
    wifi_creds.push_back(creds1);
    WifiCredentials creds2 = {WIFI_SSID2, WIFI_PASSWORD2};
    wifi_creds.push_back(creds2);
    WifiCredentials creds3 = {WIFI_SSID3, WIFI_PASSWORD3};
    wifi_creds.push_back(creds3);
    WifiCredentials creds4 = {WIFI_SSID4, WIFI_PASSWORD4};
    wifi_creds.push_back(creds4);
    WifiCredentials creds5 = {WIFI_SSID5, WIFI_PASSWORD5};
    wifi_creds.push_back(creds5);

    int best_rssi = -1000;
    String best_ssid;
    String best_password;
    // Scanner les réseaux Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    USBSerial.print("Number SSID scanned: ");
    USBSerial.println(n);
    for (int i = 0; i < n; i++) {
      USBSerial.print("SSID scanned: ");
      USBSerial.println(WiFi.SSID(i));
      USBSerial.print("RSSI: ");
      USBSerial.println(WiFi.RSSI(i));
      for (const auto &cred : wifi_creds) {
        if (WiFi.SSID(i) == cred.ssid) {
          int rssi = WiFi.RSSI(i);
          if (rssi > best_rssi) {
            best_rssi = rssi;
            best_ssid = cred.ssid;
            best_password = cred.password;
          }
        }
      }
    }
    // Se connecter au réseau Wi-Fi avec le meilleur RSSI
    if (!best_ssid.isEmpty()) {
      WiFi.begin(best_ssid.c_str(), best_password.c_str());
      USBSerial.print("Connecting to ");
      USBSerial.println(best_ssid);
      int connAttempts = 0;
      while (WiFi.status() != WL_CONNECTED && connAttempts < 20) {
        delay(500);
        USBSerial.print(".");
        connAttempts++;
      }
      USBSerial.println("");
      if (WiFi.status() == WL_CONNECTED) {
      } else {
        USBSerial.println("Failed to connect");
        zWifiTrouble();
      }
    } else {
      USBSerial.println("No known networks found");
      zWifiTrouble();
    }
  }
#endif


#ifdef zWifiManager
  #include <WiFiManager.h>

  static void ConnectWiFi() {
    WiFi.mode(WIFI_STA);
    USBSerial.println("Connexion en WIFI Manager");
    // si le bouton FLASH de l'esp32-c3 est appuyé dans les 3 secondes après le boot, la config WIFI sera effacée !
    pinMode(buttonPin, INPUT_PULLUP);
    if ( digitalRead(buttonPin) == LOW) {
      WiFiManager wm; wm.resetSettings();
      USBSerial.println("Config WIFI effacée !"); delay(1000);
      ESP.restart();
    }
    WiFiManager wm;
    bool res;
    res = wm.autoConnect("esp32_wifi_config",""); // pas de password pour l'ap esp32_wifi_config
    if(!res) {
      zWifiTrouble();
    }
  }
#endif


#ifdef zWifiNormal
  static void ConnectWiFi() {
  USBSerial.println("Connexion en WIFI Normal avec secrets.h");
  USBSerial.printf("WIFI_SSID: %s\nWIFI_PASSWORD: %s\n", WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // WiFi.setTxPower(WIFI_POWER_8_5dBm);  //c'est pour le Lolin esp32-c3 mini V1 ! https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html
  USBSerial.println("Connecting");
  long zWifiTiemeout = 10000 + millis(); 
  while(WiFi.status() != WL_CONNECTED){
    USBSerial.print("."); delay(100);
    if(millis() > zWifiTiemeout ){
      zWifiTrouble();
    }
  }
}
#endif


// start WIFI
void zStartWifi(){
  digitalWrite(ledPin, HIGH);
  #ifdef zWifiNormal
    ConnectWiFi();
  #endif
  #ifdef zWifiManager
    ConnectWiFi();
  #endif
  #ifdef zWifiAuto
    connectToBestWifi();
  #endif
  USBSerial.println("\nConnecté au réseau WiFi !");
  USBSerial.print("SSID: ");
  USBSerial.println(WiFi.SSID());
  USBSerial.print("RSSI: ");
  USBSerial.println(WiFi.RSSI());
  int txPower = WiFi.getTxPower();
  USBSerial.print("TX power: ");
  USBSerial.println(txPower);  
  USBSerial.print("IP: ");
  USBSerial.println(WiFi.localIP());
  digitalWrite(ledPin, LOW);
}
