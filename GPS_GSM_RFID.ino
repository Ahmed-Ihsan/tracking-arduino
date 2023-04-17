#include <MFRC522.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define GPS_RX_PIN 4
#define GPS_TX_PIN 3

SoftwareSerial SIM800L(7, 8);
SoftwareSerial GPSSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus GPS;
MFRC522 mfrc522(SS_PIN, RST_PIN);

String message ;

void setup() {
  Serial.begin(9600);
  GPSSerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Ready to read RFID cards");
}

void loop() {
  cardRFID();
  GPSDATA();
}

void cardRFID() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID:");
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    content.toUpperCase();
    if (content.substring(1) == "83 E8 EC 93")
    {
      Serial.println("Authorized access");
      Serial.println();
      delay(3000);
    }
    else   {
      Serial.println(" Access denied");
      gsm(message);
      delay(3000);
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

void GPSDATA() {
  while (GPSSerial.available() > 0) {
    if (GPS.encode(GPSSerial.read())) {
      if (GPS.location.isValid()) {
        message = "Latitude: " + String(GPS.location.lat(), 6) + ", Longitude: " + String(GPS.location.lng(), 6);
        Serial.println(message);
      } else {
        Serial.println("No GPS fix");
      }
    }
  }
}

void gsm(String msg){
  SIM800L.begin(9600);
  SIM800L.println("AT+CMGF=1"); // set SMS mode to text
  delay(1000);

  SIM800L.print("AT+CMGS=\"+9647516274515\"\r"); // replace with recipient's phone number
  delay(1000);

  SIM800L.print(msg); // replace with your message
  delay(1000);

  SIM800L.write(0x1A); // send Ctrl+Z to indicate end of message
  delay(1000);
  GPSSerial.begin(9600);
  }
