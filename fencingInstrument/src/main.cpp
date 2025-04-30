/**
 * @file main.cpp
 * @author B. Bouchardon
 * @brief 
 * @version 2.0
 * @date 2024-03-15
 * 
 */
#include "main.h"

/** Time between two measures (ms) */
#define FRAME_RATE_MS 100
/** Midi channel to communicate on USB */
#define MIDI_CHANNEL 0

lidar sensors(100);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

#ifdef DEBUG
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  DEBUG_PRINTLN("Initializing...");

#endif

  // Initialise
  sensors.begin();

  DEBUG_PRINTLN("Initialized");
}


void loop()
{
  // Wait some time before taking the next measurement. Match this with your frame rate.
  uint16_t newDistance = sensors.get_n_compute();
  if (newDistance != UINT16_MAX) {
    play_note(newDistance);
  }
  delay(20);
}

void play_note(uint16_t newDistance)
{
  uint8_t lastPositionNote = (uint8_t)(sensors.m_last_choosed_dist / 100);
  uint8_t newPositionNote = (uint8_t)(newDistance / 100);
  uint16_t distance = (uint16_t) (abs(sensors.m_last_choosed_dist - newDistance));
  uint8_t velocity = (uint8_t) (map(distance, HYST_CM, 100, 64, 127));

  DEBUG_PRINT("Position note: ");
  DEBUG_PRINT(newPositionNote);
  DEBUG_PRINT(", Velocity note: ");
  DEBUG_PRINTLN(velocity);
  #ifndef NO_MIDI
    note_on(MIDI_CHANNEL, notePitches[newPositionNote], velocity);
    if (lastPositionNote != newPositionNote) {
      note_off(MIDI_CHANNEL, notePitches[lastPositionNote], 0);
    }
    // Send MIDI data to USB
    MidiUSB.flush();
  #endif

  sensors.m_last_choosed_dist = newDistance;
}

#ifndef NO_MIDI

void note_on(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, (uint8_t)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}


void note_off(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, (uint8_t)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
#endif