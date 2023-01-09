/**
 *
 * @file    main.cpp
 * @authors Riccardo Iacob, Marco Pagotto; 5ELIA A.S. 2022/23
 * @brief   Multiplexing of a 7+1-segment, 4-digit display, which is controlled by an Android app.
 * @version 0.1
 * @date    2022-12-17
 *
 * @copyright MIT License
 *
 */

// Includes
#include <Arduino.h>

// Mixed Settings
#define kSerialBufferSize 9 // The size of the serial buffer
// Serial Commands
#define CMD_DIGIT_1 10   // Set first digit according to following data
#define CMD_DIGIT_2 20   // Set second digit according to following data
#define CMD_DIGIT_3 30   // Set third digit according to following data
#define CMD_DIGIT_4 40   // Set fourth digit according to following data
#define CMD_DIGIT_ALL 50 // Set all digits according to following data
// Display Settings
#define kDigits 4   // Number of digits
#define kSegments 8 // Number of segments
#define kPolarity 0 // Polarity of the display, 1 is common anode and 0 is common cathode

// Digits' GPIOs
uint8_t pinsDigits[kDigits] = {2, 3, 4, 5};
// Segments' GPIOs
uint8_t pinsSegments[kSegments] = {6, 7, 8, 9, 10, 11, 12, 13};
// Display Buffer
// The first dimension of the array contains the digit index
// The second dimension of the array contains the segments' statuses in the corresponding digit
uint8_t displayBuffer[kDigits][kSegments];
// Serial buffer
uint8_t serialBuffer[kSerialBufferSize];

// Function prototypes
void multiplexDisplay(int persistenceTime);
void handleSerial();

void setup()
{
  // Begin serial port
  Serial.begin(9600);
  int i = 0;
  // Set all digits' GPIOs to output
  for (i = 0; i < kDigits; i++)
  {
    pinMode(pinsDigits[i], OUTPUT);
  }
  // Set all segments' GPIOs to output
  for (i = 0; i < kSegments; i++)
  {
    pinMode(pinsSegments[i], OUTPUT);
  }
}

void loop()
{
  // Continue multiplexing the display using the data in the displayBuffer (takes about (persistenceTime*4)mS to complete)
  multiplexDisplay(5);
  // Continuously check for incoming serial data
  handleSerial();
}

// Multiplexing function, pulses each display digit with its own contents retrieved from the displayBuffer
void multiplexDisplay(int persistenceTime)
{
  // For each digit
  for (int i = 0; i < kDigits; i++)
  {
    // For each segment in each digit, set the status according to the displayBuffer
    for (int j = 0; j < kSegments; j++)
    {
      // The polarity of the segments is the opposite to the polarity of the digits
      digitalWrite(pinsSegments[j], (displayBuffer[i][j] && !kPolarity));
    }
    // Pulse the digit with the given persistenceTime
    digitalWrite(pinsDigits[i], kPolarity);
    delay(persistenceTime);
    digitalWrite(pinsDigits[i], !kPolarity);
  }
}

// Handle the incoming serial data
void handleSerial()
{
  // Save incoming data to the serial buffer (CMD,a,b,c,d,e,f,g,dp; total 9 bytes of data)
  if (Serial.available() == 9)
  {
    for (int i = 0; i < 9; i++)
    {
      serialBuffer[i] = Serial.read();
    }
  }
  // Clear the internal serial buffer
  Serial.flush();
  // Compare the incoming command
  switch (serialBuffer[0])
  {
  // Set first digit according to following segment data
  case CMD_DIGIT_1:
  {
    for (int i = 0; i < kSegments; i++)
    {
      displayBuffer[0][i] = serialBuffer[i + 1];
    }
    break;
  }
  // Set second digit according to following segment data
  case CMD_DIGIT_2:
  {
    for (int i = 0; i < kSegments; i++)
    {
      displayBuffer[1][i] = serialBuffer[i + 1];
    }
    break;
  }
  // Set third digit according to following segment data
  case CMD_DIGIT_3:
  {
    for (int i = 0; i < kSegments; i++)
    {
      displayBuffer[2][i] = serialBuffer[i + 1];
    }
    break;
  }
  // Set fourth digit according to following segment data
  case CMD_DIGIT_4:
  {
    for (int i = 0; i < kSegments; i++)
    {
      displayBuffer[3][i] = serialBuffer[i + 1];
    }
    break;
  }
  // Set all digits according to following segment data
  case CMD_DIGIT_ALL:
  {
    for (int i = 0; i < kDigits; i++)
    {
      for (int j = 0; j < kSegments; j++)
      {
        displayBuffer[i][j] = serialBuffer[j + 1];
      }
    }
  }
  // If command does not correspond to anyone of the above, continue
  default:
  {
    break;
  }
  }
}