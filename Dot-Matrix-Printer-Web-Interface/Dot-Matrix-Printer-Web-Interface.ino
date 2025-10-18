#include "src/iec_interface.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "src/index.h"
#include "wifi_password.h"

AsyncWebServer server(80);
WebSocketsServer webSocket(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length)
{
  switch(type)
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      break;
    }
    case WStype_TEXT:
    {
      String data = String((char*)payload);
      if (data == "led_on") {
        digitalWrite(13, HIGH);
      } else if (data == "led_off") {
        digitalWrite(13, LOW);
      }
      break;
    }
  }
}

float getTemperature() {
  // temporary random data
  float temp_x100 = random(0, 10000);
  return temp_x100 / 100;
}

void setup() {
  Serial.begin(115200);
  iec_init();

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.setHostname("esp32");
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi.");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Serve the HTML from the file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.print("ESP32 Web Server: New request received: ");
    Serial.println("GET /");

    request->send(200, "text/html", webpage);
  });

  // Define route to get the temperature data
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.print("ESP32 Web Server: new request received: ");
    Serial.println("GET /temperature");
    float temperature = getTemperature();
    // Format the temp with 2 decimal places
    String tempStr = String(temperature, 2);
    request->send(200, "text/plain", tempStr);
  });

  // Run the server
  server.begin();
}

uint8_t buffer[1000];
void loop() {
  webSocket.loop();
}
