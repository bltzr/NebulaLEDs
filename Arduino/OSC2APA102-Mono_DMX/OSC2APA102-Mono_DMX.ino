/*
  OSC2APA102.ino

  listen on USB Serial for slip encoded OSC packet
  to switch an LED strip on and off

  Copyright Antoine Villeret / Pascal Baltazar - 2015/2017

*/
#include <OSCBundle.h>
#include <PacketSerial.h>
#include <TeensyDmx.h>
#include "APA102_WithGlobalBrightness.h"

PacketSerial_<SLIP, SLIP::END, 8192> serial;

// How many leds in your strip?
#define NUM_LEDS 12

// How many DMX channels at Max?
#define NUM_DMX 64

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 11
#define CLOCK_PIN 27 // 27 for teensy >= 3.5 / for <3.5, use pin 13 (which causes the LED to stay lit)

APA102Controller_WithBrightness<DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(4)> ledController;

CRGB leds[NUM_LEDS];


uint8_t DMXvalues[NUM_DMX];
TeensyDmx Dmx(Serial1);


void LEDcontrol(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    ledController.setAPA102Brightness(msg.getInt(0));
  }
  else if (msg.isBlob(0))
  {
    msg.getBlob(0, (unsigned char *)leds);
  }
}


void setGlobalBrightness(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    FastLED.setBrightness(msg.getInt(0));
  }
}

void setDMX(OSCMessage &msg)
{
  if (msg.isBlob(0))
  {
    Dmx.setChannels(0, DMXvalues, msg.getBlob(0, (unsigned char *)DMXvalues));
  }
}

void onPacket(const uint8_t* buffer, size_t size) {
  OSCBundle bundleIN;

  for (uint32_t i = 0; i < size; i++) {
    bundleIN.fill(buffer[i]);
  }

  if (!bundleIN.hasError()) {
    bundleIN.dispatch("/1", LEDcontrol);
    bundleIN.dispatch("/b", setGlobalBrightness);
    bundleIN.dispatch("/DMX", setDMX);
  }
}

void setup() {
  // We must specify a packet handler method so that
  serial.setPacketHandler(&onPacket);
  serial.begin(12000000); // baudrate is ignored, is always run at 12Mbps
  FastLED.addLeds((CLEDController*) &ledController, leds, NUM_LEDS);

  // Now for DMX
  Dmx.setMode(TeensyDmx::DMX_OUT);

  // Setting brightness to minimum
  ledController.setAPA102Brightness(1);

  // Turn off all LEDs
  FastLED.show(CRGB::Black);
}

void loop() {
  serial.update();
  FastLED.show();
}



