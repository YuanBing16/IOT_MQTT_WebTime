#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Ganti sesuai jaringanmu
const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Pin relay
const int relay1 = D1;  // GPIO5
const int relay2 = D2;  // GPIO4
const int relay3 = D3;  // GPIO0
const int relay4 = D6;  // GPIO12

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi menerima pesan
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima: ");
  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(pesan);

  // Kontrol relay berdasarkan topik
  if (String(topic) == "coba/lampu1") digitalWrite(relay1, pesan == "1" ? LOW : HIGH);
  else if (String(topic) == "coba/lampu2") digitalWrite(relay2, pesan == "1" ? LOW : HIGH);
  else if (String(topic) == "coba/lampu3") digitalWrite(relay3, pesan == "1" ? LOW : HIGH);
  else if (String(topic) == "coba/lampu4") digitalWrite(relay4, pesan == "1" ? LOW : HIGH);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP_" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung!");

      // Subscribe topik untuk semua relay
      client.subscribe("coba/lampu1");
      client.subscribe("coba/lampu2");
      client.subscribe("coba/lampu3");
      client.subscribe("coba/lampu4");

    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Atur pin sebagai output
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  // Matikan semua relay (HIGH untuk relay aktif LOW)
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
