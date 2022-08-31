#include <Arduino.h>


#define DURATION_ROW 4


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

void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);