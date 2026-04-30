#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>

const char* ssid = "TBE3162";
const char* password = "f38U175!";

#define CURRENT_VERSION "1.0"
const char* versionURL = "https://raw.githubusercontent.com/YOUR_USERNAME/YOUR_REPO/main/version.json";

void checkForUpdate() {
  WiFiClientSecure client;
  client.setInsecure(); // skip certificate validation (OK for testing)

  HTTPClient http;
  http.begin(client, versionURL);

  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Version file:");
    Serial.println(payload);

    DynamicJsonDocument doc(512);
    deserializeJson(doc, payload);

    String newVersion = doc["version"];
    String firmwareURL = doc["firmware"];

    Serial.print("Current: ");
    Serial.println(CURRENT_VERSION);
    Serial.print("Available: ");
    Serial.println(newVersion);

    if (newVersion != CURRENT_VERSION) {
      Serial.println("Updating firmware...");

      t_httpUpdate_return ret = httpUpdate.update(client, firmwareURL);

      switch (ret) {
        case HTTP_UPDATE_OK:
          Serial.println("Update complete");
          break;

        case HTTP_UPDATE_FAILED:
          Serial.printf("Update failed: %d\n", httpUpdate.getLastError());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("No updates");
          break;
      }
    } else {
      Serial.println("Already up to date");
    }
  } else {
    Serial.println("Failed to fetch version file");
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");

  checkForUpdate();  // run once on boot
}

void loop() {
  // keep empty for now
}