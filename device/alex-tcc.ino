#include <ESP8266WiFiMulti.h>
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define PN532_SS (2)
#define BUTTON D0
#define HTTP_CODE_OK 200

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const char *SSID = "";
const char *PASSWORD = "";

const char *BASE_URL = "";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;

void initSerial();
void initWiFi();
void initNFC();
void initDisplay();
void accessControl();
void printDisplay();
void printLCDdone();

ESP8266WiFiMulti WiFiMulti;
HTTPClient https;
Adafruit_PN532 nfc(PN532_SS);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, 0);
NTPClient timeClientLCD(ntpUDP, ntpServer, gmtOffset_sec);

bool isIn = true;

void setup()
{
  pinMode(BUTTON, INPUT);
  initSerial();
  initWiFi();
  initNFC();
  timeClient.begin();
  timeClientLCD.begin();
  initDisplay();
}

void loop()
{
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;
  bool httpResponse;
  int httpCode;

  timeClient.update();
  timeClientLCD.update();
  accessControl();
  printDisplay();
  delay(50);

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 200))
  {

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
      httpRequest(cardid);
      printLCDdone(cardid);
      delay(3000);
    }
  }
}

void initSerial()
{
  Serial.begin(115200);
}

void initWiFi()
{
  delay(1000);
  Serial.println("Connecting in: " + String(SSID));

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);

  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    Serial.println();
    Serial.print("Connected in " + String(SSID) + " | IP => ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Unable to connect");
  }
}

void initNFC()
{
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    while (1)
      ;
  }
  nfc.SAMConfig();
}

void initDisplay()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

void accessControl()
{
  if (digitalRead(BUTTON) == HIGH)
  {
    isIn = !isIn;
  }
}

void printDisplay()
{
  String in = (isIn) ? "IN" : "OUT";
  String msg = String("-------" + in + "-------");
  lcd.setCursor(0, 0);
  lcd.print(timeClientLCD.getFullFormattedTime());
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

void printLCDdone(uint32_t cardid)
{
  String id = String(cardid);
  String msg = String("-------OK!------");
  lcd.setCursor(0, 0);
  lcd.print("RFID: " + id);
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

void httpRequest(uint32_t cardid)
{
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  String id = String(cardid);
  String in = (isIn) ? "true" : "false";
  String timestamp = timeClient.getFullFormattedTime();
  timestamp.setCharAt(10, 'T');
  timestamp = timestamp + "Z";
  Serial.println(timestamp);

  if (https.begin(*client, BASE_URL))
  {

    https.addHeader("content-type", "application/json");

    String bodyJSON = String("{\"rfid\": \"" + id + "\", \"timestamp\": \"" + timestamp + "\", \"isIn\":" + in + "}");
    Serial.println("Registering...");
    Serial.println(bodyJSON);
    int httpCode = https.POST(bodyJSON);

    if (httpCode < 0)
    {
      Serial.println("request error - " + httpCode);
    }

    if (httpCode != HTTP_CODE_OK)
    {
      Serial.println(httpCode);
    }

    Serial.println("Done!");

    https.end();
  }
}
