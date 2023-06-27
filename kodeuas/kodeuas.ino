#include <virtuabotixRTC.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

virtuabotixRTC myRTC(5, 4, 2);

Servo sg90;

const int servoPin = D5;
bool servo = true;
int feedingHour = 11;     // Jam pemberian makan
int feedingMinute = 23;  // Menit pemberian makan

const char *ssid = "nyambung";                // nama SSID untuk koneksi Anda
const char *password = "bossssss";        // password akses point WIFI Anda
const char *mqtt_server = "35.78.95.245";  // isikan server broker

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;
String macAddr = "";

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(macAddr.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // myRTC.setDS1302Time(0,2,3,7,27,6,2023);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  sg90.attach(servoPin);
  sg90.write(15);
}

void moveServo() {
  sg90.write(15);
  client.publish("esp/servo", "15");
  delay(1500);
  sg90.write(90);
  client.publish("esp/servo", "90");
  delay(1000);
  sg90.write(15);
  client.publish("esp/servo", "15");
  delay(1500);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop()) {
    client.connect(macAddr.c_str());
  }
  myRTC.updateTime();
  String rtcHours = String(myRTC.hours);
  client.publish("esp/rtc", rtcHours.c_str());
  now = millis();
  if (now - lastMeasure > 5000) {
    lastMeasure = now;

    if (myRTC.hours == feedingHour && myRTC.minutes == feedingMinute && servo == true) {
      moveServo();  // Memanggil fungsi untuk memberikan makanan
      delay(6000);  // Memberi jeda waktu 1 menit untuk mencegah pemberian makan berulang
      // servo = false;
    } 
    // else {
    //   servo = true;
    // }
  }
}
