#include <Adafruit_NeoPixel.h>

#define SWITCH_PIN 4
#define RELAY_PIN 3
#define NEOPIXEL_PIN 2

const float BPM = 120 * 8;
const float BeatPerMillis = BPM / 60 / 1000;

Adafruit_NeoPixel led = Adafruit_NeoPixel(
  1, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

bool beat[16];

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  led.begin();
}

void loop() {
  unsigned long now = millis();
  unsigned long counter = now * BeatPerMillis;
  bool on_beat = counter % 2 == 0;

  bool should_beat = beat[(counter / 2) % 16] && on_beat;
  digitalWrite(RELAY_PIN, should_beat ? HIGH : LOW);

  if (digitalRead(SWITCH_PIN) && on_beat) {
    beat[(counter / 2) % 16] = true;
  }

  // LED rhythm indication
  int c1 = counter / 8 % 4;
  int c2 = 8 - counter % 8;
  led.setPixelColor(0, led.Color(
    c2 * 10 * (c1 == 0 || c1 == 3 ? 1 : 0),
    c2 * 10 * (c1 == 1 || c1 == 3 ? 1 : 0),
    c2 * 10 * (c1 == 2 || c1 == 3 ? 1 : 0)));
  led.show();
}
