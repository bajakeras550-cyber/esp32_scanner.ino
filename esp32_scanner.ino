#include <WiFi.h>
#include <WebServer.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>

WebServer server(80);

NimBLEScan* bleScan;


// ===============================
// Escape JSON
// ===============================

String jsonEscape(String text) {

  String result = "";

  for (int i = 0; i < text.length(); i++) {

    char c = text[i];

    if (c == '"' || c == '\\') {
      result += '\\';
    }

    if (c == '\n') {
      result += "\\n";
    }
    else {
      result += c;
    }

  }

  return result;
}


// ===============================
// SCAN
// ===============================

void scanDevices() {

  // -------------------------------
  // WiFi scan
  // -------------------------------

  int wifiCount =
      WiFi.scanNetworks(
          false,
          true
      );


  String json =
      "{\"wifi\":[";


  for (
      int i = 0;
      i < wifiCount;
      i++
  ) {

    if (i > 0)
      json += ",";


    String ssid =
        WiFi.SSID(i);


    int rssi =
        WiFi.RSSI(i);


    int channel =
        WiFi.channel(i);


    String bssid =
        WiFi.BSSIDstr(i);


    bool secure =
        WiFi.encryptionType(i)
        != WIFI_AUTH_OPEN;


    json += "{";


    json +=
        "\"ssid\":\"" +
        jsonEscape(ssid) +
        "\",";


    json +=
        "\"rssi\":" +
        String(rssi) +
        ",";


    json +=
        "\"channel\":" +
        String(channel) +
        ",";


    json +=
        "\"bssid\":\"" +
        bssid +
        "\",";


    json +=
        "\"secure\":" +
        String(
            secure
            ? "true"
            : "false"
        );


    json += "}";

  }


  WiFi.scanDelete();


  // -------------------------------
  // BLE scan
  // -------------------------------

  json += "],\"ble\":[";


  NimBLEScanResults results =
      bleScan->getResults(
          2500,
          false
      );


  for (
      int i = 0;
      i < results.getCount();
      i++
  ) {

    if (i > 0)
      json += ",";


    const NimBLEAdvertisedDevice*
        device =
        results.getDevice(i);


    String name =
        device->getName().c_str();


    if (name.length() == 0) {
      name = "(tanpa nama)";
    }


    int rssi =
        device->getRSSI();


    String address =
        device->getAddress()
               .toString()
               .c_str();


    json += "{";


    json +=
        "\"name\":\"" +
        jsonEscape(name) +
        "\",";


    json +=
        "\"rssi\":" +
        String(rssi) +
        ",";


    json +=
        "\"address\":\"" +
        address +
        "\"";


    json += "}";

  }


  bleScan->clearResults();


  json += "]}";


  server.send(
      200,
      "application/json",
      json
  );

}


// ===============================
// SETUP
// ===============================

void setup() {

  Serial.begin(115200);


  // WiFi scanner
  WiFi.mode(WIFI_STA);


  WiFi.disconnect();


  delay(500);


  // BLE scanner
  NimBLEDevice::init("");


  bleScan =
      NimBLEDevice::getScan();


  bleScan->setActiveScan(true);


  bleScan->setInterval(45);


  bleScan->setWindow(30);


  // ESP32 membuat WiFi sendiri
  WiFi.softAP(
      "ESP32-2G4-Scanner",
      "12345678"
  );


  // API
  server.on(
      "/api/scan",
      HTTP_GET,
      scanDevices
  );


  server.begin();


  Serial.println();
  Serial.println(
      "================================"
  );

  Serial.println(
      "ESP32 2.4 GHz Scanner"
  );

  Serial.println(
      "WiFi: ESP32-2G4-Scanner"
  );

  Serial.println(
      "Password: 12345678"
  );

  Serial.println(
      "IP: 192.168.4.1"
  );

  Serial.println(
      "================================"
  );

}


// ===============================
// LOOP
// ===============================

void loop() {

  server.handleClient();

}
