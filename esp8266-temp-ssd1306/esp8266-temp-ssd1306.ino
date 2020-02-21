// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "DHT.h"   //https://github.com/adafruit/DHT-sensor-library 
#include "SSD1306.h" // alias for 
#include <ESP8266WiFi.h>
#include "secrets.h"
#include <PubSubClient.h>

const char* ssid     = STASSID;
IPAddress broker(192,168,100,50);
const char* topic    = "sensor/outtemp";

WiFiClient espClient;
PubSubClient   client(espClient);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5,4);

// SH1106 display(0x3c, 5,4);
// Create the DHT temperature and humidity sensor object 
DHT dht1(0, DHT22); 

float DHT22_t, DHT22_h;
int xpos = 0;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}

void initMQTT() {
  client.setServer(broker, 1883);
  // client.setCallback(callback); // No callback/receiving here
  
  while (!client.connected()) {
    Serial.println("Attempting to connect ....");
    while (!client.connect("ESP8266Client")) {
       Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  dht1.begin();

  initWiFi();
  initMQTT();
  DHT22_t = dht1.readTemperature(); 
  DHT22_h = dht1.readHumidity(); 
     
  Serial.print("DHT22  "); 
  Serial.print(DHT22_t,1); Serial.print(String(char(176))+"C  "); 
  Serial.print(DHT22_h,1); Serial.println("%RH"); 
  Serial.println(); 

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
}

float toFahrenheit(float centigrades) {
  return (centigrades*1.8+32);
}

void drawTempHumiDemo() {
  DHT22_t = dht1.readTemperature(); 
  DHT22_h = dht1.readHumidity(); 
  char buf[15];
  String temp = String(DHT22_t,1);
  temp.toCharArray(buf,15);

  client.publish(topic, buf);
    
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER); // The coordinates define the center of the screen!
  display.drawString(64,0,String(DHT22_t,1)+"°C");
  display.drawString(64,30,String(DHT22_h,1)+"% rh");
  display.setTextAlignment(TEXT_ALIGN_LEFT); // The coordinates define the center of the screen!
  display.drawString(xpos%128,40,"-");
}

void loop() {
  display.clear();  // clear the display
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  drawTempHumiDemo();
  display.display();
  delay(1500);
  xpos = xpos + 2;
}
