Firmware for the tacTile!
www.hop-sounds.com

Daisy Seed handles audio + analogReads
(https://electro-smith.com/products/daisy-seed) 

ATtiny1616 drives WS2812 LEDs; recieves commands from Seed over i2c
(https://www.adafruit.com/product/5690)

ESP32 drives PN532 NFC reader + CV/Midi Expansion Unit; communicates with Seed over Serial

TO UPLOAD MODIFIED FIRMWARE: 1) Connect tacTile to computer via USB-C port. 2) With device powered on, hold left joystick "up", right joystick "down", and hold loop button all together for ~8 seconds, until device freezes. 3) Upload your sketch. Detailed instructions for flashing with DaisyDuino can be found here: https://docs.daisy.audio/tutorials/arduino-dev-env/
