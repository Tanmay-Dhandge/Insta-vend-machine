#include <WiFi.h>
#include <WiFiClientSecure.h>

#define MQTT_MAX_PACKET_SIZE 1024
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ── WiFi + MQTT Credentials ────────────────────────────────
const char* ssid        = "Tanmay";
const char* password    = "tnmy9416";

const char* mqtt_server = "366c887e88bf40c496821b5e646d3a12.s1.eu.hivemq.cloud";
const char* mqtt_user   = "hivemq.webclient.1775540663302";
const char* mqtt_pass   = "7@1wDCrTa0jP<Wn>8yN,";

#define TOPIC "vendbot/order"

// ── GPIO Mapping ───────────────────────────────────────────
#define PIN_A1   23
#define PIN_A2   22
#define PIN_B1   21
#define PIN_B2   19

// ── MQTT Objects ───────────────────────────────────────────
WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastReconnect = 0;

// ── Normalize Slot String ──────────────────────────────────
void normalizeSlot(char* slot) {

  // Convert to lowercase + remove '-' or spaces
  int j = 0;

  for (int i = 0; slot[i]; i++) {
    char c = tolower(slot[i]);

    if (c != ' ' && c != '-') {
      slot[j++] = c;
    }
  }

  slot[j] = '\0';
}

// ── Slot → Pin Mapping ─────────────────────────────────────
int pinForSlot(char* slot) {

  normalizeSlot(slot);

  if (strcmp(slot, "a1") == 0) return PIN_A1;
  if (strcmp(slot, "a2") == 0) return PIN_A2;
  if (strcmp(slot, "b1") == 0) return PIN_B1;
  if (strcmp(slot, "b2") == 0) return PIN_B2;

  Serial.print("[ERROR] Invalid slot: ");
  Serial.println(slot);

  return -1;
}

// ── Timing per Slot (ms) ───────────────────────────────────
int timeForSlot(const char* slot) {

  if (strcmp(slot, "a1") == 0) return 1100;
  if (strcmp(slot, "a2") == 0) return 1200;
  if (strcmp(slot, "b1") == 0) return 1150;
  if (strcmp(slot, "b2") == 0) return 1150;

  return 1100;
}

// ── Dispense Function (Clean & Reusable) ───────────────────
void dispense(int pin, int duration, int qty) {

  for (int i = 0; i < qty; i++) {

    Serial.printf("Dispensing %d/%d\n", i + 1, qty);

    digitalWrite(pin, HIGH);

    unsigned long start = millis();
    while (millis() - start < duration) {
      client.loop(); // keep MQTT alive
    }

    digitalWrite(pin, LOW);

    delay(400); // gap between items
  }
}

// ── MQTT Callback ──────────────────────────────────────────
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.println("\n============================");
  Serial.println("   NEW ORDER RECEIVED");
  Serial.println("============================");

  char json[length + 1];
  memcpy(json, payload, length);
  json[length] = '\0';

  Serial.print("Raw JSON: ");
  Serial.println(json);

  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, json);

  if (err) {
    Serial.print("[ERROR] JSON parse failed: ");
    Serial.println(err.c_str());
    return;
  }

  JsonArray items = doc["items"];

  if (items.isNull() || items.size() == 0) {
    Serial.println("[ERROR] No items found");
    return;
  }

  int index = 0;

  for (JsonObject item : items) {

    index++;

    const char* rawSlot = item["slot"] | "";
    int qty = item["qty"] | 0;

    char slot[10];
    strncpy(slot, rawSlot, sizeof(slot));
    slot[sizeof(slot) - 1] = '\0';

    Serial.printf("\n[Item %d]\n", index);
    Serial.print("Raw Slot : '");
    Serial.print(slot);
    Serial.println("'");

    int pin = pinForSlot(slot);
    int duration = timeForSlot(slot);

    Serial.printf("Normalized Slot : %s\n", slot);
    Serial.printf("Qty             : %d\n", qty);
    Serial.printf("GPIO Pin        : %d\n", pin);
    Serial.printf("Time            : %d ms\n", duration);

    if (pin == -1 || qty <= 0) {
      Serial.println("[SKIP] Invalid data");
      continue;
    }

    Serial.println(">> DISPENSING...");
    dispense(pin, duration, qty);
    Serial.println(">> DONE");
  }

  Serial.println("\n============================");
  Serial.println("   ALL ITEMS DISPENSED");
  Serial.println("============================\n");
}

// ── Reconnect ─────────────────────────────────────────────
bool reconnect() {

  Serial.println("Connecting MQTT...");

  if (client.connect("esp32-vend", mqtt_user, mqtt_pass)) {
    Serial.println("MQTT connected");
    client.subscribe(TOPIC);
    return true;
  }

  Serial.print("Failed, rc=");
  Serial.println(client.state());
  return false;
}

// ── Setup ─────────────────────────────────────────────────
void setup() {

  Serial.begin(115200);

  int pins[] = { PIN_A1, PIN_A2, PIN_B1, PIN_B2 };

  for (int p : pins) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  espClient.setInsecure();

  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);

  client.setBufferSize(1024);
  client.setKeepAlive(60);

  reconnect();
}

// ── Loop ──────────────────────────────────────────────────
void loop() {

  if (!client.connected()) {
    unsigned long now = millis();

    if (now - lastReconnect > 5000) {
      lastReconnect = now;
      reconnect();
    }
  }

  client.loop();
}
