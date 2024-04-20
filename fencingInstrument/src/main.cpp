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

/** 
 * Serial interface object
 * 10 = RX, 11 = TX
 */
//SoftwareSerial mySerial(10, 11);

lidar sensor_0(10, 11, 100);
lidar sensor_1(8, 9, 100);
lidar sensor_2(16, 17, 100);

//LedStatus ledBoard(LED_BUILTIN);

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
  for (uint8_t i = 0; i < SENSOR_NUMBER; i++)
  {
    current_distance[i] = 0;
  }
  sensor_0.begin();
  sensor_1.begin();
  sensor_2.begin();

  DEBUG_PRINTLN("Initialized");
}


void loop()
{
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis();
  uint8_t whichDistanceUpdated = 255;
  
  //ledBoard.updateStatus();
  // Wait some time before taking the next measurement. Match this with your frame rate.
  if ((currentMillis - lastMillis) >= FRAME_RATE_MS)
  {
    lastMillis = currentMillis;
    // Update sensor 0 distance
    current_distance[0] = sensor_0.get_n_compute();
    if (sensor_0.is_dist_updated) {
      whichDistanceUpdated = 0;
    }
    // Update sensor 1 distance
    current_distance[1] = sensor_1.get_n_compute();
    if (sensor_1.is_dist_updated) {
      whichDistanceUpdated = 1;
    }
    // Update sensor 2 distance
    current_distance[2] = sensor_2.get_n_compute();
    if (sensor_2.is_dist_updated) {
      whichDistanceUpdated = 2;
    }
  }

  if (whichDistanceUpdated != 255)
  {
    // At least, two distances are identical, take this distance as the new true one
    if ((current_distance[0] == current_distance[1]) || (current_distance[0] == current_distance[2])
        || (current_distance[1] == current_distance[2]))
    {
      uint16_t lidarDistance = (current_distance[0] == current_distance[1]) ? current_distance[0] :
          (current_distance[0] == current_distance[2]) ? current_distance[0] : current_distance[2];
      play_note(lidarDistance);
    }
    else if (abs(current_distance[whichDistanceUpdated] - last_distance) > MAX_DIFF)
    {
      play_note(current_distance[whichDistanceUpdated]);
    }
  }


  /*if (measurement.temperature > 70) {
    ledBoard.setRowDurationError(error_type::LIDAR_TEMP_TOO_HIGH);
  } else {
    ledBoard.setRowDurationError(error_type::NO_ERROR);
  }*/
}

void play_note(uint16_t newDistance)
{
  uint8_t lastPositionNote = (uint8_t)(last_distance / 100);
  uint8_t newPositionNote = (uint8_t)(newDistance / 100);
  uint16_t distance = (uint16_t) (abs(last_distance - newDistance));
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

  last_distance = newDistance;
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