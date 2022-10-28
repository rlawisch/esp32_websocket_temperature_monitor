#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

#define GREEN_LED_PIN 12
#define RED_LED_PIN 13

#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

AsyncWebServer server(80);
AsyncWebSocket ws("/");

const char *ssid = "ESP32_Lawisch";
const char *password = "asdasdasd";

enum state {
  TEMPERATURE,
  LED
};

state current_state = TEMPERATURE;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "May you please give me the current temperature readings?") == 0) {
      if (current_state == TEMPERATURE) { // Check current state
        current_state = LED; // Change current state       
        float temperature = dht.readTemperature(); // Read temperature from DHT
        Serial.printf("DHT reading: %f\n", temperature);
        ws.textAll(String(temperature));
      }
    }
    else if (strcmp((char*)data, "Please, turn on the green LED.") == 0) {
      if (current_state == LED) { // Check current state
        current_state = TEMPERATURE; // Change current state
        Serial.printf("LED: Green\n");
        digitalWrite(RED_LED_PIN, LOW); // Turn off red the LED
        digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green the LED
      }
    }
    else if (strcmp((char*)data, "Please, turn on the red LED.") == 0) {
      if (current_state == LED) { // Check current state
        current_state = TEMPERATURE; // Change current state
        Serial.printf("LED: Red\n");
        digitalWrite(GREEN_LED_PIN, LOW); // Turn off green the LED
        digitalWrite(RED_LED_PIN, HIGH); // Turn on red the LED
      }
    }
    else {
      current_state = TEMPERATURE; // Reset state
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      // Print connected client info
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      // Print disconnecter client info
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      // Handle received data
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void setup() {
  Serial.begin(115200); // Start ESP32 serial communication

  dht.begin(); // Start DHT with initial parameters

  pinMode(GREEN_LED_PIN, OUTPUT); // Set the green LED pin as output
  pinMode(RED_LED_PIN, OUTPUT); // Set the red LED pin as output

  WiFi.softAP(ssid, password); // Start ESP32 Access Point mode
  Serial.println(WiFi.softAPIP()); // Print the websocket server IP to connect

  ws.onEvent(onEvent); // Start listening for events on the websocket server
  server.addHandler(&ws); // Add the websocket server handler to the webserver

  server.begin(); // Start listening for socket connections
}

void loop() {
  ws.cleanupClients();
}
