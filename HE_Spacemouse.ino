// This code is the combination of multiple works by others:
// 1. Original code for the Space Mushroom by Shiura on Thingiverse: https://www.thingiverse.com/thing:5739462
//    The next two from the comments on the instructables page: https://www.instructables.com/Space-Mushroom-Full-6-DOFs-Controller-for-CAD-Appl/
//    and the comments of Thingiverse: https://www.thingiverse.com/thing:5739462/comments
// 2. Code to emulate a 3DConnexion Space Mouse by jfedor: https://pastebin.com/gQxUrScV
// 3. This code was then remixed by BennyBWalker to include the above two sketches: https://pastebin.com/erhTgRBH
// 4. Four joystick remix code by fdmakara: https://www.thingiverse.com/thing:5817728
// The work by Teaching Tech involves mixing all of these. The basis is fdmakara's four joystick movement logic, with jfedor/BennyBWalker's HID SpaceMouse emulation.
// The four joystick logic sketch was setup for the joystick library instead of HID, so elements of this were omitted where not needed.
// The outputs were jumbled no matter how He plugged them in, so I spent a lot of time adding debugging code to track exactly what was happening.
// On top of this, He has added more control of speed/direction and comments/links to informative resources to try and explain what is happening in each phase.

// Spacemouse emulation
// Teaching Tech followed the instructions here from nebhead: https://gist.github.com/nebhead/c92da8f1a8b476f7c36c032a0ac2592a
// with two key differences:
// 1. He changed the word 'DaemonBite' to 'Spacemouse' in all references. 2. He changed the VID and PID values as per jfedor's instructions: vid=0x256f, pid=0xc631 (SpaceMouse Pro Wireless (cabled))
// When compiling and uploading, He select Arduino AVR boards (in Sketchbook) > Spacemouse and then the serial port.
// You will also need to download and install the 3DConnexion software: https://3dconnexion.com/us/drivers-application/3dxware-10/
// If all goes well, the 3DConnexion software will show a SpaceMouse Pro wireless when the Arduino is connected.
// *JC - John Crombie - I'd like to add a big thank you to Teaching Tech for this code. If it wasn't for this I wouldn't
// have been tempted to try to implement a Hall Effect sensor version. All my changes will be marked with a *JC comment
// All debug Serial.print statements have had the joystick output text has been renamed as HESx e.g. AX is now HSE0
/**********************************************
 * change log
 * C001 - 18-Jul-24 - Reversed default direction of Z rotation
 * C002 - 22-Jul-24 - Added state machine code to handle pressing two buttons at once to cause a third action and supress
 *                    suppress the default action of the two buttons pressed. Before I didn't supress the actions
 * C003 - 25-Jul-24 - Added define for movement3DC to switch between default 3DConnexion axis movement and Teaching Techs default movement
 * C004 - 04-Aug-24 - bug fix - Changed key reporting so that a zero report is sent when the final key is released.
 *                    Changed the place where duplicate keys reports are supressed. Used to be in the key rutine now in the report routine
 * C006 - 08-Aug-24 - bug fix - After logical button was pressed, all buttons were being sent in state 4. Now corrected.                   
 ************************************************/
 
// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID 
#include "HID.h"

// Debugging
// 0: Debugging off. Set to this once everything is working.
// 1: *JC Output raw Hall Effect Sensor values. 176 - 862 raw ADC 10-bit values. 5V ADC reference
// 2: *JC Output centered Hall Effect Sensor values. Values should be approx -424 to +424, jitter around 0 at idle. ADC reference 2.56v
// 3: *JC Output centered Hall Effect Sensor values. Filtered for deadzone. Approx -424+DEADZONE to +424-DEADZONE, locked to zero at idle. Also button values. ADC reference 2.56v
// 4: Output translation and rotation values. Approx -500 to +500 depending on the parameter. *JC ADC reference 2.56v
// 5: Output debug 3 and 4 side by side for direct cause and effect reference. *JC ADC reference 2.56v
// 6: *JC Output debug info for pseudo key state machine. ( two keys pressed at once to simulate another key press)
int debug = 0;

// Choose between 3DConnexion default movement or Teaching Tech's
// With 3DConnexion you push the joystick away from you to zoom out and towards you to zoom in.
// lifting up the joystick moves up and pushing down moves down.
// With the Teaching Tech default, these two axis are swapped so that pulling up or pushing down the knob controls zoom
// and pushing away or pulling it towards you controls up and down. I prefer this.
// set to true for 3DConnection movement.
bool movement3DC  = false;

// Direction
// Modify the direction of translation/rotation depending on preference. This can also be done per application in the 3DConnexion software.
// Switch between true/false as desired.
bool invX = false; // pan left/right
bool invY = false; // Zoom in/out or pan up/down // C003 *JC - 3DC default movement or TT default
bool invZ = true; // pan up/down or zoom in/out // C003 *JC - 3DC default movement or TT default
bool invRX = false; // Rotate around X axis (tilt front/back)
bool invRY = false; // Rotate around Y axis (tilt left/right)
bool invRZ = false; // Rotate around Z axis (twist left/right)

// Speed
// Modify to change sensitibity/speed. Default and maximum 100. Works like a percentage ie. 50 is half as fast as default. This can also be done per application in the 3DConnexion software.
int16_t speed = 80;


// Default Assembly when looking from above: *JC modified for Hall Effect Sensors (HES)
//      7 6          Y+
//       |           .
// 8 9 --+--2 3 X-...Z+...X+
//       |           .
//      0 1          Y-
//
// Wiring. Matches the first eight analogue pins of the Arduino Pro Micro (atmega32u4)
int PINLIST[8] = { // The positions of the reads *JC comments indicate which Hall Effect sensor is connected
  A0, // HES 6 o'clock left
  A1, // HES 6 o'clock right
  A2, // HES 3 o'clock near
  A3, // HES 3 o'clock far
  A6, // HES 12 o'clock right
  A7, // HES 12 o'clock left
  A8, // HES 9 o'clock far
  A9  // HES 9 o'clock near
};

// *JC added button list for digital inputs
int BTNLIST[3] = { // Button pin list
  0,
  1,
  2
};

// Deadzone to filter out unintended movements. 
// Increase if the mouse has small movements when it should be idle or the mouse is too senstive to subtle movements.
// Note that the 3d Connections also has its own deadzone processes
int DEADZONE = 40;


// This portion sets up the communication with the 3DConnexion software. The communication protocol is created here.
// hidReportDescriptor webpage can be found here: https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/ 
static const uint8_t _hidReportDescriptor[] PROGMEM = {
  0x05, 0x01,           //  Usage Page (Generic Desktop)
  0x09, 0x08,           //  0x08: Usage (Multi-Axis)
  0xa1, 0x01,           //  Collection (Application)
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x01,           //  Report ID
  0x16, 0x00, 0x80,     //logical minimum (-500)
  0x26, 0xff, 0x7f,     //logical maximum (500)
  0x36, 0x00, 0x80,     //Physical Minimum (-32768)
  0x46, 0xff, 0x7f,     //Physical Maximum (32767)
  0x09, 0x30,           //    Usage (X)
  0x09, 0x31,           //    Usage (Y)
  0x09, 0x32,           //    Usage (Z)
  0x75, 0x10,           //    Report Size (16)
  0x95, 0x03,           //    Report Count (3)
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,                 //  End Collection
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x02,           //  Report ID
  0x16, 0x00, 0x80,     //logical minimum (-500)
  0x26, 0xff, 0x7f,     //logical maximum (500)
  0x36, 0x00, 0x80,     //Physical Minimum (-32768)
  0x46, 0xff, 0x7f,     //Physical Maximum (32767)
  0x09, 0x33,           //    Usage (RX)
  0x09, 0x34,           //    Usage (RY)
  0x09, 0x35,           //    Usage (RZ)
  0x75, 0x10,           //    Report Size (16)
  0x95, 0x03,           //    Report Count (3)
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,                 //  End Collection
 
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x03,           //  Report ID
  0x15, 0x00,           //   Logical Minimum (0)
  0x25, 0x01,           //    Logical Maximum (1)
  0x75, 0x01,           //    Report Size (1)
  0x95, 32,             //    Report Count (24) // *JC - I dont undwerstand what the comment says 24 but gives a value of 32
  0x05, 0x09,           //    Usage Page (Button)
  0x19, 1,              //    Usage Minimum (Button #1)
  0x29, 32,             //    Usage Maximum (Button #24) // *JC - same comment as above
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,
  0xC0
};

// Sensors are matched to pin order.
// *JC - Note HES0 and BTN0 are not the same pin. HSE0 is Analog input 0 and BTN0 is digital input 0
#define HES0 0
#define HES1 1
#define HES2 2
#define HES3 3
#define HES6 4
#define HES7 5
#define HES8 6
#define HES9 7
#define BTN0 0
#define BTN1 1
#define BTN2 2

// Centerpoint variable to be populated during setup routine.
int centerPoints[8];

// Function to read and store analogue voltages for each joystick axis.
void readAllFromSensors(int *rawReads){
  for(int i=0; i<8; i++){
    rawReads[i] = analogRead(PINLIST[i]);
  }
}

// *JC Function to read and store button values
// When pressing two buttons at once for a different function, one button is usually pressed slightly before the other.
// To prevent the first buttons function being triggered, we wait 15ms to see if another button is pressed in the meantime.
// if so we send the pseudo button value. if not we send the first button value.
// keyState 0 - no button pressed
// keyState 1 - 1 or 3 pressed
// keystate 2 - 1&3 pressed within time limit
// keystate 3 = 1&3 not pressed within time limit
// keyState 4 = Wait until physical buttons released to reset state.
unsigned long keyTimeNew, keyTimeOld = 0;
uint8_t keyState = 0, keyPressed = 0, oldButtonValues[4] = {0,0,0,0}; // C004 - *JC - keyPresed added to keep track of last key pressed (in state machine).

void readAllFromButtons(uint8_t *buttonValues){
  for(int i=1; i<4; i++){ // read real button values
    buttonValues[i] = !digitalRead(BTNLIST[i-1]);
  }

  // C002 - *JC changed logic for handling pseudo/logical switch (two buttons pressed at once gives different function)
  buttonValues[0] = false;
  keyTimeNew = millis();
  switch(keyState) {
    case 0: // no button pressed so far
     if (buttonValues[1] || buttonValues[3]) {
       if (debug == 6) Serial.println("keyState 0 - button pressed move to keyState 1");
       keyState = 1;
       keyTimeOld = keyTimeNew;
       buttonValues[1] = buttonValues[3] = false; // don't send button values yet.
     }
     break;

     case 1: // button 1 or 3 pressed - what has happened with the elapsed time
     if (debug == 6) Serial.println("keyState 1 - one button pressed");
     if (keyTimeNew - keyTimeOld > 15) {
       keyState = 3; // second button not pressed
     } else if (buttonValues[1] && buttonValues[3]) {
       keyState = 2; // second button pressed
     }
     buttonValues[1] = buttonValues[3] = false; // don't send button values yet
     break;

     case 2: // second button pressed - set logical button
     if (debug == 6) Serial.println("keyState 2 - second button pressed - set logical button");
     buttonValues[0] = true;
     keyState = 4;
     keyPressed = 0; // C004 - *JC - record button 0 pressed
     buttonValues[1] = buttonValues[3] = false;
     break;

     case 3: // second button not pressed, send the original button
     if (debug == 6) Serial.println("keyState 3 - second button not pressed in time");
     keyState = 4;
     if (buttonValues[1]) { // C004 - *JC - record which button was pressed and will be reported
      keyPressed = 1;
     } else {
      keyPressed = 3;
     }
     break;

     case 4: //wait until buttons released to reset state
     if (debug == 6) Serial.println("keyState 4 - wait for buttons to be released before resetting state");

     if (!buttonValues[1] && !buttonValues[3]) {
       keyState = 0;   
     }
     buttonValues[0] = buttonValues[1] = buttonValues[3] = false; //C005 - *JC - bug fix. Was here before but was removed for the last release
     buttonValues[keyPressed] = true; // C004 - *JC - keep the keys pressed.

     break;
}

      
/* C004 - *JC - move supression of sending multiple key reports to report sending routine. 
// *JC - only send button value once 
  for (int i=0;i<4;i++) {
    if (buttonValues[i] == oldButtonValues[i]) {
      buttonValues[i]=0; // send only once
    } else {
      if (debug == 6) {
        Serial.print("Button "); Serial.print(i); Serial.print(" changed - Old Value ");Serial.print(oldButtonValues[i]); Serial.print(" New Value ");Serial.println(buttonValues[i]);
      }
      oldButtonValues[i] = buttonValues[i];   
    }
   }
*/
  //  based on real values set logical switch
  /*  Old code to handle logical button - doesn't supress first button pressed action
  if( buttonValues[1] && buttonValues[3]) {
    buttonValues[0] = true;
    buttonValues[1] = buttonValues[3] = false;
  }
  else
  {
    buttonValues[0] = false;
  }
  */
}

void setup() {
  // HID protocol is set.
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);
  // Begin Seral for debugging
  Serial.begin(250000);
  delay(100);
  // *JC - setup button pins for digitalRead
  for(int i=0; i<3; i++){
    pinMode(BTNLIST[i],INPUT_PULLUP);
  }
  //*JC - reduce ADC reference voltage from 5V to 2.56 if not using debug = 1
  if (debug == 1) {
    analogReference(DEFAULT);
  } else {
    analogReference(INTERNAL);
  }

  // Read idle/centre positions for Sensors.
  // *JC - First read gives unpredictable values so do it twice
  readAllFromSensors(centerPoints);
  readAllFromSensors(centerPoints);
}

uint8_t keyChange = 0; // C004 - *JC - variable to determine if new key report needs to be sent.
// Function to send translation and rotation data to the 3DConnexion software using the HID protocol outlined earlier. Two sets of data are sent: translation and then rotation.
// For each, a 16bit integer is split into two using bit shifting. The first is mangitude and the second is direction.
// *JC - Added button report
void send_command(int16_t rx, int16_t ry, int16_t rz, int16_t x, int16_t y, int16_t z, uint8_t *buttonValues) {
  uint8_t trans[6] = { x & 0xFF, x >> 8, y & 0xFF, y >> 8, z & 0xFF, z >> 8 };
  HID().SendReport(1, trans, 6);
  uint8_t rot[6] = { rx & 0xFF, rx >> 8, ry & 0xFF, ry >> 8, rz & 0xFF, rz >> 8 };
  HID().SendReport(2, rot, 6);
  // *JC - Button Report
  // these are the button functions for first byte in Fusion 360. For other functions see the GitHub repositry
  //  bit 0 - bring up configuration dialog - logical button (press BTN0 and BTN2 at the same time) rotaee 45 degrees
  //  bit 1 - fit to screen
  //  bit 2 - plan view
  //  bit 3 - no function?
  //  bit 4 - right view hide
  //  bit 5 - front view File
  //  bit 6 - no function?
  //  bit 7 - no function?
  uint8_t btn[4] ={32*buttonValues[3]+16*buttonValues[2]+4*buttonValues[1]+buttonValues[0],0,0,0};
  if (buttonValues[0]+2*buttonValues[1]+4*buttonValues[2]+8*buttonValues[3]!=keyChange) { // C004 - *JC - changed operation *JC - only send report if a button is pressed
    HID().SendReport(3,btn,4);
    keyChange = buttonValues[0]+2*buttonValues[1]+4*buttonValues[2]+8*buttonValues[3]; // C004 - *JC - record keys pressed for next time through the loop
    if (debug == 6) {Serial.print("keyChange = "); Serial.println(keyChange);} // C005 - *JC - to help debug key press issues

  }
}

void loop() {
  int rawReads[8], centered[8];
  uint8_t buttonReads[4];
  // sensor values are read. range should be 176 - 1024 for debug levels other than 1 and 88-860 for debug 1
  readAllFromSensors(rawReads);
  // button values true or false
  readAllFromButtons(buttonReads);
  // Report back 0-1023 raw ADC 10-bit values if enabled
  if(debug == 1){ 
    Serial.print("HES0:"); Serial.print(rawReads[0]); Serial.print(",");
    Serial.print("HES1:"); Serial.print(rawReads[1]); Serial.print(",");
    Serial.print("HES2:"); Serial.print(rawReads[2]); Serial.print(",");
    Serial.print("HES3:"); Serial.print(rawReads[3]); Serial.print(",");
    Serial.print("HES6:"); Serial.print(rawReads[4]); Serial.print(",");
    Serial.print("HES7:"); Serial.print(rawReads[5]); Serial.print(",");
    Serial.print("HES8:"); Serial.print(rawReads[6]); Serial.print(",");
    Serial.print("HES9:"); Serial.println(rawReads[7]);
  }

  // Subtract centre position from measured position to determine movement.
  // *JC - As we are going negative with the readings, we make them positive
  // by subtraction them from the recorded centerPoints rather than the other was around.
  // C0004 - changed back to the original TT version to match the code from AndunHH 
  for(int i=0; i<8; i++) centered[i] = centerPoints[i]-rawReads[i]; // 
  // Report centered Sensor values if enabled. Values should be approx -256 to +256, jitter around 0 at idle.
  if(debug == 2){
    Serial.print("HES0:"); Serial.print(centered[0]); Serial.print(",");
    Serial.print("HES1:"); Serial.print(centered[1]); Serial.print(",");
    Serial.print("HES2:"); Serial.print(centered[2]); Serial.print(",");
    Serial.print("HES3:"); Serial.print(centered[3]); Serial.print(",");
    Serial.print("HES6:"); Serial.print(centered[4]); Serial.print(",");
    Serial.print("HES7:"); Serial.print(centered[5]); Serial.print(",");
    Serial.print("HES8:"); Serial.print(centered[6]); Serial.print(",");
    Serial.print("HES9:"); Serial.println(centered[7]);
  }
  // Filter movement values. Set to zero if movement is below deadzone threshold.
  // *JC - Changed operation so there isn't a sudden jump when the value first falls outside deadzone
  for(int i=0; i<8; i++){
    if(centered[i]<DEADZONE && centered[i]>-DEADZONE) {
      centered[i] = 0;
    } else {
      int sgn = centered[i] / abs(centered[i]);
      centered[i] = sgn*(abs(centered[i])-DEADZONE);
    }
  }
  // Report centered Sensor values. Filtered for deadzone. Approx -500 to +500, locked to zero at idle
  if(debug == 3){
    Serial.print("HES0:"); Serial.print(centered[0]); Serial.print(",");
    Serial.print("HES1:"); Serial.print(centered[1]); Serial.print(",");
    Serial.print("HES2:"); Serial.print(centered[2]); Serial.print(",");
    Serial.print("HES3:"); Serial.print(centered[3]); Serial.print(",");
    Serial.print("HES6:"); Serial.print(centered[4]); Serial.print(",");
    Serial.print("HES7:"); Serial.print(centered[5]); Serial.print(",");
    Serial.print("HES8:"); Serial.print(centered[6]); Serial.print(",");
    Serial.print("HES9:"); Serial.print(centered[7]); Serial.print(",");
    Serial.print("But0:"); Serial.print(buttonReads[0]); Serial.print(",");
    Serial.print("But1:"); Serial.print(buttonReads[1]); Serial.print(",");
    Serial.print("But2:"); Serial.print(buttonReads[2]); Serial.print(",");
    Serial.print("But3:"); Serial.println(buttonReads[3]);  
  }

  // Doing all through arithmetic contribution by fdmakara
  // Integer has been changed to 16 bit int16_t to match what the HID protocol expects.
  int16_t transX, transY, transZ, rotX, rotY, rotZ; // Declare movement variables at 16 bit integers
  // Original fdmakara calculations
  //transX = (-centered[AX] +centered[CX])/1;
  //transY = (-centered[BX] +centered[DX])/1;
  //transZ = (-centered[AY] -centered[BY] -centered[CY] -centered[DY])/2;
  //rotX = (-centered[AY] +centered[CY])/2;
  //rotY = (+centered[BY] -centered[DY])/2;
  //rotZ = (+centered[AX] +centered[BX] +centered[CX] +centered[DX])/4;
  
  // *JC - Replaced Joystick calculations with ones for the Hall Effect Sensors
  transX = (centered[HES1]-centered[HES0]+centered[HES6]-centered[HES7])/2;  
  transY = (centered[HES2]-centered[HES3]+centered[HES9]-centered[HES8])/2;  
  transZ = (centered[HES0]+centered[HES1]+centered[HES2]+centered[HES3]+centered[HES6]+centered[HES7]+centered[HES8]+centered[HES9])/4;
  rotX = (centered[HES0]+centered[HES1]-centered[HES6]-centered[HES7])/2;
  rotY = (centered[HES8]+centered[HES9]-centered[HES2]-centered[HES3])/2;
  rotZ = (centered[HES0]+centered[HES2]+centered[HES6]+centered[HES8]-centered[HES1]-centered[HES3]-centered[HES7]-centered[HES9])/4; // C0001 *JC - changed default direction of rotation
// *JC - modified speed calculation to allow for the fact that this is integer calculations
// so do multiplications prior to divisions to maintain maximum accuracy.
  transX = (transX*speed)/100;
  transY = (transY*speed)/100;
  transZ = (transZ*speed)/100;
  rotX = (rotX*speed)/100;
  rotY = (rotY*speed)/100;
  rotZ = (rotZ*speed)/100;
// Invert directions if needed
  if(invX == true){ transX = transX*-1;};
  if(invY == true){ transY = transY*-1;};
  if(invZ == true){ transZ = transZ*-1;};
  if(invRX == true){ rotX = rotX*-1;};
  if(invRY == true){ rotY = rotY*-1;};
  if(invRZ == true){ rotZ = rotZ*-1;};

// Report translation and rotation values if enabled. Approx -800 to 800 depending on the parameter.
  if(debug == 4){
    Serial.print("TX:"); Serial.print(transX); Serial.print(",");
    Serial.print("TY:"); Serial.print(transY); Serial.print(",");
    Serial.print("TZ:"); Serial.print(transZ); Serial.print(",");
    Serial.print("RX:"); Serial.print(rotX); Serial.print(",");
    Serial.print("RY:"); Serial.print(rotY); Serial.print(",");
    Serial.print("RZ:"); Serial.println(rotZ);
  }
// Report debug 4 and 5 info side by side for direct reference if enabled. Very useful if you need to alter which inputs are used in th arithmatic above.
  if(debug == 5){
    Serial.print("HES0:"); Serial.print(centered[0]); Serial.print(",");
    Serial.print("HES1:"); Serial.print(centered[1]); Serial.print(",");
    Serial.print("HES2:"); Serial.print(centered[2]); Serial.print(",");
    Serial.print("HES3:"); Serial.print(centered[3]); Serial.print(",");
    Serial.print("HES6:"); Serial.print(centered[4]); Serial.print(",");
    Serial.print("HES7:"); Serial.print(centered[5]); Serial.print(",");
    Serial.print("HES8:"); Serial.print(centered[6]); Serial.print(",");
    Serial.print("HES9:"); Serial.print(centered[7]); Serial.print("||");
    Serial.print("TX:"); Serial.print(transX); Serial.print(",");
    Serial.print("TY:"); Serial.print(transY); Serial.print(",");
    Serial.print("TZ:"); Serial.print(transZ); Serial.print(",");
    Serial.print("RX:"); Serial.print(rotX); Serial.print(",");
    Serial.print("RY:"); Serial.print(rotY); Serial.print(",");
    Serial.print("RZ:"); Serial.println(rotZ);
  }

// Send data to the 3DConnexion software.
// The correct order for me was determined after trial and error - Teaching Tech
// *JC - Added buttons for button report
// *JC C003 Allowing swap between TT movement and 3DC movement defaults.
  if (movement3DC) {
    send_command(rotX, rotY, rotZ, transX, transY, transZ,buttonReads); // 3DC default
  }
  else {
    send_command(rotX, rotY, rotZ, transX, transZ, transY,buttonReads); // TT default
  }
}
