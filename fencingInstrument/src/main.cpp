#include "main.h"
#include <SoftwareSerial.h>

#define FRAME_RATE_MS 25
#define MIN_STRENGHT 500
// Possible values : 0 - 127
#define MIDI_VELOCITY 127
#define MIDI_CHANNEL 0
#define HYST_CM 5


// Setup software serial port 
SoftwareSerial mySerial(10, 11);      // Uno RX (TFMINI TX), Uno TX (TFMINI RX)
TFMiniS tfmini;

LedStatus ledBoard(LED_BUILTIN);

void setup()
{
  measurement.distance = 0;
  measurement.strength = 0;
  measurement.temperature = 0;

  last_distance = 0;

  offset_position = 30;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#ifdef DEBUG
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  Serial.println ("Initializing...");

#endif

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerial.begin(TFMINIS_BAUDRATE);

  // Step 3: Initialize the TF Mini sensor
  tfmini.begin(&mySerial);

  // Step 3: Set the frame rate to how quickly you are planning on reading from the device
  // Picking your frame rate is the most imporant part because UART will buffer readings and
  // if you aren't reading quickly enough then the sensor values will change VERY slowly.
  // You'll notice that I've set the frame rate to 4Hz and in the loop I'm delaying by 250ms
  // So I will be keeping up with the sensor readings. This is what makes the sensor seem
  // performant.
  tfmini.setFrameRate(FRAME_RATE_MS);

  // Step 4: Save setting. This is not necessary since we are setting the frame rate on every boot.
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
  }

  if (measurement.distance != last_distance)
  {
    if ((measurement.distance >= offset_position) and
        (measurement.distance <= MAX_LENGHT_CM + offset_position))
    {
      if (((measurement.distance - HYST_CM) >= last_distance) or
          ((measurement.distance + HYST_CM) <= last_distance))
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
        MidiUSB.flush();
      #endif
        last_distance = measurement.distance;
      }
    }
  }

  /*if (measurement.temperature > 70) {
    ledBoard.setRowDurationError(error_type::LIDAR_TEMP_TOO_HIGH);
  } else {
    ledBoard.setRowDurationError(error_type::NO_ERROR);
  }*/

#ifdef DEBUG
  Serial.print("Dist: ");
  Serial.print(measurement.distance);
  Serial.print(" Strength: ");
  Serial.print(measurement.strength);
  Serial.print(" Temperature: ");
  Serial.println(measurement.temperature);
#endif

}

#ifndef NO_MIDI
void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, (uint8_t)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, (uint8_t)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

#endif