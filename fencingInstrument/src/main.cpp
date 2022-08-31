#include "main.h"
#include <SoftwareSerial.h>
#include "TFMiniS.h"
#include "MIDIUSB.h"


#define FRAME_RATE 25
#define MIN_STRENGHT 500
// Possible values : 0 - 127
#define MIDI_VELOCITY 127

uint32_t current_millis;

uint16_t last_distance = 0;

// Setup software serial port 
SoftwareSerial mySerial(10, 11);      // Uno RX (TFMINI TX), Uno TX (TFMINI RX)
TFMiniS tfmini;

LedStatus ledBoard(LED_BUILTIN);

void setup() {

  current_millis = millis();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  Serial.println ("Initializing...");

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
  tfmini.setFrameRate(FRAME_RATE);

  // Step 4: Save setting. This is not necessary since we are setting the frame rate on every boot.
  tfmini.saveSettings();
}


void loop() {
  //ledBoard.updateStatus();
  Measurement measurement = tfmini.getMeasurement();

  if (measurement.temperature > 70) {
    ledBoard.setRowDurationError(error_type::LIDAR_TEMP_TOO_HIGH);
  } else {
    ledBoard.setRowDurationError(error_type::NO_ERROR);
  }

  /*Serial.print("Dist: ");
  Serial.println(measurement.distance);
  Serial.print("Strength: ");
  Serial.println(measurement.strength);
  Serial.print("Temperature: ");
  Serial.println(measurement.temperature);*/

  // Wait some time before taking the next measurement. Match this with your frame rate.
  delay(FRAME_RATE);
}


void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}