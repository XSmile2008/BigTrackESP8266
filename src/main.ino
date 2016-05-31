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

    uint32_t mill = millis();
    int sec = mill / 1000 % 60;
  	int min = mill / 60000 % 60;
  	int hr = mill / 3600000 % 60;
    mill %= 1000;

    char html[] = "<html>\
    <head>\
      <meta http-equiv='refresh' content='1'/>\
      <title>ESP8266 Demo</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>Hello from ESP8266!</h1>\
      <p>Uptime: %02d:%02d:%02d.%03d</p>\
      IP = \
      Free memory = %lu\
      Reset reason - \
    </body>\
    </html>";

    char temp[sizeof(html) + 100];
    snprintf ( temp, sizeof(temp), html,
      hr, min, sec, mill, ESP.getFreeHeap());

    webServer.send(200, "text/html", temp);
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

  //TODO: Rewrite this
  if (client && client.connected()){
    while(client.available()) {
      Serial.write(client.read());
    }
    while(Serial.available()) {
      size_t len = Serial.available();
      uint8_t sbuf[len];
      Serial.readBytes(sbuf, len);
      client.write(sbuf, len);
      // delay(1);
    }
  }
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
