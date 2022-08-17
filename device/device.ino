#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define PN532_SS   (2)
#define BUTTON D0

LiquidCrystal_I2C lcd(0x3F, 16, 2); //FUNÇÃO DO TIPO "LiquidCrystal_I2C"

const char* SSID = "SSID"; 
const char* PASSWORD = "PASSWORD"; 

String BASE_URL = "http://192.168.15.6:3000/";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;  

void initSerial();
void initWiFi();
void initNFC();
void initLEDs();
void initDisplay();
void ledControl();
void printDisplay();

WiFiClient client;
HTTPClient http;
Adafruit_PN532 nfc(PN532_SS);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);

bool isIn = true;

void setup() {
  initSerial();
  initWiFi();
  initNFC();
  initLEDs();
  timeClient.begin();
  initDisplay();
}

void loop() {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;
    bool httpResponse;

    timeClient.update();
    ledControl();
    printDisplay();
    delay(50);

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 200)) {

      nfc.PrintHex(uid, uidLength);
      
      if (uidLength == 4)
      {
          uint32_t cardid = uid[0];
          cardid <<= 8;
          cardid |= uid[1];
          cardid <<= 8;
          cardid |= uid[2];  
          cardid <<= 8;
          cardid |= uid[3]; 
          httpResponse = httpRequest(cardid);
          delay(3000);
      }
    }
}

bool httpRequest(uint32_t cardid)
{
  http.begin(client, BASE_URL + "sensors");

  http.addHeader("content-type", "application/x-www-form-urlencoded");
  
  String id = String(cardid);
  String in = (isIn) ? "true" : "false";
  
  String body =  String("id=" + id + "&isIn=" + in + "&time=" + timeClient.getFullFormattedTime());
 
  Serial.println("Registering...");
  Serial.println(body);
  int httpCode = http.POST(body);

  if (httpCode < 0) {
     Serial.println("request error - " + httpCode);
     return false;
  }

  if (httpCode != HTTP_CODE_OK) {
    return false;
  }

  String response =  http.getString();
  http.end();
  Serial.println("Done!");
  return true;
}

void initSerial() {
  Serial.begin(115200);
}

void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
}

void initNFC() {
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1);
    }
    nfc.SAMConfig();
}

void initLEDs(){
    pinMode(D0, INPUT);
}

void ledControl(){
  if(digitalRead(D0) == HIGH)
  {
    isIn = !isIn;
  }
}

void initDisplay(){
  lcd.init();   // INICIALIZA O DISPLAY LCD
  lcd.backlight(); // HABILITA O BACKLIGHT (LUZ DE FUNDO) 
  lcd.setCursor(0, 0); //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
}

void printDisplay(){
  String in = (isIn) ? "IN" : "OUT";
  String msg = String("-------" + in + "-------");
  lcd.setCursor(0, 0); 
  lcd.print(timeClient.getFullFormattedTime());
  lcd.setCursor(0, 1);    
  lcd.print(msg);
}
