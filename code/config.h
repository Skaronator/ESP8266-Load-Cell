// Required Libarys:
// async-mqtt-client:  https://codeload.github.com/marvinroger/async-mqtt-client/zip/master
// ESPAsyncTCP:        https://codeload.github.com/me-no-dev/ESPAsyncTCP/zip/master
// HX711:              https://codeload.github.com/bogde/HX711/zip/master

// MQTT Broker Config
#define MQTT_HOST IPAddress(192, 168, 178, 5)
#define MQTT_PORT 1883
#define MQTT_USER "homeassistant"
#define MQTT_PASS "your_passwd"
#define MQTT_RECONNECT_TIME 2 // in seconds

// MQTT Client Config
#define MQTT_CLIENT_ID "/room/<room_name>/bed"
#define MQTT_TOPIC_LOAD MQTT_CLIENT_ID "/load"
#define MQTT_TOPIC_LOAD_QoS 0 // Keep 0 if you don't know what it is doing
#define MQTT_TOPIC_TARE MQTT_CLIENT_ID "/tare"
#define MQTT_TOPIC_TARE_QoS 0 // Keep 0 if you don't know what it is doing

// WiFi Config
#define WIFI_SSID "YourWiFi_SSID"
#define WIFI_PASSWORD "YourWiFi_PW"
#define WIFI_CLIENT_ID "BED-210"
#define WIFI_RECONNECT_TIME 2 // in seconds

// Wifi optional static ip (leave client ip empty to disable)
#define WIFI_STATIC_IP    0 // set to 1 to enable static ip
#define WIFI_CLIENT_IP    IPAddress(192, 168, 178, 210)
#define WIFI_GATEWAY_IP   IPAddress(192, 168, 178, 1)
#define WIFI_SUBNET_IP    IPAddress(255, 255, 255, 0)
#define WIFI_DNS_IP       IPAddress(192, 168, 178, 4)

#define OTA_PATCH 1 // Set to 0 if you don't want to update your code over-the-air
#define OTA_PASS "set_ota_password"

// HX711 Config
#define PIN_DOUT D3
#define PIN_CLK  D4

// Get Data:
#define CALIBRATION   -11000 // ADC bits to Kg conversion factor | modify this value if your reading are way off. Different cable length result in different values.
#define SAMPLE_PERIOD 100    // sample period in ms
#define NUM_SAMPLES   5      // number of samples - max 255 - published mqtt update every NUM_SAMPLES * SAMPLE_PERIOD = ms

// Send Data:
#define RESOLUTION    1      // number of characters after the decimal - 0, 1 or 2 is supported.

// Save the Tare to EEPROM.
// Set 0 to disable. It'll tare once the device has started.
// Set 1 to save the tare offset to EEPROM - it'll be saved while restarting/power off
#define SAVE_TARE      1
#define EEPROM_ADDRESS 0 // You can change this adress after a while to reduce the wear on the EEPROM
