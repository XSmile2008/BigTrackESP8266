#include "WiFiClient.h"
#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "ESP8266WebServer.h"
#include "RGB.h"

const char* ssid = "TP-Link TL740N";
RGB rgb = RGB(D5, D6, D7);

ESP8266WebServer webServer(80);
WiFiServer server(23);
WiFiClient client;

long lastLog;
void log() {
  if (millis() > lastLog + 5000 && client && client.connected()) {
    lastLog = millis();
    client.print("Time = "); client.print(millis());
    client.print("Free heap = "); client.println(ESP.getFreeHeap());
  }
}

void startWebServer() {
  webServer.on("/", []() {
    rgb.blue();
    String page = "hello from esp8266!\n";
    page += "ip = " + WiFi.localIP().toString();
    webServer.send(200, "text/plain", page);
    rgb.off();
  });
  webServer.onNotFound([](){
    webServer.send(404, "Not found");
  });
  webServer.begin();
  Serial.println("HTTP server started");
  MDNS.addService("http", "tcp", 80);
}

void startTCPServer() {
  server.begin();
  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
  MDNS.addService("telnet", "tcp", 23);
}

bool connect() {
  WiFi.begin(ssid);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 10) {
    delay(900);
    rgb.blue();
    delay(100);
    rgb.off();
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nConnected to "); Serial.println(ssid);
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
    if (!MDNS.begin("esp8266")) {
      Serial.println("Error setting up MDNS responder!");
      return false;
    }
    Serial.println("mDNS responder started");
    rgb.green();
    delay(500);
    rgb.off();
    return true;
  } else {
    rgb.red();
    delay(500);
    rgb.off();
    Serial.print("Can't connect to "); Serial.println(ssid);
    return false;
  }
}

void handleClient() {
  if (server.hasClient()) {
    if (!client || !client.connected()) {//If new client avalable
      if (client) client.stop();
      client = server.available();
      Serial.print("New client: "); Serial.println(client.remoteIP().toString());
    } else {//If already have client
      server.available().stop();
    }
  }

  if (client && client.connected()){
    while(client.available()) {
      Serial.write(client.read());
    }
    while(Serial.available()) {
      size_t len = Serial.available();
      uint8_t sbuf[len];
      Serial.readBytes(sbuf, len);
      client.write(sbuf, len);
      delay(1);
    }
  }

  // if (client && client.connected()){
  //   while(client.available()) {
  //     String s = client.readStringUntil('\n');
  //     if (s.indexOf("nyan") != -1) {
  //       Serial.println(s);
  //       client.println("punyan");
  //       rgb.green();
  //       delay(100);
  //       rgb.off();
  //     } else {
  //       Serial.println(s);
  //       client.println("not find");
  //       rgb.red();
  //       delay(100);
  //       rgb.off();
  //     }
  //   }
  //   while(Serial.available()) {
  //     size_t len = Serial.available();
  //     uint8_t sbuf[len];
  //     Serial.readBytes(sbuf, len);
  //     client.write(sbuf, len);
  //     delay(1);
  //   }
  // }
}

void setup() {
  Serial.begin(115200);
  while (!connect());//TODO: may be move in loop for handle connection lost
  startWebServer();
  startTCPServer();
}

void loop() {
  webServer.handleClient();
  handleClient();
  // log();
}
