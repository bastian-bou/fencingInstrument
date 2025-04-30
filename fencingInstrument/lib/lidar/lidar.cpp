/**
 * @file lidar.cpp
 * @author Bastian Bouchardon
 * @date 01/03/2024
 * @version 1.0
*/

#include <lidar.hpp>


lidar::lidar(uint16_t frameRate)
{
    for (uint8_t i = 0; i < LIDAR_NUMBER; i++) {
        m_distance[i] = 0;
        m_is_dist_updated[i] = false;
    }
    m_last_choosed_dist = 0;

    m_framerate = frameRate;
}

lidar::~lidar()
{
}

void lidar::begin()
{
    // Initialize serial port through SoftwareSerial
    Serial1.begin(115200);
    Serial1.setTimeout(200);

    while(!Serial1);

    // Initialize cmd pin CMD1 and CMD2
    pinMode(CMD1, OUTPUT);
    pinMode(CMD2, OUTPUT);

    switch_lidar(COM_LIDAR_OFF);
}

uint16_t lidar::get_n_compute()
{
    uint8_t uart[9] = {0}; //save data measured by LiDAR

    for (uint8_t index = 0; index < LIDAR_NUMBER; index++) {
        switch_lidar(COM_LIDAR_OFF);
        // Flush receive buffer before switching on a sensor
        while(Serial1.available()) Serial1.read();

        switch_lidar(index);
        if (Serial1.readBytes(uart, 9) > 0) {
            if (uart[0] == HEADER && uart[1] == HEADER) {
                uint16_t check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
                if (uart[8] == (check & 0xff)) //verify the checksum
                {
                    uint16_t dist = uart[2] + (uart[3] << 8); //calculate distance value
                    compute(dist, index);
                }
            } else {
                DEBUG_PRINT("#");
                DEBUG_PRINT(index);
                DEBUG_PRINTLN(": No HEADER detected");
                for (uint8_t i = 0; i < 9; i++) {
                    DEBUG_PRINT(uart[i], HEX);
                }
                DEBUG_PRINTLN("");
                m_is_dist_updated[index] = false;
            }
        } else {
            DEBUG_PRINT("#");
            DEBUG_PRINT(index);
            DEBUG_PRINTLN(": ERROR during readBytes");
            m_is_dist_updated[index] = false;

        }
    }
    switch_lidar(COM_LIDAR_OFF);
    return choose_distance();
}

void lidar::compute(uint16_t newDistance, uint8_t index)
{
    bool isUpdated = false;
    uint16_t lastDistance = m_distance[index];

    DEBUG_PRINT("#");
    DEBUG_PRINT(index);
    DEBUG_PRINT(": ");
    DEBUG_PRINT(newDistance);
    DEBUG_PRINTLN(" cm");
    
    if (newDistance != lastDistance)
    {
        if ((newDistance >= OFFSET_POS)
            and (newDistance <= MAX_LENGHT_CM + OFFSET_POS))
        {
            if (((newDistance - HYST_CM) >= lastDistance)
                or ((newDistance + HYST_CM) <= lastDistance))
            {
                m_distance[index] = newDistance;
                isUpdated = true;
            }
        }
    }
    m_is_dist_updated[index] = isUpdated;
}

uint16_t lidar::choose_distance()
{
    uint8_t whichDistanceUpdated = UINT8_MAX;

    for (uint8_t index = 0; index < LIDAR_NUMBER; index++) {
        if (m_is_dist_updated[index]) whichDistanceUpdated = index;
    }

    if (whichDistanceUpdated != UINT8_MAX) {
        // At least, two distances are identical, take this distance as the new true one
        if ((m_distance[0] == m_distance[1]) || (m_distance[0] == m_distance[2])
            || (m_distance[1] == m_distance[2]))
        {
            return (m_distance[0] == m_distance[1]) ? m_distance[0] :
                (m_distance[0] == m_distance[2]) ? m_distance[0] : m_distance[2];

        }
        else if (abs(m_distance[whichDistanceUpdated] - m_last_choosed_dist) > MAX_DIFF)
        //else
        {
            return m_distance[whichDistanceUpdated];
        }
    }

    return UINT16_MAX;
}

void lidar::switch_lidar(uint8_t lidarNumber)
{
    switch (lidarNumber)
    {
    case 0:
        digitalWrite(CMD1, HIGH);
        digitalWrite(CMD2, LOW);
        break;
    case 1:
        digitalWrite(CMD1, LOW);
        digitalWrite(CMD2, HIGH);
        break;
    case 2:
        digitalWrite(CMD1, HIGH);
        digitalWrite(CMD2, HIGH);
        break;

    case COM_LIDAR_OFF:
        digitalWrite(CMD1, LOW);
        digitalWrite(CMD2, LOW);
        break;
    
    default:
        DEBUG_PRINT("Error on lidar number: ");
        DEBUG_PRINTLN(lidarNumber);
        break;
    }
}