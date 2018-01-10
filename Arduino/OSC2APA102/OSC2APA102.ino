/*
  OSC2APA102.ino

  listen on USB Serial for slip encoded OSC packet
  to switch an LED strip on and off

  Copyright Antoine Villeret / Pascal Baltazar - 2015/2017

*/
#include <OSCBundle.h>
#include <PacketSerial.h>
#include "APA102_WithGlobalBrightness.h"

PacketSerial_<SLIP, SLIP::END, 8192> serial;

// How many leds in your strip?
#define NUM_LEDS 320 
#define NUM_LEDS2 320

// APA102 LED strips are SPI based (four wires - data, clock,
// ground, and power), so we have to define DATA_PIN and CLOCK_PIN
#define DATA_PIN 11
#define CLOCK_PIN 13
#define DATA_PIN2 7
#define CLOCK_PIN2 14

APA102Controller_WithBrightness<DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(6)> ledController;
APA102Controller_WithBrightness<DATA_PIN2, CLOCK_PIN2, BGR, DATA_RATE_MHZ(6)> ledController2;

CRGB leds[NUM_LEDS], leds2[NUM_LEDS2];

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

void LEDcontrol2(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    ledController2.setAPA102Brightness(msg.getInt(0));
  }
  else if (msg.isBlob(0))
  {
    msg.getBlob(0, (unsigned char *)leds2);
  }
}

void setGlobalBrightness(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    FastLED.setBrightness(msg.getInt(0));
  }
}

void onPacket(const uint8_t* buffer, size_t size) {
  OSCBundle bundleIN;

  for (uint32_t i = 0; i < size; i++) {
    bundleIN.fill(buffer[i]);
  }

  if (!bundleIN.hasError()) {
    bundleIN.dispatch("/1", LEDcontrol);
    bundleIN.dispatch("/2", LEDcontrol2);
    bundleIN.dispatch("/b", setGlobalBrightness);
  }
}

void setup() {
  // We must specify a packet handler method so that
  serial.setPacketHandler(&onPacket);
  serial.begin(12000000); // baudrate is ignored, is always run at 12Mbps
  FastLED.addLeds((CLEDController*) &ledController, leds, NUM_LEDS);
  FastLED.addLeds((CLEDController*) &ledController2, leds2, NUM_LEDS);

  // Setting brightness to minimum
  ledController.setAPA102Brightness(1);
  ledController2.setAPA102Brightness(1);
  
    // Turn off all LEDs 
  FastLED.show(CRGB::Black);
}

void loop() {
  serial.update();
  FastLED.show();
}



