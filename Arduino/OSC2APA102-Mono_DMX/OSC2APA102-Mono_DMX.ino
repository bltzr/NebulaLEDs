/*
  SLIP-OSC.ino

  listen on USB Serial for slip encoded OSC packet
  to switch an LED on and off

  Copyright Antoine Villeret - 2015

*/
#include <OSCBundle.h>
#include <PacketSerial.h>
#include <rdm.h>
#include <TeensyDmx.h>
#include "APA102_WithGlobalBrightness.h"

PacketSerial_<SLIP, SLIP::END, 8192> serial;

// How many leds in your strip?
#define NUM_LEDS 196

// How many DMX channels at Max?
#define NUM_DMX 64

#define DMX_REDE 2

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 11
#define CLOCK_PIN 13

APA102Controller_WithBrightness<DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(4)> ledController;

CRGB leds[NUM_LEDS];
char brightness = 255;
int i = 0;

TeensyDmx Dmx(Serial1, DMX_REDE);


void LEDcontrol(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    ledController.setAPA102Brightness(msg.getInt(0));
  }
  else if (msg.isBlob(0))
  {
    int length = msg.getDataLength(0);
    //a workaround to avoid OSCMessage's bug with blobs, that adds the byte count to the beginning of the actual blob, see: https://github.com/CNMAT/OSC/issues/40
    uint8_t v[length + 4];
    int s = msg.getBlob(0, (unsigned char *)v, min(length + 4, NUM_LEDS * 3));
    memcpy((uint8_t *)leds, v + 4, max(min(s - 4, NUM_LEDS * 3), 0));
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
    int length = msg.getDataLength(0);
    uint8_t DMXvalues[length];
    //a workaround to avoid OSCMessage's bug with blobs, that adds the byte count to the beginning of the actual blob, see: https://github.com/CNMAT/OSC/issues/40
    uint8_t values[length+4];
    int s = msg.getBlob(0, (unsigned char *)values, length + 4);
    memcpy(DMXvalues, values + 4, s - 4);
    Dmx.setChannels(0, DMXvalues, length);
  }
}

void onPacket(const uint8_t* buffer, size_t size) {
  OSCBundle bundleIN;

  for (int i = 0; i < size; i++) {
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
  
  ledController.setAPA102Brightness(1);

  // Now for DMX

  Dmx.setMode(TeensyDmx::DMX_OUT);
  
  //FastLED.show(CRGB::White);
  //delay(500);
  FastLED.show(CRGB::Black);
}

void loop() {
  serial.update();
  FastLED.show();
  Dmx.loop();
}


