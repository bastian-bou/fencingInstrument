/**
 * @author Bastian Bouchardon
 * @date 01/03/2024
 * @version 1.0
 * @brief This is a class to manage a TF-MINI-S Lidar by UART connection especially for Antiphona project
 * It is an abstraction layer on TF-MINI-S library with conditions on distance.
*/


#ifndef LIDAR_H
#define LIDAR_H

#include <Arduino.h>
#include "TFMiniS.h"
#include <SoftwareSerial.h>

#ifdef DEBUG
#define DEBUG_PRINT(...)   Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)    
#define DEBUG_PRINTLN(...)  
#endif

#define MAX_LENGHT_CM 1400

/** Distance hysteresis (cm) */
#define HYST_CM 10
/** Offset position (minimun and maximum distance) in cm */
#define OFFSET_POS 30

#define MAX_DIFF  100


class lidar
{


public:
    uint16_t distance;
    uint16_t strength;
    uint16_t temperature;
    bool     is_dist_updated;

    /**
     * @brief Constructor
     * @param[in] rxPin Receive UART pin
     * @param[in] txPin Transmit UART pin
     * @param[in] frameRate Number of measurement per seconds (default 10Hz), O if you want to trigger yourself
    */
    lidar(uint16_t rxPin, uint16_t txPin, uint16_t frameRate = 10);

    /**
     * @brief Destructor
    */
    ~lidar();

    void begin();

    /**
     * @brief Trigger lidar
     * @return True if the distance is updated since the last call or False if it is not
    */
    bool trigger_n_compute();

    /**
     * @brief Get values from lidar
     * @return New distance value or the last saved if it's not computed
    */
    uint16_t get_n_compute();

private:
    SoftwareSerial *m_serial;
    TFMiniS m_tfmini;

    uint16_t m_framerate;

    /**
     * @brief Compute new values
     * @param[in] measure Struct of distance, strength and temperature
     * @return New distance or last saved
    */
    uint16_t compute(Measurement newMeasure);
};



#endif