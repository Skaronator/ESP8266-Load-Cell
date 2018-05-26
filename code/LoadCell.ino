// Required Libarys:
// async-mqtt-client:  https://codeload.github.com/marvinroger/async-mqtt-client/zip/master
// ESPAsyncTCP:        https://codeload.github.com/me-no-dev/ESPAsyncTCP/zip/master
// HX711:              https://codeload.github.com/bogde/HX711/zip/master

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <HX711.h>
#include <ArduinoOTA.h>
#include "config.h"


HX711 scale(PIN_DOUT, PIN_CLK);

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

bool firstRun = true;
unsigned char samples;
int sum, average, offset;
char oldResult[10];

void eeWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}


void connectToWifi() {
  Serial.printf("[WiFi] Connecting to %s...\n", WIFI_SSID);

  WiFi.hostname(WIFI_CLIENT_ID);
  WiFi.mode(WIFI_STA);
  if (WIFI_STATIC_IP != 0) {
    WiFi.config(WIFI_CLIENT_IP, WIFI_GATEWAY_IP, WIFI_SUBNET_IP, WIFI_DNS_IP);
  }

  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.print("[WiFi] Connected, IP address: ");
  Serial.println(WiFi.localIP());
  wifiReconnectTimer.detach();
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("[WiFi] Disconnected from Wi-Fi!");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(WIFI_RECONNECT_TIME, connectToWifi);
}

void connectToMqtt() {
  Serial.println("[MQTT] Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("[MQTT] Connected to MQTT!");

  mqttReconnectTimer.detach();

  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(MQTT_TOPIC_TARE, MQTT_TOPIC_TARE_QoS);
  Serial.print("Subscribing to ");
  Serial.println(MQTT_TOPIC_TARE);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("[MQTT] Disconnected from MQTT!");

  if (WiFi.isConnected()) {
    Serial.println("[MQTT] Trying to reconnect...");
    mqttReconnectTimer.once(MQTT_RECONNECT_TIME, connectToMqtt);
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  if (!strcmp(topic, MQTT_TOPIC_TARE)) {
    Serial.print("Zeroing: ");
    Serial.println(average / (float) 100);
    offset = average;
    if (SAVE_TARE != 0) {
      Serial.print("Saving to EEPROM...");
      eeWriteInt(EEPROM_ADDRESS, offset);
    }
  }
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  Serial.println("Startup!");

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  if (MQTT_USER != "") {
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
  }

  if (SAVE_TARE == 1) {
    offset = eeGetInt(EEPROM_ADDRESS);

    Serial.print("Loading offset from EEPROM: ");
    Serial.println(offset / (float) 100);
  }

  scale.set_scale(CALIBRATION);

  if (OTA_PATCH == 1) {
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

    ArduinoOTA.setHostname(WIFI_CLIENT_ID);
    ArduinoOTA.setPassword(OTA_PASS);


    ArduinoOTA.begin();
  }

  connectToWifi();
}

void loop() {
  if (WiFi.isConnected() && (OTA_PATCH != 0)) {
    ArduinoOTA.handle();
  }

  sum += scale.get_units() * 100;
  samples++;

  if (samples >= NUM_SAMPLES) {
    if (firstRun) {
      firstRun = false;
      if (SAVE_TARE == 0) {
        average = offset;
        return;
      }
    }

    char result[10];
    average = sum / samples;
    dtostrf(((average-offset) / (float) 100), 5, RESOLUTION, result);

    if (mqttClient.connected() && strcmp(result, oldResult)) {
      mqttClient.publish(MQTT_TOPIC_LOAD, MQTT_TOPIC_LOAD_QoS, true, result);
      Serial.print("Pushing new result:");
      Serial.println(result);
    }

    strncpy(oldResult, result, 10);
    samples = 0;
    sum = 0;
  }

  delay(SAMPLE_PERIOD);
}
