
/*
   "Azeron clone" Game Pad controller
   A custom made keyboard/joystick controller using HID on Arduino Pro Micro
   https://www.instructables.com/id/Azeron-Game-Pad-DIY-Under-35/
   JerkWagon & Anon Engineering @Discord 9/2020
   Rev. 091620vs
   Forked by Zenduin December 11th 2022
   * added simplified 8-way code for joystick
   * tweaked pinout to match wiring diagram (unless I wired mine wrong)
*/

//#define DEBUG // Comment out for Keyboard, uncomment for Serial debugging

#ifdef DEBUG
#define KEY_BEGIN()
#define KEY_PRESS(x)
#define KEY_RELEASE(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
// Key constants, numeric values used in DEBUG mode
#define KEY_LEFT_CTRL 128
#define KEY_LEFT_SHIFT 129
#define KEY_LEFT_ALT 130
#define KEY_LEFT_GUI 131
#define KEY_RIGHT_CTRL 132
#define KEY_RIGHT_SHIFT 133
#define KEY_RIGHT_ALT 134
#define KEY_RIGHT_GUI 135
#define KEY_UP_ARROW 218
#define KEY_DOWN_ARROW 217
#define KEY_LEFT_ARROW 216
#define KEY_RIGHT_ARROW 215
#define KEY_BACKSPACE 178
#define KEY_TAB 179
#define KEY_RETURN 176
#define KEY_ESC 177
#define KEY_INSERT 209
#define KEY_DELETE 212
#define KEY_PAGE_UP 211
#define KEY_PAGE_DOWN 214
#define KEY_HOME 210
#define KEY_END 213
#define KEY_CAPS_LOCK 193
#define KEY_F1 194
#define KEY_F2 195
#define KEY_F3 196
#define KEY_F4 197
#define KEY_F5 198
#define KEY_F6 199
#define KEY_F7 200
#define KEY_F8 201
#define KEY_F9 202
#define KEY_F10 203
#define KEY_F11 204
#define KEY_F12 205
#else
#include <Keyboard.h>
#define KEY_BEGIN() Keyboard.begin()
#define KEY_PRESS(x) Keyboard.press(x)
#define KEY_RELEASE(x) Keyboard.release(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

//Globals
const int xCenter = 250;    // Tweak for your D-pad's center values, should be 250
const int yCenter = 250;
const int xDeadband = 100;  // Sets the size of the center deadband
const int yDeadband = 100;
double lastPosX = 0.0;
double lastPosY = 0.0;

// D-pad: UP = w, RIGHT = d, DOWN = s, LEFT = a, DPadNone is "center value"
const char dPadUp = 'e';
const char dPadRt = 'w';
const char dPadDn = 'q';
const char dPadLt = 's';

typedef struct {
  int pin;
  char cmd;
  bool wasPressed;
} dKey;

// Switch pins, {PIN, KEY, False}
dKey dSwitch[] = {
  {4, '7', false}, //near little finger  
  {5, '8', false}, // near ring finger
  {8, '9', false}, // near middle finger
  {15, '0', false}, // near forefinger
  {3, '2', false}, // mid little finger
  {6, '6', false}, // mid ring finger
  {9, '-', false}, // mid middle finger
  {14, '=', false}, // mid forefinger
  {2, '1', false}, // far little finger
  {7, '3', false}, // far ring finger  
  {10, '4', false}, // far middle finger  
  {16, '5', false}, //far forefinger
  {21, ' ', false}, // Pin 21 = A3 // Joystick btn
};

void setup() {
  Serial.begin(9600);
  KEY_BEGIN();
  // Pin Mode setup
  for (int i = 0; i <= 12; i++) {
    pinMode(dSwitch[i].pin, INPUT_PULLUP);
  }
  // Shows key mapping if in DEBUG
  #ifdef DEBUG
  while (!Serial);  // Wait for serial to come up
  DEBUG_PRINTLN("Key mapping: ");
  DEBUG_PRINT("Pin\t");
  DEBUG_PRINTLN("Key");
  for (int i = 0; i <= 12; i++) {
    DEBUG_PRINT(dSwitch[i].pin);
    DEBUG_PRINT('\t');
    DEBUG_PRINTLN(dSwitch[i].cmd);
  }
  DEBUG_PRINTLN();
  #endif
}

void loop() {
  readJoystick();
  readKeys();
}

void readJoystick() {
  int joyX = analogRead(20);  // Or A3, rotates 0 angle (0 degrees is full right by default)
  int joyY = analogRead(19);  // Or A2, rotates 0 angle
  double mapJoyX = map(joyX, 0, 1023, 0, 500);
  double mapJoyY = map(joyY, 0, 1023, 0, 500);
  // For test, use to determine xCenter, yCenter
  //DEBUG_PRINT("Mapped X value: ");
  //DEBUG_PRINTLN(mapJoyX);
  //DEBUG_PRINT("Mapped Y value: ");
  //DEBUG_PRINTLN(mapJoyY);
  if (mapJoyX <= xCenter + xDeadband && mapJoyX >= xCenter - xDeadband) {
    if (lastPosX >= xCenter + xDeadband) {
      KEY_RELEASE(dPadRt);      
    } else if (lastPosX <= xCenter - xDeadband) {
      KEY_RELEASE(dPadLt);
    }
  } else {
    if (mapJoyX >= xCenter + xDeadband) { // Right
      if (lastPosX <= xCenter - xDeadband) KEY_RELEASE(dPadLt); // Was Left
      if (lastPosX <= xCenter + xDeadband) KEY_PRESS(dPadRt); // Was Center
    } else if (mapJoyX <= xCenter - xDeadband) { // Left
      if (lastPosX >= xCenter + xDeadband) KEY_RELEASE(dPadRt); // Was Left
      if (lastPosX >= xCenter - xDeadband) KEY_PRESS(dPadLt); // Was Center
    }
  }
  if (mapJoyY <= yCenter + yDeadband && mapJoyY >= yCenter - yDeadband) {
    if (lastPosY >= yCenter + yDeadband) {
      KEY_RELEASE(dPadUp);      
    } else if (lastPosY <= yCenter - yDeadband) {
      KEY_RELEASE(dPadDn);
    }
  } else {
    if (mapJoyY >= yCenter + yDeadband) { // Right
      if (lastPosY <= yCenter - yDeadband) KEY_RELEASE(dPadDn); // Was Left
      if (lastPosY <= yCenter + yDeadband) KEY_PRESS(dPadUp); // Was Center
    } else if (mapJoyY <= yCenter - yDeadband) { // Left
      if (lastPosY >= yCenter + yDeadband) KEY_RELEASE(dPadUp); // Was Left
      if (lastPosY >= yCenter - yDeadband) KEY_PRESS(dPadDn); // Was Center
    }
  }
  lastPosX = mapJoyX;
  lastPosY = mapJoyY;
}

void readKeys() {
  for (int i = 0; i <= 12; i++)  {
    if (digitalRead(dSwitch[i].pin) == LOW) {
      if (dSwitch[i].wasPressed == false)  {
        KEY_PRESS(dSwitch[i].cmd)
        DEBUG_PRINT("Key press:\t");
        DEBUG_PRINT(dSwitch[i].pin);
        DEBUG_PRINT('\t');
        DEBUG_PRINTLN (dSwitch[i].cmd);
        dSwitch[i].wasPressed = true;
      }
    }
    else  {
      if (dSwitch[i].wasPressed == true)  {
        KEY_RELEASE(dSwitch[i].cmd);
        DEBUG_PRINT("Key released:\t");
        DEBUG_PRINT(dSwitch[i].pin);
        DEBUG_PRINT('\t');
        DEBUG_PRINTLN (dSwitch[i].cmd);
        dSwitch[i].wasPressed = false;
      }
    }
  }
}
