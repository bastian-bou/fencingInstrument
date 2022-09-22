/**
 * @file main.cpp
 * @author B. Bouchardon
 * @brief 
 * @version 0.1
 * @date 2022-09-21
 * 
 */
#include "main.h"

/** Time between two measures (ms) */
#define FRAME_RATE_MS 100
/** Midi channel to communicate on USB */
#define MIDI_CHANNEL 0
/** Distance hysteresis (cm) */
#define HYST_CM 10
/** Offset position (minimun and maximum distance) in cm */
#define OFFSET_POS 30

#define MAX_DIFF  100

// Lidar sensor object
TFMiniS tfmini;
/** 
 * Serial interface object
 * 10 = RX, 11 = TX
 */
SoftwareSerial mySerial(10, 11);

//LedStatus ledBoard(LED_BUILTIN);

void setup()
{
  // Initialise structure
  measurement.distance = 0;
  measurement.strength = 0;
  measurement.temperature = 0;

  last_distance = 0;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#ifdef DEBUG
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  Serial.println("Initializing...");

#endif

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerial.begin(TFMINIS_BAUDRATE);

  // Step 3: Initialize the TF Mini sensor
  tfmini.begin(&mySerial);

  // 10Hz to refesh value
  tfmini.setFrameRate(10);

  tfmini.saveSettings();
}


void loop()
{
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis();
  
  //ledBoard.updateStatus();
  // Wait some time before taking the next measurement. Match this with your frame rate.
  if ((currentMillis - lastMillis) >= FRAME_RATE_MS)
  {
    lastMillis = currentMillis;
    measurement = tfmini.getMeasurement();
    #ifdef DEBUG
      Serial.print("Dist: ");
      Serial.print(measurement.distance);
      Serial.print(" Strength: ");
      Serial.println(measurement.strength);
      /*Serial.print(" Temperature: ");
      Serial.println(measurement.temperature);*/
    #endif
  }

  if (measurement.distance != last_distance)
  {
    if ((measurement.distance >= OFFSET_POS) and
        (measurement.distance <= MAX_LENGHT_CM + OFFSET_POS))
    {
      if (((measurement.distance - HYST_CM) >= last_distance) or
          ((measurement.distance + HYST_CM) <= last_distance))
      {
        if (abs(measurement.distance - last_distance) > MAX_DIFF)
        {
          uint8_t last_position_note = (uint8_t)(last_distance / 100);
          uint8_t new_position_note = (uint8_t)(measurement.distance / 100);
          uint16_t distance = (uint16_t) (abs(last_distance - measurement.distance));
          uint8_t velocity = (uint8_t) (map(distance, HYST_CM, 100, 64, 127));

        #ifdef DEBUG
          Serial.print("Position note: ");
          Serial.print(new_position_note);
          Serial.print(", Velocity note: ");
          Serial.println(velocity);
        #endif
        #ifndef NO_MIDI
          noteOn(MIDI_CHANNEL, notePitches[new_position_note], velocity);
          if (last_position_note != new_position_note) {
            noteOff(MIDI_CHANNEL, notePitches[last_position_note], 0);
          }
          // Send MIDI data to USB
          MidiUSB.flush();
        #endif
          last_distance = measurement.distance;
        }
      }
    }
  }

  /*if (measurement.temperature > 70) {
    ledBoard.setRowDurationError(error_type::LIDAR_TEMP_TOO_HIGH);
  } else {
    ledBoard.setRowDurationError(error_type::NO_ERROR);
  }*/

}

#ifndef NO_MIDI
/**
 * @brief Set a MIDI note
 * 
 * @param channel channel number to communicate
 * @param pitch note to play
 * @param velocity the force with which you press the note
 */
void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, (uint8_t)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

/**
 * @brief Unset a MIDI note
 * 
 * @param channel channel number to communicate
 * @param pitch note to unplay
 * @param velocity the force with which you press the note (not mandatory)
 */
void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, (uint8_t)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

#endif