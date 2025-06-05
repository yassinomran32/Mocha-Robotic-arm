#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678";

WiFiServer server(1234);
WiFiClient client;

Servo servos[6];
int current[6] = {90, 90, 90, 90, 0, 60};
int target[6] = {90, 90, 90, 90, 0, 60};

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  server.begin();

  servos[0].attach(2);
  servos[1].attach(4);
  servos[2].attach(5);
  servos[3].attach(13);
  servos[4].attach(18);
  servos[5].attach(21);

  for (int i = 0; i < 6; i++) {
    servos[i].write(current[i]);
  }
}

void loop() {
  if (!client || !client.connected()) {
    client = server.available();
    return;
  }

  if (client.available()) {
    String line = client.readStringUntil('\n');
    parseTheta(line);
  }

  bool allReached = false;

  while (!allReached) {
    allReached = true;

    for (int i = 0; i < 5; i++) {
      if (current[i] != target[i]) {
        current[i] += (current[i] < target[i]) ? 1 : -1;
        servos[i].write(current[i]);
        allReached = false;
      }
    }
    delay(20);
  }
  
  if (current[5] != target[5]) {
    current[5] = target[5];
    servos[5].write(current[5]);
  }
  delay(10);
}

void parseTheta(String data) {
  int index = 0;
  int lastComma = -1;

  for (int i = 0; i < 6; i++) {
    int nextComma = data.indexOf(',', lastComma + 1);
    if (nextComma == -1 && i < 5) break;

    String valStr = data.substring(lastComma + 1, (nextComma == -1) ? data.length() : nextComma);
    target[i] = constrain(valStr.toInt(), 0, 180);
    lastComma = nextComma;
  }

  Serial.print("Received: ");
  Serial.println(data);
}
