#include <stdio.h>

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* SSID = "STRAYLIGHT_IOT";
const char* PASSWORD = "";

const int NUM_LEDS = 144 * 6 - 26;

const int MAX_ARGS = 10;
const int FPS = 150;
const float INTERVAL = 1.0 / FPS;

WiFiServer server(8080);
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(NUM_LEDS, 4, NEO_GRB + NEO_KHZ800);

bool flickering = false;
int flicker_r = 255;
int flicker_g = 255;
int flicker_b = 255;

#include "Patterns.h"

void setup() {
  Serial.begin(115200);
  delay(10);

  EEPROM.begin(512);
  flickering = EEPROM.read(0);
  flicker_r = EEPROM.read(1);
  flicker_g = EEPROM.read(2);
  flicker_b = EEPROM.read(3);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.begin();
  Serial.print("\nServer listing at ");
  Serial.println(WiFi.localIP());

  neopixel.begin();
  neopixel.show();
}

void loop() {
  // Wait before server.available() for voltage to stablize.
  delay(1 + INTERVAL * 1000);

  WiFiClient client = server.available();
  if (!client) {
    if (flickering) {
      flicker();
    }
    return;
  }

  // Wait until the client sends some data
  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);

  client.print(F("HTTP/1.1 200 OK\r\n"));
  client.print(F("Content-Type:text/plain\r\n"));
  client.print(F("Connection:close\r\n\r\n"));

  bool success = false;
  if (request.startsWith("GET /")) {
    String path = request.substring(5, request.length() - 9);
    if (processRequest(path)) {
      success = true;
    }
  }

  client.print(success ? F("OK") : F("NG"));
  client.flush();
}

bool processRequest(String req) {
  float argv[MAX_ARGS];
  if (parseArgs(req, "set", argv) == 3) {
    set(argv[0], argv[1], argv[2]);
    if (argv[0] == 0 && argv[1] == 0 && argv[2] == 0) {
      flickering = false;
      EEPROM.write(0, flickering);
      EEPROM.commit();
    }
  } else if (parseArgs(req, "fade", argv) == 7) {
    fade(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], identity);
  } else if (parseArgs(req, "easein", argv) == 7) {
    fade(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], easein);
  } else if (parseArgs(req, "easeout", argv) == 7) {
    fade(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], easeout);
  } else if (parseArgs(req, "pulse", argv) == 6) {
    pulse(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
  } else if (parseArgs(req, "rainbow", argv) == 0) {
    rainbow();
  } else if (parseArgs(req, "rotate", argv) == 5) {
    rotate(argv[0], argv[1], argv[2], argv[3], argv[4]);
  } else if (parseArgs(req, "flicker", argv) == 3) {
    flickering = true;
    flicker_r = argv[0];
    flicker_g = argv[1];
    flicker_b = argv[2];
    EEPROM.write(0, flickering);
    EEPROM.write(1, flicker_r);
    EEPROM.write(2, flicker_g);
    EEPROM.write(3, flicker_b);
    EEPROM.commit();
  } else {
    Serial.println("ERROR: Unknown command.");
    return false;
  }
  return true;
}

int parseArgs(String in, String function_name, float* args) {
  if (!in.startsWith(function_name + "(")) {
    return -1;
  }
  String args_str = in.substring(function_name.length() + 1);
  int start = 0;
  int argc = 0;
  bool found_closing = false;
  while (!found_closing) {
    int end = args_str.indexOf(",", start);
    if (end == -1) {
      end = args_str.indexOf(")", start);
      if (end == -1) {
        return -1;
      }
      found_closing = true;
    }
    if (start != end) {
      args[argc++] = args_str.substring(start, end).toFloat();
    }
    start = end + 1;
  }
  return argc;
}

