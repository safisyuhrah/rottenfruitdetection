#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========== WiFi Config ==========
const char* ssid = "A1-12-3";
const char* password = "kaf751123ka";

// ========== MQTT Config ==========
const char* mqtt_server = "test.mosquitto.org"; 
const char* mqtt_topic_temp = "fruit_data/temperature";
const char* mqtt_topic_humidity = "fruit_data/humidity";
const char* mqtt_topic_gas = "fruit_data/gas";

WiFiClient espClient;
PubSubClient client(espClient);

// ========== Sensor Config ==========
#define DHTPIN D5
#define DHTTYPE DHT22
#define MQ4_ANALOG A0
#define MQ4_DIGITAL D4

DHT dht(DHTPIN, DHTTYPE);

// ========== Function to Get Averaged Gas Reading ==========
int getAverageGasValue(int samples = 10) {
  int total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(MQ4_ANALOG);
    delay(20);
  }
  return total / samples;
}

// ========== Setup ==========
void setup() {
  Serial.begin(9600);
  delay(10);

  dht.begin();
  pinMode(MQ4_DIGITAL, INPUT);

  Serial.println();
  Serial.println("📶 Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  client.setServer(mqtt_server, 1883);

  Serial.println("🔥 Warming up MQ-4 sensor...");
  delay(30000);
  Serial.println("✅ MQ-4 ready.");

  reconnect();
}

// ========== WiFi Reconnect ==========
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\n✅ WiFi reconnected.");
  }
}

// ========== MQTT Reconnect ==========
void reconnect() {
  while (!client.connected()) {
    Serial.print("🔄 Connecting to MQTT...");
    String clientId = "ESP8266Client-" + String(random(1000, 9999));
    if (client.connect(clientId.c_str())) {
      Serial.println("✅ Connected to MQTT broker");
    } else {
      Serial.print("❌ Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// ========== Main Loop ==========
void loop() {
  checkWiFiConnection();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sendSensorData();
  delay(3000);  // Send data every 3 seconds
}

// ========== Publish Sensor Data ==========
void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gasAnalogValue = getAverageGasValue();
  int gasDigitalValue = digitalRead(MQ4_DIGITAL);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Failed to read from DHT22!");
    return;
  }

  Serial.println("===== Sensor Readings =====");
  Serial.print("🌡 Temperature: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("💧 Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("🟠 Gas (Analog Avg): "); Serial.println(gasAnalogValue);
  Serial.print("🔴 Gas Detected (Digital): ");
  Serial.println(gasDigitalValue == HIGH ? "Yes" : "No");
  Serial.println("============================");

  // Publish with retain flag = true
  client.publish(mqtt_topic_temp, String(temperature).c_str(), true);
  client.publish(mqtt_topic_humidity, String(humidity).c_str(), true);
  client.publish(mqtt_topic_gas, String(gasAnalogValue).c_str(), true);
}