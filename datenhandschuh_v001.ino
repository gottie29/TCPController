#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

const int buttonPin1 = D7;
const int buttonPin2 = D6;
const int buttonPin3 = D3;
const int buttonPin4 = D4;

int buttonState1 = 0;
int buttonState2 = 0;
int gyroLastUpdate = 0;

int buttonState3 = 0;
int buttonState4 = 0;
int objectCount = 0;
int currentObj = 0;

const char* ssid = "your ssid";
const char* password = "your pwd";
const char* host = "host ip";
uint16_t port = 4681; // your open port e.g. OpenSpace use 4681

#define ARRAYSIZE 10
String sosyObj[ARRAYSIZE] = {"Earth", "Mercury", "Venus", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "ISS" };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // while (!Serial);
  Serial.println("MPU6050 OLED demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found a MPU-6050 sensor");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  
}

void loop() {

  WiFiClient client;

  if (!client.connect(host, port)) {

        Serial.println("Connection to host failed");
        delay(1000);
        return;
  }



  Serial.println("Connected to server successful!");

    long now = micros();                  // Zeit in Mikrosekunden (µs)
    long dt  = now - gyroLastUpdate;      // Zeitdifferenz in Mikrosekunden (µs) 
    gyroLastUpdate = now;            



    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float winkelX;

    buttonState1 = digitalRead(buttonPin1);
    buttonState2 = digitalRead(buttonPin2);
    buttonState3 = digitalRead(buttonPin3);
    buttonState4 = digitalRead(buttonPin4);
    
    // put your main code here, to run repeatedly:
    display.clearDisplay();
    display.setCursor(0, 0);

    display.print("IP: ");
    display.print(WiFi.localIP());
    display.println(""); 

    if (buttonState1 == HIGH) {
      display.println("Button 1 unpressed");
    } else {
      display.println("Button 1 pressed");
      StaticJsonDocument<200> doc;
      doc["topic"] = 1;
      doc["type"] = "luascript";
      JsonObject payload = doc.createNestedObject("payload");
      //payload["script"] = "openspace.printInfo(\"Hello World\")";
      payload["script"] = "openspace.navigation.addGlobalRoll(5)";
      //serializeJson(doc, client);
    }
    if (buttonState2 == HIGH) {
      display.println("Button 2 unpressed");
    } else {
      display.println("Button 2 pressed");
      StaticJsonDocument<200> doc;
      doc["topic"] = 1;
      doc["type"] = "luascript";
      JsonObject payload = doc.createNestedObject("payload");
      //payload["script"] = "openspace.printInfo(\"Hello World\")";
      payload["script"] = "openspace.navigation.addGlobalRoll(-5)";
      //serializeJson(doc, client);
    }
    if (buttonState3 == LOW) {
        objectCount++;
        if (objectCount == 10) {
            objectCount = 0;
        }
    }

    //if (client.connected())
    //{
    //  client.println(a.acceleration.x);
    //}

    display.println("");
    if (buttonState1 == HIGH) {
      display.print("AccelX:");
      display.print(a.acceleration.x);
      display.println("");
      display.print("AccelY:");
      display.print(a.acceleration.y);
      display.println("");
      display.print("AccelZ:");
      display.print(a.acceleration.z);
      display.println("");
    } else {
      display.print("GyroX:");
      display.print(g.gyro.x);
      display.println("");
      display.print("GyroY:");
      display.print(g.gyro.y);
      display.println("");
      display.print("GyroZ:");
      display.print(g.gyro.z);
      display.println("");
    }

    display.print("Focus: ");
    display.print(sosyObj[objectCount]);
    if (currentObj == objectCount) {
      display.print("***");
    }
    display.println("");

    if (buttonState4 == LOW && currentObj != objectCount) {
      StaticJsonDocument<200> doc;
    
        doc["topic"] = 1;
        doc["type"] = "luascript";
        JsonObject payload = doc.createNestedObject("payload");
        payload["script"] = "openspace.pathnavigation.flyTo(\""+String(sosyObj[objectCount])+"\")";
        serializeJson(doc, client);

        currentObj = objectCount;
    }

    if (buttonState1 == LOW) {
        StaticJsonDocument<200> doc;
        float aX = a.acceleration.x;
        float aY = a.acceleration.y;

        doc["topic"] = 1;
        doc["type"] = "luascript";
        JsonObject payload = doc.createNestedObject("payload");
        payload["script"] = "openspace.navigation.addGlobalRotation("+String(aY)+", "+String(aX)+")";
        serializeJson(doc, client);
    }

    if (buttonState2 == LOW) {
        StaticJsonDocument<200> doc;
        float aX = a.acceleration.x;
        float aY = a.acceleration.y;

        doc["topic"] = 1;
        doc["type"] = "luascript";
        JsonObject payload = doc.createNestedObject("payload");
        payload["script"] = "openspace.navigation.addTruckMovement("+String(aX)+")";
        //payload["script"] = "openspace.navigation.addGlobalRotation("+String(aY)+", "+String(aX)+")";
        serializeJson(doc, client);
    }

    



    display.display();
    delay(500);

    Serial.println("Disconnecting...");
    client.stop();


}
