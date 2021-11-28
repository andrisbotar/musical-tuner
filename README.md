# musical-tuner
Tuning device for a musical instrument

This is the code for an Arduino Mega based electronic tuner. It uses a small microhpone to record the sound of an instrument and calculates its freqeuncy spectrum usign the ArduinoFFT library. The fundamental freqquency of the sound i.e. the note being played, is extracted using HPS analysis and is compared to reference tunign frequencies. Based on this the note being played is displayed using an 8-segment display, adn one of 5 LED-s indicate whether the instrument needs to be tuned higher or lower. 
