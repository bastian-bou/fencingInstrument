#include <Arduino.h>
#include "TFMiniS.h"

#ifndef NO_MIDI
#include "MIDIUSB.h"
#include "pitchToNote.h"
#endif

#define DURATION_ROW 4
#define NUM_NOTES 14

#define MAX_LENGHT_CM 14000

enum error_type {
    LIDAR_TEMP_TOO_HIGH = 0,
    LIDAR_COMM_ERROR,
    MIDI_ERROR,
    NO_ERROR
};

uint16_t duration_error[DURATION_ROW][2] = {
    {error_type::LIDAR_TEMP_TOO_HIGH, 50},
    {error_type::LIDAR_COMM_ERROR, 250},
    {error_type::MIDI_ERROR, 500},
    {error_type::NO_ERROR, 0}
};

struct LedStatus {
    uint8_t led_num;
    uint16_t duration_row;

    unsigned long last_millis;
    bool led_state;

    LedStatus()
    {
        LedStatus(LED_BUILTIN);
    }

    LedStatus(uint8_t ledNum)
    {
        last_millis = 0;
        led_state = true;
        setRowDurationError(error_type::NO_ERROR);
        pinMode(led_num = ledNum, OUTPUT);
        digitalWrite(led_num, HIGH);
    }

    void setRowDurationError(error_type error)
    {
        for (int i = 0; i < DURATION_ROW; i++)
        {
            if (duration_error[i][0] == error)
            {
                duration_row = i;
                break;
            }
        }
    }

    void updateStatus()
    {
        // if we have a duration to blink
        if (duration_error[duration_row][1] != 0)
        {
            unsigned long currentMillis = millis();

            if ((currentMillis - last_millis) >= duration_error[duration_row][1])
            {
                last_millis = currentMillis;
                led_state = !led_state;
                digitalWrite(led_num, led_state);
            }
        }
    }

};

#ifndef NO_MIDI
const byte notePitches[NUM_NOTES] = {pitchC3, pitchD3, pitchE3, pitchF3, pitchG3, pitchA3, pitchB3, pitchC4, pitchD4, pitchE4, pitchF4, pitchG4, pitchA4, pitchB4};
#endif

Measurement measurement;
uint16_t last_distance;
uint16_t offset_position;

void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);