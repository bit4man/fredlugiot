#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <WiFiNINA.h>

#include "secrets.h"

int ledPin = A0;    // LED connected to digital pin 9
char ssid[] = SECRET_SSID;        // your network SSID (name)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

IPAddress broker(192,168,100,50);
const uint16_t port = 1883;
const char topic[] = "sensor/temperature";

void initWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
  }
}
void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);

  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
//  Serial.print("SSID: ");
//  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char data[10];
  int i=0;
  while ((i<10) && (i<length)) {
    data[i]=(char)payload[i];
    i++;
  }
  data[i] = '\0';
  Serial.print(data);
//  for (int i=0;i<length;i++) {
//    Serial.print((char)payload[i]);
//  }
  Serial.println();
  int temperature = atoi(data);
  setMeter(temperature);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(ledPin, OUTPUT);
  initWiFi();

  printCurrentNet();
  printWifiData();

  mqttClient.setServer(broker, port);
  mqttClient.setCallback(callback);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  while (!mqttClient.connected()) {
     if (mqttClient.connect("arduinoClient")) {
        Serial.println("MQTT Connected");
        mqttClient.subscribe(topic);      
     } else {
       Serial.print("MQTT connection failed! Error code = ");
       Serial.println(mqttClient.state());
       Serial.println("Trying again in 10 seconds ....");
       delay(10000);
     }
  }
}

int setMeter(uint8_t strength) {
 // Serial.print("Strength:"); Serial.println(strength);
  analogWrite(ledPin, map(strength, 20, 100, 5, 145));
}

int delta = 1;
int i;
uint16_t cnt=0;

void loop() {
    mqttClient.loop();
//    if (++cnt > 1000) {
//      cnt=0;
//      setMeter(i);
//      i+=delta;
//      if (i==10) delta=-1;
//      if (i==0)  delta=1;      
//    }
}
