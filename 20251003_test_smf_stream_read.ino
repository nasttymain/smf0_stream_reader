/*
Based on Adafruit TinyUSB sample "midi_test"
*/
/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <stdio.h>

#include "smstream.hpp"
#include "yozakura.cpp"

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

uint64_t tempo_us = 666 * 1000;

FILE* fp;
smsdat_t smsdat;
void setup() {
  fp = fmemopen((void*)&smf[0], sizeof(smf), "rb");
  mstream_set_file(&smsdat, fp);
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  usb_midi.setStringDescriptor("TinyUSB MIDI");

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

}

uint32_t t = 0;
void loop() {
  #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif

  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  static uint64_t start_ms = 0;
  if(millis() - start_ms > 1000){
    start_ms += 1000;
    Serial.print(start_ms);
    Serial.print("\n");
    if(start_ms == 5000){
      t = 1;
      __dump_smsdat(&smsdat);
    }
  }
  if(t >= 1){
    static uint64_t last_timestamp = 0;
    if(t == 1){
      last_timestamp = micros();
    }
    t += 1;
    smmsg_t smmsg;
    if(mstream_next_message(&smmsg, &smsdat) != 0){
      while(micros() - last_timestamp <= tempo_us * smmsg.delta / smsdat.time_base){MIDI.read();}
      last_timestamp += tempo_us * smmsg.delta / smsdat.time_base;
      if(smmsg.status != 0xFF){
        __dump_smmsg(&smmsg);
        MIDI.send((midi::MidiType)(smmsg.status >= 0xF0 ? (smmsg.status & 0xF0) : smmsg.status), smmsg.param1, smmsg.param2, smmsg.status % 16 + 1);
      }else{
        // meta message
        if(smmsg.param1 == 0x51){
          // tempo change
          tempo_us = smmsg.meta_data[0] * 65536 + smmsg.meta_data[1] * 256 + smmsg.meta_data[2];
        }
      }
    }
  }


  // read any new MIDI messages
  MIDI.read();
}
