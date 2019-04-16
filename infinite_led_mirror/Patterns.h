void set(int r, int g, int b) {
  uint32_t c = neopixel.Color(r, g, b);
  for (int i = 0; i < NUM_LEDS; i++) {
    neopixel.setPixelColor(i, c);
  }
  neopixel.show();
}

float identity(float r) {
  return r;
}

float easeout(float r) {
  return sin(r * PI / 2);
}

float easein(float r) {
  return 1.0 - sin((1.0 + r) * PI / 2);
}

void fade(int r0, int g0, int b0, int r1, int g1, int b1, float duration, float (*easing)(float)) {
  float t = 0;
  while (t <= duration) {
    set(r0 + (r1 - r0) * (*easing)(t / duration),
        g0 + (g1 - g0) * (*easing)(t / duration),
        b0 + (b1 - b0) * (*easing)(t / duration));
    t += INTERVAL;
    delay(INTERVAL * 1000);
  }
  set(r1, g1, b1);
}

void pulse(int r, int g, int b, float base_a, float interval, int repeat) {
  for (int i = 0; i < repeat; i++) {
    float a = 0;
    float t = 0;
    float d = 2 / interval / FPS;
    while (t <= interval) {
      const float actual_a = a * (1.0 - base_a) + base_a;
      set(r * actual_a, g * actual_a, b * actual_a);
      a += d;
      if (a > 1.0) {
        a = 1.0;
        d *= -1;
      } else if (a < 0.0) {
        a = 0.0;
      }
      t += INTERVAL;
      delay(INTERVAL * 1000);
    }
  }
  set(0, 0, 0);
}

void rotate(int r, int g, int b, int width, float duration) {
  float t = 0.0;
  while (t <= duration) {
    const float pos = neopixel.numPixels() * t / duration;
    for (int i = 0; i < neopixel.numPixels(); i++) {
      const float a = 1.0f - std::min(1.0f, abs(i - pos) / width);
      neopixel.setPixelColor(i, neopixel.Color(r * a, g * a, b * a));
    }
    neopixel.show();
    t += INTERVAL;
    delay(INTERVAL * 1000);
  }
  set(0, 0, 0);
}

uint32_t wheel(int pos, float a) {
  if (pos < 85) {
    return neopixel.Color(a * pos * 3, a * (255 - pos * 3), 0);
  } else if (pos < 170) {
    pos -= 85;
    return neopixel.Color(a * (255 - pos * 3), 0, a * pos * 3);
  } else {
    pos -= 170;
    return neopixel.Color(0, a * pos * 3, a * (255 - pos * 3));
  }
}

void rainbow() {
  int a = 0;
  for (int j = 0; j < 103; j++) {
    for (int i = 0; i < neopixel.numPixels(); i++) {
      neopixel.setPixelColor(i, wheel((int(i * 256 / neopixel.numPixels()) + j * 5) & 255, a / 250.));
    }
    neopixel.show();
    if (j < 25) {
      a += 10;
    } else if (j >= 77) {
      a -= 10;
    }
    delay(INTERVAL * 1000);
  }
  set(0, 0, 0);
}

void flicker() {
  for (int j = 0; j < neopixel.numPixels(); j++) {
    if (random(1000) < 120) {
      neopixel.setPixelColor(j, neopixel.Color(0, 0, 0));
    }
    if (random(1000) < 1) {
      float a = 100. / random(101);
      neopixel.setPixelColor(j, neopixel.Color(flicker_r * a, flicker_g * a, flicker_b * a));
    }
  }
  neopixel.show();
}

