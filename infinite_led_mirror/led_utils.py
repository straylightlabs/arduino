import urllib2
from time import sleep

def command(command_str):
    urllib2.urlopen('http://192.168.0.6:8080/' + command_str)

class Color(object):
    def __init__(self, r, g, b):
        self.r = r * 255
        self.g = g * 255
        self.b = b * 255

def set(c):
    command('set(%.0lf,%.0lf,%.0lf)' % (c.r, c.g, c.b))

def ease(c0, c1, duration, easing):
    command('%s(%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.2lf)' % (
        easing, c0.r, c0.g, c0.b, c1.r, c1.g, c1.b, duration))
    sleep(duration)

def fade(c0, c1, duration):
    ease(c0, c1, duration, 'fade')

def easein(c0, c1, duration):
    ease(c0, c1, duration, 'easein')

def easeout(c0, c1, duration):
    ease(c0, c1, duration, 'easeout')

def pulse(c, interval, repeat):
    command('pulse(%.0lf,%.0lf,%.0lf,%.2lf,%.0lf)' % (
        c.r, c.g, c.b, interval, repeat))
