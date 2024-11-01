#include <WiFiS3.h>
WiFiClient network;

#include <MQTT.h>
#include <MQTTClient.h>
MQTTClient mqtt = MQTTClient(256);

#include <Key.h>
#include <Keypad.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//กำหนดตัวแปรที่เอาไว้ใช้ในการนับเลขที่เป็นวินาที
unsigned long period = 1000;
unsigned long last_time = 0;

//กำหนดปุ่มกดเพื่อสุ่มตัวเลขและกำหนดสถานะที่เป็นboolean = false
const int bt_random = 2;
bool isBtRandom = false;

//กำหนดปุ่มกดเล่นและกำหนดสถานะที่เป็นboolean = false
const int bt_play = 4;
bool isBtPlay = false;

//กำหนดปุ่มกดหยุดและกำหนดสถานะที่เป็นboolean = false
const int bt_stop = 6;
bool isBtStop = false;

//กำหนดตัวLED
const int red = 10;
const int yellow = 11;
const int green = 12;

//กำหนดสถานะที่เป็นboolean = false
bool red_on = false;
bool yellow_on = false;
bool green_on = false;

int time_count = 1;//ตัวแปรนับวินาที
int playerNum = 0;//ตัวแปรของเลขที่ผู้เล่นกดได้
int theTime = 0;//ตัวแปรของตัวเลขที่สุ่มมาเพื่อไปเช็ค
bool isGreenClick = false;
bool isRedClick = true;

int winStreak = 0;

// MQTT
const char WIFI_SSID[] = "เชื่อมไมวะ";          // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "1234567890";  // CHANGE TO YOUR WIFI PASSWORD

const char MQTT_BROKER_ADRRESS[] = "phycom.it.kmitl.ac.th";  // CHANGE TO MQTT BROKER'S ADDRESS
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "BIGBADBEST";  // CHANGE IT AS YOU DESIRE

const char PUBLISH_TOPIC[] = "Timestop/BIGBADBEST";    // CHANGE IT AS YOU DESIRE

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  randomSeed(analogRead(0));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  connectToMQTT();

  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(7, 1);
  lcd.print("Time Stop");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Black");
  lcd.setCursor(6, 1);
  lcd.print("To Start.");
  lcd.setCursor(0, 0);
}

void loop() {
  if (millis() - last_time >= period) {
    last_time = millis();
    time_count += 1;
  }
  if (digitalRead(bt_random) == 0 /* && !isBtRandom */) {
    closeLED();
    isGreenClick = false;
    isRedClick = true;
    int num = random(1, 10);
    theTime = num;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finish Random!!");
    lcd.setCursor(0, 1);
    lcd.print("Your Target:");
    lcd.setCursor(13, 1);
    lcd.print(num);
    isBtRandom = true;
  }else if (digitalRead(bt_play) == 0 /* && !isBtPlay */){
    time_count = 1;
    yellow_on = !yellow_on;
    checkYellow();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Red If You Sure!");
    lcd.setCursor(0, 1);
    lcd.print("Time is running");
    isGreenClick = true;
    isBtPlay = true;
  }else if (digitalRead(bt_stop) == 0 /* && !isBtStop */ && isGreenClick && isRedClick){
    isRedClick = false;
    playerNum = time_count;
    lcd.clear();
    lcd.setCursor(0, 0);
    yellow_on = !yellow_on;
    checkYellow();
    if(playerNum == theTime){
      green_on = !green_on;
      checkGreen();
      winStreak += 1;
      sendToMQTT();
      lcd.print("You Win !!");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Target:");
      lcd.setCursor(8, 0);
      lcd.print(theTime);
      lcd.setCursor(0, 1);
      lcd.print("You:");
      lcd.setCursor(5, 1);
      lcd.print(playerNum);
    }else{
      red_on = !red_on;
      checkRed();
      winStreak = 0;
      sendToMQTT();
      lcd.print("You Lose ??");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Target:");
      lcd.setCursor(8, 0);
      lcd.print(theTime);
      lcd.setCursor(0, 1);
      lcd.print("You:");
      lcd.setCursor(5, 1);
      lcd.print(playerNum);
    }
    isBtStop = true;
  }
/*   if (digitalRead(bt_random) == 1) {
    isBtRandom = false;
  }
  if (digitalRead(bt_play) == 1) {
    isBtPlay = false;
  }
  if (digitalRead(bt_stop) == 1) {
    isBtStop = false;
  } */
  checkRed();
  checkYellow();
  checkGreen();
}
void checkRed(){
  if (red_on) {
    digitalWrite(red, HIGH);
  } else {
    digitalWrite(red, LOW); // ปิด LED เมื่อ r_on เป็น false
  }
}
void checkYellow(){
  if (yellow_on) {
    digitalWrite(yellow, HIGH);
    delay(150); // ปรับดีเลย์ให้เหมาะสมกับความเร็วการกระพริบ
    digitalWrite(yellow, LOW);
    delay(150); // ปรับดีเลย์ให้เหมาะสมกับความเร็วการกระพริบ
  } else {
    digitalWrite(yellow, LOW); // ปิด LED เมื่อ r_on เป็น false
  }
}
void checkGreen(){
  if (green_on) {
    digitalWrite(green, HIGH);
     delay(100); 
    digitalWrite(green, LOW);
     delay(100); 
    digitalWrite(yellow, HIGH);
    delay(100); // ปรับดีเลย์ให้เหมาะสมกับความเร็วการกระพริบ
    digitalWrite(yellow, LOW);
    delay(100); // ปรับดีเลย์ให้เหมาะสมกับความเร็วการกระพริบ
    digitalWrite(red, HIGH);
    delay(100); // ปรับดีเลย์ให้เหมาะสมกับความเร็วการกระพริบ
    digitalWrite(red, LOW);
    delay(100); // ปรับดีเลย์ให้เหมาะสมกับความเร็วการกระพริบ
  } else {
    digitalWrite(green, LOW); // ปิด LED เมื่อ r_on เป็น false
  }
}
void closeLED(){
    green_on = false;
    yellow_on = false;
    red_on = false;
    digitalWrite(green, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
  }

void connectToMQTT() {
  // Connect to the MQTT broker
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);

  // Create a handler for incoming messages

  while (!mqtt.connect(MQTT_CLIENT_ID)) {
    delay(100);
  }
}

void sendToMQTT() {
  //int val = analogRead(A0);
  String val_str = String(winStreak);
  // Convert the string to a char array for MQTT publishing

  // Publish the message to the MQTT topic
  mqtt.publish(PUBLISH_TOPIC, val_str);

  // Print debug information to the Serial Monitor in one line
}