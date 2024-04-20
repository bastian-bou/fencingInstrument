/**
 * @author Bastian Bouchardon
 * @date 01/03/2024
 * @version 1.0
*/


#include <lidar.hpp>



lidar::lidar(uint16_t rxPin, uint16_t txPin, uint16_t frameRate)
{
    distance = strength = temperature = 0;
    is_dist_updated = false;

    m_framerate = frameRate;

    m_serial = new SoftwareSerial(rxPin, txPin);
}

lidar::~lidar()
{
}

void lidar::begin()
{
    // Initialize serial port through SoftwareSerial
    m_serial->begin(TFMINIS_BAUDRATE);

    while(!m_serial);

    // Initialize TF mini S
    m_tfmini.begin(m_serial);
    DEBUG_PRINTLN("m_tfmini.begin done!");
    // 10Hz to refesh value
    m_tfmini.setFrameRate(m_framerate);
    DEBUG_PRINTLN("m_tfmini.setFrameRate to "+ String(m_framerate) + " done!");
    m_tfmini.saveSettings();
    DEBUG_PRINTLN("Setting saved !");
}

bool lidar::trigger_n_compute()
{
    Measurement newMeasure = m_tfmini.triggerMeasurement();
    DEBUG_PRINTLN("New value " + String(newMeasure.distance));
    compute(newMeasure);
    return is_dist_updated;
}

uint16_t lidar::get_n_compute()
{
    int check;                    //save check value
    int i;
    int uart[9];                   //save data measured by LiDAR
    const int HEADER = 0x59;      //frame header of data package
    Measurement newMeasure;
    newMeasure.temperature = 0;
    newMeasure.distance = distance;
    newMeasure.strength = strength;

    if (m_serial->available())                //check if serial port has data input
    {
        if (m_serial->read() == HEADER)        //assess data package frame header 0x59
        {
            uart[0] = HEADER;
            if (m_serial->read() == HEADER)      //assess data package frame header 0x59
            {
                uart[1] = HEADER;
                for (i = 2; i < 9; i++)         //save data in array
                {
                    uart[i] = m_serial->read();
                }
                check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
                if (uart[8] == (check & 0xff))        //verify the received data as per protocol
                {
                    newMeasure.distance = uart[2] + uart[3] * 256;     //calculate distance value
                    newMeasure.strength = uart[4] + uart[5] * 256; //calculate signal strength value
                }
            }
        }
    }
    return compute(newMeasure);
}

uint16_t lidar::compute(Measurement newMeasure)
{
    bool isUpdated = false;
    DEBUG_PRINT(newMeasure.distance);
    DEBUG_PRINT("cm, strength ");
    DEBUG_PRINTLN(newMeasure.strength);
    
    if (newMeasure.distance != distance)
    {
        if ((newMeasure.distance >= OFFSET_POS)
            and (newMeasure.distance <= MAX_LENGHT_CM + OFFSET_POS))
        {
            if (((newMeasure.distance - HYST_CM) >= distance)
                or ((newMeasure.distance + HYST_CM) <= distance))
            {
                // TODO: Remove this if it's not necessary in the class, maybe directly in the main.cpp
                /*if (abs(newMeasure.distance - distance) > MAX_DIFF)
                {
                }*/
                distance = newMeasure.distance;
                isUpdated = true;
            }
        }
    }
    is_dist_updated = isUpdated;
    return distance;
}