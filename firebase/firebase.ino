#include <FirebaseESP32.h>
#include <WiFi.h>
#include "DHT.h"
#include "EasyNextionLibrary.h"  // Library Nextion Display

#define FIREBASE_HOST "https://monitoringcontrolling-1833b-default-rtdb.firebaseio.com/"
#define WIFI_SSID "BBP MEKTAN"   // Change the name of your WIFI
#define WIFI_PASSWORD "1ndoJarwo"  // Change the password of your WIFI
#define FIREBASE_Authorization_key "njNf49EFU3oxAQlOBwNtlv3M8eAnkEf6a9NQxIj2"

#define LDRPIN 34
#define SOILPIN 35
#define DHTPIN 32

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Relay
int S1 = 5;   // WATER PUMP
int S2 = 17;  // LAMPU

String Switch1, Switch2;

float humUdara, tempUdara, humTanah;
int ldr;

EasyNex myNex(Serial);

FirebaseData firebaseData;

void setup() {
  Serial.begin(9600);
  myNex.begin(9600);
  dht.begin();
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_Authorization_key);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop() {
  readSensor();
  switchBtn();
  nextSensor();

  Serial.print("Temperature: " + (String)tempUdara + " °C");
  Serial.print("Humidity: " + (String)humUdara + " %");
  Serial.print("Intensitas Cahaya : " + (String)ldr + " Cd");
  Serial.print("Kelembaban Tanah : " + (String)humTanah + " %");
  Serial.println();

  Firebase.setFloat(firebaseData, "/ESP32_APP/TEMPUDARA", tempUdara);
  Firebase.setFloat(firebaseData, "/ESP32_APP/HUMUDARA", humUdara);
  Firebase.setFloat(firebaseData, "/ESP32_APP/INTENSITY", ldr);
  Firebase.setFloat(firebaseData, "/ESP32_APP/HUMTANAH", humTanah);
  delay(200);

  myNex.NextionListen();
}

void switchBtn() {
  if (Firebase.getString(firebaseData, "/ESP32_APP/S1")) {
    Switch1 = firebaseData.stringData();
  }
  if (Firebase.getString(firebaseData, "/ESP32_APP/S2")) {
    Switch2 = firebaseData.stringData();
  }

  if (Switch1 == "1") {
    digitalWrite(S1, LOW);
  } else {
    digitalWrite(S1, HIGH);
  }
  if (Switch2 == "1") {
    digitalWrite(S2, LOW);
  } else {
    digitalWrite(S2, HIGH);
  }
}

void readSensor() {
  humUdara = dht.readHumidity();
  tempUdara = dht.readTemperature();
  humTanah = analogRead(SOILPIN);
  humTanah = map(humTanah, 0, 4095, 100, 0);
  ldr = analogRead(LDRPIN);
}

void nextSensor() {
  String tempUdaraString = String(tempUdara, 1);
  myNex.writeStr("t1.txt", tempUdaraString);
  int tempUdaraInt = tempUdara * 10;
  myNex.writeNum("x1.val", tempUdaraInt);

  String humUdaraString = String(humUdara, 1);
  myNex.writeStr("t2.txt", humUdaraString);
  int humUdaraInt = humUdara * 10;
  myNex.writeNum("x2.val", humUdaraInt);

  String ldrString = String(ldr, 1);
  myNex.writeStr("t0.txt", ldrString);
  int ldrInt = ldr * 10;
  myNex.writeNum("x0.val", ldrInt);

  String humTanahString = String(humTanah, 1);
  myNex.writeStr("t3.txt", humTanahString);
  int humTanahInt = humTanah * 10;
  myNex.writeNum("x3.val", humTanahInt);
}
