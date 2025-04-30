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
#include <HardwareSerial.h>

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

#define MAX_DIFF  50

#define CMD1 A4
#define CMD2 A5
#define COM_LIDAR_OFF 254

#define LIDAR_NUMBER 3

#define HEADER 0x59 //frame header of data package
#define TIMEOUT_GET_DIST 30


class lidar
{


public:
    uint16_t m_distance[LIDAR_NUMBER];
    uint16_t m_last_choosed_dist;
    bool     m_is_dist_updated[LIDAR_NUMBER];

    /**
     * @brief Constructor
     * @param[in] frameRate Number of measurement per seconds (default 10Hz), O if you want to trigger yourself
    */
    lidar(uint16_t frameRate = 10);

    /**
     * @brief Destructor
    */
    ~lidar();

    void begin();

    /**
     * @brief Get values from lidar
     * @return New distance value or the last saved if it's not computed
    */
    uint16_t get_n_compute();

private:
    const uint8_t TRIGGER_FRAME[4] = {0x5A, 0x04, 0x04, 0x62};
    const uint8_t SAVE_FRAME[4] = {0x5A, 0x04, 0x11, 0x6F};
    const uint8_t SAVE_SUCCEED_FRAME[5] = {0x5A, 0x05, 0x11, 0x00, 0x6F};
    uint16_t m_framerate;

    /**
     * @brief Compute new values
     * @param[in] newDistance New distance from one LIDAR
     * @param[in] index Index of the value in the member tab for lidars
    */
    void compute(uint16_t newDistance, uint8_t index);

    /**
     * @brief Switch the UART on a specific lidar
     * @param[in] lidarNumber Number of the sensor (0, 1, 2)
    */
    void switch_lidar(uint8_t lidarNumber);

    /**
     * @brief Choose the new true distance from the 3 sensors
     * @return The new real distance in cm or UINT16_MAX
    */
    uint16_t choose_distance();
};



#endif