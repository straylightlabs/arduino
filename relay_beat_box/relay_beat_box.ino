// Switch Beat Box Workshop.

#include <Adafruit_NeoPixel.h>

// I/O pins.
const int SWITCH_PIN = 4;
const int RELAY_PIN = 3;
const int NEOPIXEL_PIN = 2;

// Constants.
const double BPM = 120.0;
const int TICKS_PER_BEAT = 4;
const int BEATS_PER_PHRASE = 4;
const int COLOR_PALETTE[BEATS_PER_PHRASE][3] = {
  {25, 25, 25}, // RGB color for beat 0
  {25, 25, 25}, // RGB color for beat 1
  {25, 25, 25}, // RGB color for beat 2
  {25, 25, 25}, // RGB color for beat 3
};

// Derived constants.
const int MILLIS_PER_BEAT = 1.0 / BPM * 60 * 1000;
const int MILLIS_PER_TICK = MILLIS_PER_BEAT / TICKS_PER_BEAT;
const int TICKS_PER_PHRASE = TICKS_PER_BEAT * BEATS_PER_PHRASE;

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

// beat rotates between [0, NUM_BEATS_PER_PHRASE).
void update_led(int beat) {
  // Linearly decreases from 1.0 to 0.0 during a single beat.
  double energy = 1.0 - millis() % MILLIS_PER_BEAT / (double)MILLIS_PER_BEAT;
  uint32_t color = led.Color(
    energy * COLOR_PALETTE[beat][0],  // red
    energy * COLOR_PALETTE[beat][1],  // green
    energy * COLOR_PALETTE[beat][2]); // blue
  // Specify the first (aka 0-th) NeoPixel color.
  led.setPixelColor(0, color);
  // And send the signal.
  led.show();
}

// Phrase to play in the current cycle.
bool current_phrase[TICKS_PER_PHRASE] = { 0 };
// Phrase to play in the next cycle, which is being recorded currently.
bool next_phrase[TICKS_PER_PHRASE] = { 0 };

void update_phrase() {
  // Copy next_phrase to current_phrase.
  for (int i = 0; i < BEATS_PER_PHRASE; i++) {
    // Check if the beat has some input to copy.
    bool has_input = false;
    for (int j = 0; j < TICKS_PER_BEAT; j++) {
      if (next_phrase[i * TICKS_PER_BEAT + j]) {
        has_input = true;
      }
    }
    for (int j = 0; j < TICKS_PER_BEAT; j++) {
      int k = i * TICKS_PER_BEAT + j;
      // Copy the entire beat if it has some input.
      if (has_input) {
        current_phrase[k] = next_phrase[k];
      }
      // And clear next_phrase for the next recording.
      next_phrase[k] = false;
    }
  }
}

void setup() {
  // Initializes the I/O pins.
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);

  // Initializes the NeoPixels.
  led.begin();
}

// True if next_phrase has some data.
bool has_next_phrase = false;
// True if the switch has been pushed down.
bool is_switch_down = false;

void loop() {
  // Counters that are used to rhym the beat.
  // ticktack | 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
  // tick     | 0  0  1  1  2  2  3  3  4  4  5  5  6  6  7  7  8  8  9
  // can_tick | 1  0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1
  // beat     | 0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1  1  1  2
  int ticktack = millis() / (MILLIS_PER_TICK / 2);
  ticktack %= TICKS_PER_PHRASE * 2;
  int tick = ticktack / 2;
  bool can_tick = ticktack % 2 == 0;
  int beat = tick / TICKS_PER_BEAT;

  // Update LED color.
  update_led(beat);

  // At the beginning of a phrase, update the current phrase based on
  // the previous recording.
  if (tick == 0 && has_next_phrase) {
    update_phrase();
    has_next_phrase = false;
  } else if (tick == 1) {
    has_next_phrase = true;
  }

  // If we want to play a sound at the current tick, power up the relay
  // at the tick and power down at the tack.
  bool should_tick = current_phrase[tick] && can_tick;
  digitalWrite(RELAY_PIN, should_tick ? HIGH : LOW);

  // Watch for a switch down, and record it in next_phrase.
  if (digitalRead(SWITCH_PIN) == HIGH) {
    is_switch_down = true;
  } else if (is_switch_down) {
    is_switch_down = false;
    next_phrase[tick] = true;
  }
}
