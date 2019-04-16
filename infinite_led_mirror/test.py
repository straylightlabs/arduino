#!/usr/bin/env python

from led_utils import *

last_color = Color(0, 0, 0)
set(last_color)
for color in [
        Color(.6, .2, .2),
        Color(.2, .6, .2),
        Color(.2, .2, .6),
        Color(0, 0, 0)]:
    fade(last_color, color, 1.0)
    last_color = color

pulse(Color(.6, .6, .6), 1.0, 3)
