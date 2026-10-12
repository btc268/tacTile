#include <Adafruit_PN532.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
// #include <Adafruit_BNO055.h>
// #include <utility/imumaths.h>


#define PN532_IRQ (2)
#define PN532_RESET (3)


#define CARDREADREQUEST 201
// #define NOTHINGDETECTED 202



// CALIBRATION CONSTANTS (device-specific)

int halfStepZero = 498;
int halfStepLow = 690;
int halfStepHigh = 320;

int octaveZero = 490;
int octaveLow = 635;
int octaveHigh = 300;


int fsrMod0 = 20;
int fsrMod1 = 17;
int fsrMod2 = 17;
int fsrMod3 = 20;
int fsrMod4 = 12;
int fsrMod5 = 20;
int fsrMod6 = 18;
int fsrMod7 = 20;
int fsrMod8 = 20;



uint16_t sensorReading[8];
double sensorReadingFloat[8];
byte sensorReadingByte[8];


byte zoneReading[9];

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

unsigned long lastNfcCheckTime = 0;
int nfcCheckInterval = 1500;
int response = 0;
int lastResponse = 0;
int tagID = 0;

bool externalMode = false;
bool lastExternalMode = false;

float pitch = 0.0;
float roll = 0.0;
float yaw = 0.0;

float angle = 0.0;
float magnitude = 0.0;

int currentQuadrant = 0;

int hallPin = 2;
int optoPin = 3;

bool hallState = HIGH;
bool optoState = HIGH;

bool lastHallState = HIGH;
bool lastOptoState = HIGH;

bool readSuccess = false;


bool cvMode = false;




byte calibrationByte[21];





void setup() {
  delay(300);

  Wire.begin(8, 9);

  Serial1.begin(9600, SERIAL_8N1, 44, 43);


  Serial2.begin(9600, SERIAL_8N1, 12, 13);



  ////////////////////

  calibrationByte[0] = halfStepZero / 3;
  calibrationByte[1] = halfStepZero % 3;

  calibrationByte[2] = halfStepLow / 3;
  calibrationByte[3] = halfStepLow % 3;

  calibrationByte[4] = halfStepHigh / 3;
  calibrationByte[5] = halfStepHigh % 3;

  calibrationByte[6] = octaveZero / 3;
  calibrationByte[7] = octaveZero % 3;

  calibrationByte[8] = octaveLow / 3;
  calibrationByte[9] = octaveLow % 3;

  calibrationByte[10] = octaveHigh / 3;
  calibrationByte[11] = octaveHigh % 3;

  calibrationByte[12] = fsrMod0 + 120;
  calibrationByte[13] = fsrMod1 + 120;
  calibrationByte[14] = fsrMod2 + 120;
  calibrationByte[15] = fsrMod3 + 120;
  calibrationByte[16] = fsrMod4 + 120;
  calibrationByte[17] = fsrMod5 + 120;
  calibrationByte[18] = fsrMod6 + 120;
  calibrationByte[19] = fsrMod7 + 120;
  calibrationByte[20] = fsrMod8 + 120;


  Serial1.write(240);  // start byte (also signifies version number)

  while (!Serial1.available()) {  // wait for acknowledgement
  }

  int ack = Serial1.read();

  if (ack == 239) {
    for (int i = 0; i < 21; i++) {  // 12 joystick constants (byte/3 + remainder) + 9 fsr constants
      Serial1.write(calibrationByte[i]);
      delayMicroseconds(600);
    }

    while (!Serial1.available()) {  // wait for acknowledgement
    }

    int rec = Serial1.read();
  }


  /////////////////////



  pinMode(hallPin, INPUT_PULLUP);
  pinMode(optoPin, INPUT_PULLUP);



  while (!nfc.begin()) {
    delay(10);
  }
}







void loop() {


  if (cvMode) {
    if (Serial1.available() >= 10) {
      int message = Serial1.read();

      if (message == 0) {
        for (int i = 0; i < 9; i++) {
          zoneReading[i] = Serial1.read();
        }

        Serial2.write(0);
        for (int i = 0; i < 9; i++) {
          Serial2.write(zoneReading[i]);
        }
      }
    }




  } else {

    int message = Serial1.read();

    if (message == 253) {              // force card read
      Serial1.write(CARDREADREQUEST);  // send request

      while (!Serial1.available()) {  // wait for response
      }

      response = Serial1.read();

      if (response == 0) {  // request denied
      }

      else if (response == 1) {  // request approved, check nfc, reinitialize nfc, send results
        while (readSuccess == false) {
          checkNFC();
        }
        nfc.begin();
        Serial1.write(tagID);
        readSuccess = false;
      }
    }



    lastHallState = hallState;
    hallState = digitalRead(hallPin);

    lastOptoState = optoState;
    optoState = digitalRead(optoPin);




    // HALL

    if (lastHallState == HIGH && hallState == LOW) {  // pad ON

      delay(75);

      if (digitalRead(hallPin) == LOW) {  // confirm reading

        delay(75);

        if (digitalRead(hallPin) == LOW) {  // confirm reading again

          Serial1.write(CARDREADREQUEST);  // send request

          while (!Serial1.available()) {  // wait for response
          }

          response = Serial1.read();

          if (response == 0) {  // request denied
          }

          else if (response == 1) {  // request approved, check nfc, reinitialize nfc, send results
            while (readSuccess == false) {
              checkNFC();
            }
            nfc.begin();
            Serial1.write(tagID);
            readSuccess = false;
          }
        }
      }
    }


    if (lastHallState == LOW && hallState == HIGH) {  // pad OFF

      delay(75);

      if (digitalRead(hallPin) == HIGH) {  // confirm reading

        delay(75);

        if (digitalRead(hallPin) == HIGH) {  // confirm reading again

          Serial1.write(CARDREADREQUEST);  // send request

          while (!Serial1.available()) {  // wait for response
          }

          response = Serial1.read();

          if (response == 0) {  // request denied
          }

          else if (response == 1) {  // request approved, send default patch byte
            Serial1.write(77);
          }
        }
      }
    }




    // OPTO

    if (lastOptoState == HIGH && optoState == LOW) {
      delay(25);

      Serial1.write(CARDREADREQUEST);  // send request

      while (!Serial1.available()) {  // wait for response
      }

      response = Serial1.read();

      if (response == 0) {  // request denied
      }

      else if (response == 1) {  // request approved, check nfc, reinitialize nfc, send results
        while (readSuccess == false) {
          checkNFC();
        }
        nfc.begin();
        Serial1.write(tagID);
        readSuccess = false;

        if (tagID == 104) {
          cvMode = !cvMode;
        }
      }
    }
  }
}


void checkNFC() {

  lastExternalMode = externalMode;

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

  if (success) {

    if (uidLength == 4) {
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success) {
        uint8_t data[16];
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success) {
          tagID = data[0];

          if (tagID == 103) {
            externalMode = !externalMode;
          }

          readSuccess = true;
        }
      }
    }
  } else {
    // tagID = NOTHINGDETECTED;
  }
}