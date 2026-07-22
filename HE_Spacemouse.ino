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
 *                    Changed the place where duplicate keys reports are supressed. Used to be in the key routine now in the report routine
 * C006 - 08-Aug-24 - bug fix - After logical button was pressed, all buttons were being sent in state 4. Now corrected. 
 * C007 - 07-Aug-25 - Adding two more pseudo buttons. Achieved by pressing front button at the same time as one of the side buttons.
 *                    These give TAB/Rotate lock (Left and fromt button) and Fit to screen (Right and front button) by default
 * C008 - 09-Aug-25 - Remove Speed adjustment left over from TT code - This can be controlled through 3DConections configuration menu.
 * C009 - 12-Aug-25 - Changed centre button to cycle through three views if enabled with include_cycleViews being defined
 * C010 - 28-Feb-26 - Added code section by Jonas Edvinsson and added define to include it #define include_C010 comment this out to exclude it
 * C011 - 11-May-26 - Altered the weights of the various movements.
 * C012 - 17-May-26 - All sensor values change with any movement. For a lateral movement, say moveing the knob left, the main sensors are 0 and 7
 *                    where the magnet moves over them and 1 and 6 where the magnet moves off them, but the magnets also move off sensors 2, 3, 8 and 9,
 *                    giving a change in reading that contributes to the lateral reading. This change includes these sensor contributions to the movements.
 * C013 - 11-Jul-26 - Auto update center position, if drift detected. If the mouse sensor readings remain constant (within a set permitted variation) but
 *                    different from the current recorded center position by more than the same margin for a set amount of time then update the
 *                    recorded center position.
 * C014 - 16-Jul-26 - Added sensitivity selector. Press the right and front mouse buttons simultaniously to step between full, half and quarter sensitivities
 *                    This operates by stepping through 3 different weightDivisor values. This change was made possible by changes for C011
 * 
 ************************************************/
 
// Include inbuilt Arduino HID library by NicoHood: https://github.com/NicoHood/HID 
#include "HID.h"

// Define to include C010 code by Jonas Edvinsson. Calibrate the mouse first before including it.
// This change ignores small movement changes when there is a large one.
// Comment out the define to exclude it.
#define include_IgnoreSmallChanges

// Define to include C012 - just in case it causes problems
// This change uses all 8 hall effect sensors to calculate all movements.
// comment out the define below to exclude the change
#define include_UseAllSensorsForAllMovement

// Debugging
// 0: Debugging off. Set to this once everything is working.
// 1: *JC Output raw Hall Effect Sensor values. 176 - 862 raw ADC 10-bit values. 5V ADC reference
// 2: *JC Output centered Hall Effect Sensor values. Values should be approx -424 to +424, jitter around 0 at idle. ADC reference 2.56v
// 3: *JC Output centered Hall Effect Sensor values. Filtered for deadzone. Approx -424+DEADZONE to +424-DEADZONE, locked to zero at idle. Also button values. ADC reference 2.56v
// 4: Output translation and rotation values. Approx -500 to +500 depending on the parameter. *JC ADC reference 2.56v
// 5: Output debug 3 and 4 side by side for direct cause and effect reference. *JC ADC reference 2.56v
// 6: *JC Output debug info for pseudo key state machine. ( two keys pressed at once to simulate another key press)
// 7: *JC output debug messages for Auto recenter code
// 8: *JC output debug messages for sensitivity adjustment
int debug = 0;

// Choose between 3DConnexion default movement or Teaching Tech's
// With 3DConnexion you push the joystick away from you to zoom out and towards you to zoom in.
// lifting up the joystick moves up and pushing down moves down.
// With the Teaching Tech default, these two axis are swapped so that pulling up or pushing down the knob controls zoom
// and pushing away or pulling it towards you controls up and down. I prefer this.
// set to true for 3DConnection movement.
bool movement3DC  = false;

// C013 - detect drift of default home position and reset it 
#define include_AutoRecenter  // comment this line out to remove auto recentering code

#ifdef include_AutoRecenter
// if we get sensor readings that are above the variation threshold of the center point (for 1 sensor or more) and we get continous
// sensor readings within the variation threshold of this value for a set time then we assume this is our new center point
// and reset the stored centerPoints to these new values.
const int variationThreshold = 7; // Amount of drift permitted from the current centerPoints and the variations allowed in the new values
const unsigned long stableTime = 1000; // time readings have to be within the variation threshold in milliseconds to cause the new readings to be accepted as teh new centerPoints
// internal global variables for C013
bool greaterThanThreshold = false; // does the current values exceed the variation threshold
unsigned long startTime; // if so start a timer
bool withinBounds,firstTime=true; // Are the new values within the variation threshold. first time outside the threshold start timer
int oldCentered[8]; // what were the centered sensor value the last time through the loop
#endif
// end C013 setup

// switch between two modes of operation. The original mapping of buttons including pushing two at once or an alternative mapping where
// the front button pretends to be three different buttons mapping to three views.
#define include_cycleViews
uint8_t cycleInitialButton = 0; // Added to lowest pseudo button value
const uint8_t buttonDelay = 20; // 20ms wait time for second button to be pressed

// Direction
// Modify the direction of translation/rotation depending on preference. This can also be done per application in the 3DConnexion software.
// Switch between true/false as desired.
bool invX = false; // pan left/right
bool invY = false; // Zoom in/out or pan up/down // C003 *JC - 3DC default movement or TT default
bool invZ = true; // pan up/down or zoom in/out // C003 *JC - 3DC default movement or TT default
bool invRX = false; // Rotate around X axis (tilt front/back)
bool invRY = false; // Rotate around Y axis (tilt left/right)
bool invRZ = false; // Rotate around Z axis (twist left/right)

// C011 - different weights to different movements
// This change is to give more prominence to certain movements.
// For example it is more diffcult to move laterally than to zoom
// so to try and even things out we can give lateral movements a bigger
// weight than the zoom movement.
// This works by multiplying the actual movement by the weight applied 
// and dividing by the weightDivisor.
// NOTE : Integer arritmetic is being used so divide rounds down
const int  transXWeight = 7; // change to 6 for previous operational feel
const int  transYWeight = 7; // change to 6 for previous operational feel
const int  transZWeight = 3;  // change to 3 for previous operational feel
const int  rotXWeight = 6; // change to 6 for previous operational feel
const int  rotYWeight = 6; // change to 6 for previous operational feel
const int  rotZWeight = 4; // change to 3 for previous operational feel
int  weightDivisor = 12; // C014 removed the const

// C014 - Variable weightDivisor - Sensitivity adjusment
#define include_SensitivityAdjustment   // comment out to exclude C014 change and use press of right and front button together as normel button
const int maxNumWeightDivisors = 3; // number of values in array below
const int weightDivisorSelection[] = {12,24,48};
int currentDivisorIndex = 0;



// Speed
// Modify to change sensitibity/speed. Default and maximum 100. Works like a percentage ie. 50 is half as fast as default. This can also be done per application in the 3DConnexion software.
// int16_t speed = 80; C008 - remove this as it can be controlled through 3dConnection software


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

//-------------------------------------------------------------------------
// Function to read and store analogue voltages for each joystick axis.
void readAllFromSensors(int *rawReads){
  for(int i=0; i<8; i++){
    rawReads[i] = analogRead(PINLIST[i]);
  }
}

//-------------------------------------------------------------------------
// *JC Function to read and store button values
// When pressing two buttons at once for a different function, one button is usually pressed slightly before the other.
// To prevent the first buttons function being triggered, we wait 15ms to see if another button is pressed in the meantime.
// if so we send the pseudo button value. if not we send the first button value.
// keyState 0 - no button pressed
// keyState 1 - 1 or 3 pressed
// keystate 2 - 1&3 pressed within time limit
// keystate 3 = 1&3 not pressed within time limit
// keyState 4 = Wait until physical buttons released to reset state.
// C007 changed logic from above now waits for any two buttons to be pressed and keyState 5 is called when button 1 & 2 are pressed together and keyState 6 when buttons 2 & 3 are pressed
// keystate 5 - 1&2 pressed within time limit
// keystate 6 - 2&3 pressed within time limit
// C014 added code to intercept one of the pseudo buttons and adjust the sensitivity of the mouse
unsigned long keyTimeNew, keyTimeOld = 0;
uint8_t keyState = 0, keyPressed = 0; // C004 - *JC - keyPressed added to keep track of last key pressed (in state machine).
// uint8_t oldButtonValues[6] = {0,0,0,0}; no longer used with state machine

void readAllFromButtons(uint8_t *buttonValues){

  for(int i=1; i<4; i++){ // read real button values
    buttonValues[i] = !digitalRead(BTNLIST[i-1]);
  }

  // C002 - *JC changed logic for handling pseudo/logical switch (two buttons pressed at once gives different function)
  // C007 - *JC added entries 4 and 5 for new pseudo buttons. 0 is the existing one.
  // C009 - *JC if include_cycleViews is defined then middle button (2) will set pseudo buttons 6, 7 and 8
  buttonValues[0] = buttonValues[4] = buttonValues[5] = buttonValues[6] = buttonValues[7] = buttonValues[8] = false; // all pseudo buttons set to false
  keyTimeNew = millis();
  switch(keyState) {
    case 0: // no button pressed so far
     if (buttonValues[1] || buttonValues[3] || buttonValues[2]) { // C007 - *JC - added buttonValues[2]
       if (debug == 6) Serial.println("keyState 0 - button pressed move to keyState 1");
       keyState = 1;
       keyTimeOld = keyTimeNew;
       buttonValues[1] = buttonValues[3] = buttonValues[2] = false; // don't send button values yet. C007 - *JC added ButtonValues 2 to the list
     }
     break;

     case 1: // button 1 or 3 pressed - what has happened with the elapsed time
     if (debug == 6) Serial.println("keyState 1 - one button pressed");
     if (keyTimeNew - keyTimeOld > buttonDelay) { // C007  - changed the waiting time from 15 to 20 as 15 seemed to short for ackward double button presses C009 changed number to a constant defined elsewhere
       keyState = 3; // second button not pressed
     } else if (buttonValues[1] && buttonValues[3]) {
       keyState = 2; // second button pressed pseudo button 1
     } else if (buttonValues[1] && buttonValues[2]) { // start of C007 changes - this introduces 2 new states for the two new pseudo buttons
       keyState = 5; // second button pressed pseudo button 2
     } else if (buttonValues[2] && buttonValues[3]) {
       keyState = 6; // second button pressed pseudo button 3
     } // end of C007 changes
    
     buttonValues[1] = buttonValues[3] = buttonValues[2] = false; // don't send button values yet - C007 - *JC added buttonValues[2] to the list
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
     } else if (buttonValues[2]) { // C007 - *JC - added extra button to possible two button presses
     // C009 - *JC - if include_cycleViews is defined then button 2 will set one of three pseudo buttons
     //              that will then be used to display one of three views on rotation
       #ifdef include_cycleViews
         buttonValues[6+cycleInitialButton] = true;
         keyPressed = 6+cycleInitialButton;
         cycleInitialButton = (cycleInitialButton+1)%3;
         if (debug == 6) {Serial.print("cycleInitialButton = "); Serial.println(keyPressed);}
       #else
         keyPressed = 2;
       #endif
     } else {
      keyPressed = 3;
     }
     break;

     case 4: //wait until buttons released to reset state
     //if (debug == 6) Serial.println("keyState 4 - wait for buttons to be released before resetting state");

     if (!buttonValues[1] && !buttonValues[3] && !buttonValues[2]) { // C007 - *JC added buttonValues[2]
       keyState = 0;   
     }
     buttonValues[0] = buttonValues[1] = buttonValues[3] = buttonValues[2] = buttonValues[4] = buttonValues[5] = false; //C005 - *JC - bug fix. Was here before but was removed for the last release. C007 added extra pseudo buttons
     buttonValues[keyPressed] = true; // C004 - *JC - keep the keys pressed.

     break;

     case 5: // C007 -*JC - second pseudo button
     if (debug == 6) Serial.println("keyState 5 - second button pressed - set logical button");
     buttonValues[4] = true;
     keyState = 4;
     keyPressed = 4; // C004 - *JC - record button 0 pressed
     buttonValues[1] = buttonValues[2] = buttonValues[3] = false;
     break;

    case 6 :  // C007 - *JC - third pseudo button
     if (debug == 6) Serial.println("keyState 6 - second button pressed - set logical button");
     buttonValues[5] = true;
     keyState = 4;
     keyPressed = 5; // C004 - *JC - record button 0 pressed
     buttonValues[1] = buttonValues[2] = buttonValues[3] = false;
     // C014
     #ifdef include_SensitivityAdjustment
        currentDivisorIndex = (currentDivisorIndex+1)%maxNumWeightDivisors; // calculate new index value (step through values)
        weightDivisor =  weightDivisorSelection[currentDivisorIndex]; // select related sensitivity divisor
        if (debug == 8) {Serial.print("Current weightDivisor = ");Serial.println(weightDivisor);}
     #endif  // include_SensitivityAdjustment end C014
     break;

  }
}

      

//-------------------------------------------------------------------------
void setup() {
  // HID protocol is set.
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);
  // Begin Seral for debugging
  Serial.begin(9600);
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
  delay(1000);
  // Read idle/centre positions for Sensors.
  // *JC - First read gives unpredictable values so do it twice
  readAllFromSensors(centerPoints);
  readAllFromSensors(centerPoints);
 
}

//-------------------------------------------------------------------------
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



  uint8_t btn[4] ={32*buttonValues[3]+16*buttonValues[2]+4*buttonValues[1]+buttonValues[0]+2*buttonValues[5],0,0,4*buttonValues[4]}; // C007 added 2nd Pseudo button as Fit to Screen

  #ifdef include_cycleViews // C009 use pseudo buttons to select views - button 2 controls which view is selected.
    btn[0] = 32*buttonValues[6]+16*buttonValues[7]+4*buttonValues[8]+buttonValues[0]+2*buttonValues[1];
    btn[1] = buttonValues[4]+16*buttonValues[5];
    btn[3] = 4*buttonValues[3];
  #endif
    if (buttonValues[0]+2*buttonValues[1]+4*buttonValues[2]+8*buttonValues[3]+16*buttonValues[4]+32*buttonValues[5]+64*buttonValues[6]+128*buttonValues[7]+256*buttonValues[8]!=keyChange) {  // // C004 - *JC - changed operation *JC - only send report if a button is pressed C007 added new pseudo buttons to check
    if (debug == 6) {Serial.print("btn[0] = ");Serial.print(btn[0]);Serial.print(" btn[1] = ");Serial.print(btn[1]);Serial.print(" btn[2] = ");Serial.print(btn[2]);Serial.print(" btn[3] = ");Serial.println(btn[3]); }
    keyChange = buttonValues[0]+2*buttonValues[1]+4*buttonValues[2]+8*buttonValues[3]+16*buttonValues[4]+32*buttonValues[5]+64*buttonValues[6]+128*buttonValues[7]+256*buttonValues[8]; // C004 - *JC - record keys pressed for next time through the loop C007 added new pseudo buttons to keychange value
    if (debug == 6) {Serial.print("keyChange = "); Serial.println(keyChange);} // C005 - *JC - to help debug key press issues
  //C014

  #ifdef include_SensitivityAdjustment
    if (!buttonValues[5]) {
  #endif
    HID().SendReport(3,btn,4);
  #ifdef include_SensitivityAdjustment
    }
  #endif
  // end C014
  }  // end key changed
}

//-------------------------------------------------------------------------
void loop() {
  int rawReads[8], centered[8];
  uint8_t buttonReads[9]; // C007 - *JC added two more values for two extra pseudo buttons C009 added another 3 pseudo switches to cycle views when button 2 (front) pressed

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

  // C013 -- cneck if mouse in home position but not centered
  #ifdef include_AutoRecenter
  if( greaterThanThreshold) { // always false the first time through. this indicates a possible change in the center position or user movement
    withinBounds=true;
    for(int i=0;i<8;i++) { // check if any sensor reading is outside the permitted threshold for adjusting the center i.e. this is user movement
      if(abs(centered[i] -oldCentered[i])>variationThreshold) {
        withinBounds = false;
        if (debug==7) {Serial.print("Sensor ");Serial.print(i);Serial.print(" Centered = ");Serial.print(centered[i]);Serial.print(" oldCentered = ");Serial.print(oldCentered[i]);Serial.print(" Variation = ");Serial.println(abs(centered[i] -oldCentered[i]));}
      }
    }
    if (withinBounds) {
      // possible new center position. check if the values have been stable for sufficient time
      //Serial.println("withinBounds");
      if ((millis() - startTime)>stableTime)
        {
          readAllFromSensors(centerPoints);
          if (debug == 7) Serial.println("reset centerPoints");
          greaterThanThreshold = false;
          firstTime = true;
        }
    } else {
      //no change or user movement
      if (debug == 7) Serial.println("no change");
      greaterThanThreshold = false;
      firstTime = true;
    }
   }

// check if we have moved from the last center position (user movement or center drift)
   for(int i=0;i<8;i++){
    if((!greaterThanThreshold) && (abs(centered[i])>variationThreshold))  greaterThanThreshold = true; // if any user movement is detected then it is not drift
    }

// if there is movement detected and this is the first time it has been detected then remember this position and start timer.
  if ((greaterThanThreshold) && (firstTime)) {
    for(int i=0;i<8;i++){
      oldCentered[i]=centered[i];  // if oldCentered Changes we need to readjust it
    }
    firstTime = false;
    startTime = millis();
  }
#endif // include_AutoRecenter
  // end of C013

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
  
  // *JC - Replaced Joystick calculations with ones for the Hall Effect Sensors
  // C011 replaced the divide by 2 on shorter equasions and divide by 4 on longer ones with a configurable weight calculation
  // C012 All movement calvulations now include all sensors even if their impact on the values returned will be small
  #ifdef include_UseAllSensorsForAllMovement
    transX = (centered[HES1]-centered[HES0]+centered[HES6]-centered[HES7]+centered[HES2]+centered[HES3]+centered[HES9]+centered[HES8])*transXWeight/weightDivisor;  //C012
    transY = (centered[HES2]-centered[HES3]+centered[HES9]-centered[HES8]+centered[HES1]+centered[HES0]+centered[HES6]+centered[HES7])*transYWeight/weightDivisor;  //C012
  #else
    transX = (centered[HES1]-centered[HES0]+centered[HES6]-centered[HES7])*transXWeight/weightDivisor;  // pre C012
    transY = (centered[HES2]-centered[HES3]+centered[HES9]-centered[HES8])*transYWeight/weightDivisor;  //pre C012
  #endif 
  transZ = (centered[HES0]+centered[HES1]+centered[HES2]+centered[HES3]+centered[HES6]+centered[HES7]+centered[HES8]+centered[HES9])*transZWeight/weightDivisor;
  #ifdef include_UseAllSensorsForAllMovement
    rotX = (centered[HES0]+centered[HES1]-centered[HES6]-centered[HES7]+centered[HES2]-centered[HES3]+centered[HES9]-centered[HES8])*rotXWeight/weightDivisor;  // C012
    rotY = (centered[HES8]+centered[HES9]-centered[HES2]-centered[HES3]-centered[HES1]+centered[HES0]-centered[HES6]+centered[HES7])*rotYWeight/weightDivisor;  // C012
  #else
    rotX = (centered[HES0]+centered[HES1]-centered[HES6]-centered[HES7])*rotXWeight/weightDivisor;  // pre C012
    rotY = (centered[HES8]+centered[HES9]-centered[HES2]-centered[HES3])*rotYWeight/weightDivisor;  // pre C012
  #endif
  rotZ = (centered[HES0]+centered[HES2]+centered[HES6]+centered[HES8]-centered[HES1]-centered[HES3]-centered[HES7]-centered[HES9])*rotZWeight/weightDivisor; // C001 *JC - changed default direction of rotation

  #ifdef include_IgnoreSmallChanges 
  // This section by Jonas Edvinsson
  // C010... (After existing code where transX, transY, transZ, rotX, rotY, rotZ are calculated) ...

  // 1. Settings
  // How strong does the "noise" have to be relative to the main movement to be kept?
  // 0.30 means: "If an axis is less than 30% of the strongest force, kill it."
  // Higher number (0.50) = Stricter, feels more robotic (good for CAD views).
  // Lower number (0.15) = Looser, allows more fluid diagonal movement.
  float noiseRatio = 0.35;
  // 2. Find the strongest movement (The "Dominant" Axis)
  int16_t maxVal = 0;
  if (abs(transX) > maxVal) maxVal = abs(transX);
  if (abs(transY) > maxVal) maxVal = abs(transY);
  if (abs(transZ) > maxVal) maxVal = abs(transZ);
  if (abs(rotX) > maxVal) maxVal = abs(rotX);
  if (abs(rotY) > maxVal) maxVal = abs(rotY);
  if (abs(rotZ) > maxVal) maxVal = abs(rotZ);
  // 3. Filter out the weak "noise" movements
  // We calculate a dynamic threshold based on your current hardest press.
  int16_t dynamicThreshold = maxVal * noiseRatio;
  if (abs(transX) < dynamicThreshold) transX = 0;
  if (abs(transY) < dynamicThreshold) transY = 0;
  if (abs(transZ) < dynamicThreshold) transZ = 0;
  if (abs(rotX) < dynamicThreshold) rotX = 0;
  if (abs(rotY) < dynamicThreshold) rotY = 0;
  if (abs(rotZ) < dynamicThreshold) rotZ = 0;

  // end C010... (Before existing code for "Invert directions" and "send_command"
  #endif // include_IgnoreSmallChanges - end of C010 change

// *JC - modified speed calculation to allow for the fact that this is integer calculations
// so do multiplications prior to divisions to maintain maximum accuracy.
// C008 now removed
/*  transX = (transX*speed)/100;
  transY = (transY*speed)/100;
  transZ = (transZ*speed)/100;
  rotX = (rotX*speed)/100;
  rotY = (rotY*speed)/100;
  rotZ = (rotZ*speed)/100; */

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
