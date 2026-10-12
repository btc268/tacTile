Firmware for the tacTile!
www.hop-sounds.com

Daisy Seed handles audio + analogReads
(https://electro-smith.com/products/daisy-seed) 

ATtiny1616 drives WS2812 LEDs; recieves commands from Seed over i2c
(https://www.adafruit.com/product/5690)

Arduino Nano drives PN532 NFC reader and optional external i2c sensor (6-DoF IMU, in this case); communicates with Seed over Serial
