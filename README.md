[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg

# Antiphona

This is an art project name Antiphona from the artist [Alexandra Riss](https://alexandra-riss.fr/).

It transforms the movements of a fencer during a fight into musical notes.
The fight becomes a unique music.

The device consists of two tripods on either side of the fencing piste.
Each tripod consists of 3 IR sensors spaced 50cm apart and an electronic interface PCB between an arduino micro and the 3 sensors.


## Hardware

IR Sensors are tfmini-s [datasheet](pj2-sj-pm-tfmini-s-a00-product-mannual-en-2155.pdf)

They need 5V DC power supply but the communication is through a LVTTL (3.3V) UART interface.

As the result, the PCB include a level shifter from 3.3V to 5V for the arduino and because the arduino will interrogate each sensor one after the other, there is a 3-way analog multiplexer for each communication channel (RX, TX).  
The PCB also include decoupling capacitors, as all components including sensors are powered by the 5V from the USB port on the arduino.

## Software

To communicate with sensors, I made my own library according to the datasheet with only the functionalities I needed and my specificities, which were:
- multipexer management,
- retrieval of values from the 3 lidars,
- selection of the right value on the 3 sensors

Every 20ms we interrogate the sensors, and play a MIDI note on the USB interface only if a new valid distance is received.  
The MIDI note is played as a function of distance and relative speed. Each meter on the track corresponds to a note from C3 to B4, and the speed (as a function of the distance separating the old value from the new one) corresponds to the velocity of the note.