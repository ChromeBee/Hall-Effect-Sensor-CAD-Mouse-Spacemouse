# Hall-Effect-Sensor-CAD-Mouse-Spacemouse
A 6 degrees of freedom mouse for CAD programs using cheap linear hall effect sensors for position measurement

This started as a thought project based on the Teaching Tech $20 CAD mouse code. This code works with the 3D ConneXions device driver so any program that works with the 3D connecxions mouse will work with his $20 CAD mouse and also this one.
The linear hall effect sensors used are the (SS) 49e variety. They are very cheap, I bought a pack of 40 for £10. 8 are required for this project. They are arranged in 4 groups of 2. 2 of the groups are parallel to the x-axis and 2 parallel to the y-axis.
These sensors have 3 pins. VCC, GND and output. With power connected, the output should give a value of half way between VCC abd GND. i.e. if 5v is connected then with no magnent field detected the output should be 2.5v. If the north pole of a magnet approches the sensor the output should decrease upto to a minimum voltage of 0.86v while if a south pole approches then the voltage at the output should increase up to a maximum of 4.21v.
Using this, the operation of the mouse can be described in a couple of pictures.

![HSE default position](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/22768b33-1452-4082-bedd-532636bab065)
The red dots represent magnets suspended a short distance above each hall effect sensor pair. In their resting position (shown above) the sensors values are measured. 
![HSE forward position v2](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/e42b34f1-42dd-414b-b4ef-b3ecb01f2316)

If the joystick is pushed forward, the magnets move closer towards two of the hall effect sensors on opposite pairs, increasing the magnetic field that they detect and further away from the other two which will detect less.
Because we can present only one pole of the magnet, we are restricted to only half of the possible output range. Luckily the Arduino Pro Micro we are using to measure the outputs, has the ability of switching to an internal ADC reference of 2.56 volts. Therefore if we present north poles to the sensors we should expect each sensor to output between 0.86 and 2.5 volts. so we don't lose too much resolution.
So for the picture above, the sensors with the magnets moving towards them will show a decreasing voltage, while the two where the magnets are moving away from them will output an increasing voltage.
![HSE twist position](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/fbe4f562-9d27-4331-9b0a-0155fb85486c)

If we twist the joystick then diagonally opposite sensors will show a decreasing voltage as the magnet approches while their pair will show an increasing voltage as the magnet moves away.

When the joystick is tilted towards a pair of sensors then both their outputs show a decreasing voltage while the magnet on the opposite side will be moving away from the pair below it and so the sensors will output an increasing voltage.
Of course this will be the same for the other pair parallel to the other axis.

<h2>That is the operation now here is the implementation.</h2>
![2HE Spacemouse small ](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/5f6114a7-bd73-48bb-8b9a-cf5aaafbc14b)

![DSC05620](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/4f5cfa1a-4475-4a92-9e19-b41e6ca1fe40)


The mouse is made up of several layers or plates. The bottom plate holds the arduino pro micro. This needs to be wired to the sensors and optional switches. The wiring diagram is shown below.
![HE Spacemouse Wiring Diagram](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/ee5f8d90-065a-41cc-b25e-70e7bf3e9211)

I am using servo sockets to hold the hall effect sensors, however the default wiring colours for servo connectors doesn't match what is needed for the sensors. The middle pin on a servo is 5v and the connector wll normally have a red wire to this, however it is ground on the sensor. Withe the angled side face up and the legs towards you, The left most pin is VCC, the middle pin ground and the right most pin the output. I swapped the VCC and GND wires on the servo sockets to avoid confusion.

![DSC05604](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/a20550b7-fcf4-4fb4-9411-fe3bc642fd1a)

To reduce the wires connected directly to the arduino, I have joined the the VCC pins of each pair together, also the GND pins and a GND wire for a switch. Then joined the 4 wires of VCC wires from the sensor pairs together to a single wire to go to the arduino and the same with the GND wires.

![DSC05605](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/6d92b4a5-68d1-4f0a-b682-199aaa43b6e3)

These are then wires to the arduino pro micro together with the sensor and switch output wires.
The arduino is then mounted to the 3d printed Arduino plate and held in place with a short length of filament.

![DSC05610](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/98bbf02a-54d8-47da-a36f-8f45aa6195c0)

An M4x40mm bolt is then attached to the Bolt plate and this attached to the arduino plate using M3x20mm bolts

![DSC05612](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/d461cffb-a943-47fc-93a1-dc99661eed88)

4  0.2x12x4mm tension springs are attached to the bolt plate. These fit in the slots on the centre column and are held in place with short (8mm) lengths of 1mm diameter piano wire.
A second set of nuts is added to the 20mm bolts holding on the bolt plate, then the sensor plate is placed on top and the sensors slotted into place.

![DSC05614](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/c7eb104b-5b8b-47b9-bd37-40a1d4cadbbe)

Loading the software on to the arduino pro micro and setting debug level to 1, we can ensure, by bringing a magnet close to each sensor, that the sensor works, is correctly connected and which pole of the magnet is being presented to it. Once we have identified the north pole of the magnet we can glue 4 10x5x3mm magnets into the magnet holder.

![DSC05616](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/12432203-b8fb-4d10-b5e4-b7c99cad2b40)

Once the magnets are glued in we can add m3 x 4mm heat pressed brass inserts on the magnet holder and the case also the base if it is going to be used.

![DSC05607](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/f1c086c7-0fb9-44e9-837d-0f4ad9233718)

Now we need to mount the magnet plate to the mouse. It goes over the M4 bolt and the springs attached to the bolt plate have to be mounted to the slots in the magnet plate using 4 15mm lengths of 1mm diameter piano wire.

![DSC05619](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/3371f0a5-315b-421c-a8a9-8be6ced8bbc5)

4 more of the tension springs are attached to the magnet plate in the same slots. A second M4 nut is added to the M4 bolt and the wire holder is put on top. 4 more pieces of piano wire of 8mm in length and placed on the wire holder and the springs lifted and fitted on these.

![DSC05620](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/9b4a0a46-65ac-4370-9900-0315fc3dab4f)

An optional washer and M4 nut can be placed on top.

Set debug level to 2 in the code and upload it. Now test the movement of the magnet plate and verify that numbers go both up and down on all sensors.

The base is intended to be a weighted base and should be filled with something heavy and non metalic. I was thinking of cement, epoxy resin or modeling clay. 

Mount the base (if you are using it) using M3x6mm bolts and the knob using M3x8mm bolts.

![DSC05626](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/cb65caaa-9176-4a48-b3c7-6ed28052755c)

Now make sure the knob is high enough so that when the case is put on the knob is high enough to have sufficient movement is all directions. If not, adjust the top nuts on the M4 bolt.

Next put the buttons in the holes in the case and place the the case over the mechanism aligning the cutout for the USB connector with the connector on the pro micro. bolt the case in place using either M3x6mm or M3x8mm bolts.

The Cad Mouse/Spacemouse is now complete.
