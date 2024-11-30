Hall-Effect-Sensor-CAD-Mouse-Spacemouse
A 6 degrees of freedom mouse for CAD programs using cheap linear Hall Effect sensors for position measurement

![2HE Spacemouse ](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/2feb6cea-6c4b-4f79-b23b-a1e912a4b1b3)

This started as a thought project based on the Teaching Tech $20 CAD mouse code. This code works with the 3D ConneXions device driver so any program that works with the 3D connexions Spacemouse will work with his $20 CAD mouse and also this one. If it hadn't been for his project and the resulting code, I wouldn't have even considered this.

Luckily the thought project turned into a real one as I did my research.

The linear Hall Effect sensors used are the (SS) 49e variety. They are very cheap, I bought a pack of 40 for £10. 8 are required for this project. They are arranged in 4 groups of 2. 2 of the groups are parallel to the x-axis and 2 parallel to the y-axis.
These sensors have 3 pins. VCC, GND and output. With power connected, the output should give a value of half way between VCC and GND. i.e. if 5v is connected then with no magnet field detected the output should be 2.5v. If the north pole of a magnet approaches the sensor the output should decrease up to to a minimum voltage of 0.86v while if a south pole approaches then the voltage at the output should increase up to a maximum of 4.21v.
Using these sensors, the operation of the mouse can be described in a couple of pictures.

![HSE default position](https://github.com/user-attachments/assets/1f4cd16f-ca06-49b7-ac2b-b3b004b99ea6)

The red dots represent magnets suspended a short distance above each Hall Effect sensor pair. In their resting position (shown above) the sensors values are measured. Any change from these measured values represents a move of the joystick.

![HSE forward position v2](https://github.com/user-attachments/assets/830d69a4-e3ec-4239-aa2f-18654d20d580) ![HSE forward position](https://github.com/user-attachments/assets/8e460d1b-338a-4dab-aa7d-9ab180917c38)

If the joystick is pushed forward slightly, the magnets move closer towards two of the Hall Effect sensors on opposite pairs (A & C in the picture), increasing the magnetic field that they detect and further away from the other two (B & D) which will detect less.
Because we can present only one pole of the magnet, we are restricted to only half of the possible output range. I chose to have the north pole being the one closest to the sensors as the analogue to digital converter was returning a value over half its possible range so I was getting the greatest range of values this way. Although I didn't know it at the time, the Arduino Pro Micro we are using to measure the outputs, also has the ability of switching to an internal ADC reference of 2.56 volts. Therefore if we present north poles to the sensors we should expect each sensor to output between 0.86 and 2.5 volts. So we don't lose much resolution.
So for the picture above, the sensors with the magnets moving towards them will output a decreasing voltage, while the two where the magnets are moving away from them will output an increasing voltage. If we take the values for the sensors at their resting positions and subtract the current values then The resulting value will go up as the magnet approaches and down when the magnet moves away. We will call this the sensor value from now on.

Now by adding the resulting values for sensors A & C and subtracting the values for B & D we get a result proportional to the amount of movement. If the value is positive in our example above we know it is a movement towards Sensors A & C and if it is negative then it is a movement away from them.

![HSE twist position v2](https://github.com/user-attachments/assets/283c17a6-c5ca-4265-b88b-3254fcbe5a69) ![HSE twist position](https://github.com/user-attachments/assets/f9283c45-3b4b-476e-ad99-67ee85f07616)


If we twist the joystick then diagonally opposite sensors will show a decreasing voltage as the magnet approaches while the other sensor in the pair will show an increasing voltage as the magnet moves away. 

By adding the values from sensors A & D and subtract the values from B & C we get a result that is proportional to the amount of twist. In our example a positive movement number will be a twist in the clockwise direction and negative will mean an anti-clockwise twist direction.

If we apply these values to the linear movement formula, we find that the the increased value for A is cancelled out by the increased value for D and B cancels out C. So we can tell the difference between a twist and a push. The same logic applies to the other movements.

![magnet movement](https://github.com/user-attachments/assets/242753d5-30d4-4fa5-ac5f-48ff3149bf8d)

When the joystick is tilted towards a pair of sensors then both their outputs show a decreasing voltage while the magnets on the opposite side will be moving away from the pair below it and so the sensors will output an increasing voltage.
Of course, the other pair parallel to the other axis will operate in the same way for tilts, twists and linear movements in the other axis.

When we push down on the joystick, the magnets get closer to all the sensors so the voltage output drops in them all and when we pull up on the joystick, the magnets move further away from all the sensors, so the outputs will increase.

![DSC05601 v2](https://github.com/user-attachments/assets/b89e7a96-9139-462e-9681-07e234784e64)

I tried various designs before I came up with the one that I'm going to take you through below. Including ones with multiple magnets per sensor pair but that is irrelevant if you just want to build what I came up with.

<h2>That is the operation now here is the implementation.</h2>

This build is also available on Printables. There may be more build details there but there will always be more technical details here on GitHub.
https://www.printables.com/model/940040-cad-mouse-spacemouse-using-hall-effect-sensors

<h3> Parts list</h3>
<ul>
  <li>Arduino Pro Micro</li>
  <li>8 of (SS) 49e linear hall effect sensors</li>
  <li>4 of 10x5x3mm Neodymium magnets
  <li>8 of servo sockets and leads</li>
  <li>3 of 6x6x5mm small tactile switches</li>
  <li>1mm diameter piano wire cut into 8 lengths of 8mm each and 4 lengths of 15mm</li>
  <li>8 of 0.2x12x4mm tension springs ( wire diameter 0.2mm, total length 12mm, length of spring portion 4mm and OD of 4mm </li>
  <li>1 of M4x40mm bolt fully threaded.</li>
  <li>3 of M4 Nut</li>
  <li>4 of M3x20 bolts to secure the bolt plate and hold the sensor plate the correct distance from the magnet plate.</li>
  <li>12 of M3 nuts for the above bolts.</li>
  <li>10 of M3x4mm heat press brass inserts</li>
  <li>2 of M3x8mm to hold on knob</li>
  <li>4 of M3x6mm bolts to hold on base</li>
  <li>4 of M3x6mm or M3x8mm bolts to hold on case</li>
  <li>Heat Shrink tubing or Insulation tape or liquid insulation to cover wire to wire solder joints</li>
</ul>


![DSC05621](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/a798ff2c-2c87-416c-a540-b41bc3f5fb03)

The mouse is made up of several layers or plates. The bottom plate holds the Arduino pro micro. This needs to be wired to the sensors and optional switches. The wiring diagram is in it's own folder in this project directory.
![coloured render of HE Spacemouse with labels](https://github.com/user-attachments/assets/4b6e3db6-7545-4a1f-980b-1fec078da638)

I am using servo sockets to hold the Hall Effect sensors, however the default wiring colours for servo connectors doesn't match what is needed for the sensors. The middle pin on a servo is 5v and the connector will normally have a red wire to this, however it is ground on the sensor. With the angled side face up and the legs towards you, the left most pin is VCC, the middle pin ground and the right most pin the output. I swapped the VCC and GND wires on the servo sockets to avoid confusion.

![DSC05604](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/64d0b8ea-f067-4db1-8d43-c55bf9f30a4b)

To reduce the wires connected directly to the Arduino, I have joined the VCC pins of each pair together, also the GND pins and a GND wire for a switch. Then joined the 4 wires of VCC wires from the sensor pairs together to a single wire to go to the Arduino and the same with the GND wires. To make sure that the wires weren’t too long and to make things easier to wire. I attached the M4x40mm bolt to the bolt plate and the bolt plate to the sensor plate. I then mounted the sensors in their appropriate holders and held them all in place with an elastic band. This is what is shown in the picture above. The band was also useful for holding wires I didn't need at the time out of the way. 

![DSC05605](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/59f08a25-918a-4ba3-b3e9-28a88ebde7a6)

These are then wired to the Arduino pro micro together with the sensor and switch output wires.
The Arduino is then mounted to the 3d printed Arduino plate and held in place with a short length of filament.

![DSC05610](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/903f1664-1cae-4914-b5f2-e1e1495744b1)

An M4x40mm bolt is then attached to the Bolt plate and this attached to the Arduino plate using M3x20mm bolts

![DSC05612](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/53708aef-47ce-465d-ac06-29f2a0525aaf)

4 of the 0.2x12x4mm tension springs are attached to the bolt plate. These fit in the slots on the centre column and are held in place with short (8mm) lengths of 1mm diameter piano wire.
A second set of nuts is added to the 20mm bolts holding on the bolt plate, then the sensor plate is placed on top and the sensors slotted into place.

![DSC05614](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/09749bd8-3714-411c-8503-8e7d2a5fded3)

Loading the software on to the Arduino pro micro and setting debug level to 1, we can ensure, by bringing a magnet close to each sensor, that the sensor works, is correctly connected and which pole of the magnet is being presented to it. Once we have identified the north pole of the magnet we can glue 4 10x5x3mm magnets into the magnet holder. The north pole should face downward, that is towards the side of the magnet plate with a slot in the area were the magnets go.

![DSC05616](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/5cc63cbd-342c-4449-ac5e-858ab9c8cb56)

Once the magnets are glued in we can add m3 x 4mm heat pressed brass inserts on the magnet holder and the case also the base if it is going to be used. I just used a soldering iron to push these into the corresponding holes.

![location of brass inserts](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/4b53d578-0ecc-42d7-a0a6-a30b0055a89b)

The diagram above shows the location of the brass inserts.

Now we need to mount the magnet plate to the mouse. It goes over the M4 bolt and the springs attached to the bolt plate have to be mounted to the slots in the magnet plate using 4 x 15mm lengths of 1mm diameter piano wire.

![DSC05619](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/3371f0a5-315b-421c-a8a9-8be6ced8bbc5)

4 more of the tension springs are attached to the magnet plate in the same slots. A second M4 nut is added to the M4 bolt and the wire holder is put on top. 4 more pieces of piano wire of 8mm in length and placed on the wire holder and the springs lifted and fitted on these.

![DSC05620](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/2685300a-4c7e-4bb5-bd39-61e542d8551e)

An optional washer and M4 nut can be placed on top.

Set debug level to 2 in the code and upload it. Now test the movement of the magnet plate and verify that numbers go both up and down on all sensors.

The base is intended to be a weighted base and should be filled with something heavy and non metallic. I was thinking of cement, epoxy resin or modelling clay. 

Mount the base (if you are using it) using M3x6mm bolts and the knob using M3x8mm bolts.

![DSC05626](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/05070a8e-3efd-4f63-bb5b-acdf0f1b1a82)

Now make sure the knob is high enough so that when the case is put on the knob is clear of the case and there is sufficient movement in all directions. If not, adjust the top nuts on the M4 bolt.

Next put the buttons in the holes in the case and place the the case over the mechanism aligning the cut-out for the USB connector with the connector on the pro micro. Bolt the case in place using either M3x6mm or M3x8mm bolts.

The CAD Mouse/Spacemouse is now complete.

<h2>The Arduino now needs to be setup to emulate the Spacemouse.</h2>

This is the same process as the teaching Tech model and is covered on their Open Source Spacemouse model's Printables page at https://www.printables.com/model/864950-open-source-spacemouse-space-mushroom-remix

The process is also covered in the comments in the Arduino sketch. It involves setting up a custom Arduino board that the 3D connexion software recognises as a Spacemouse Pro Wireless.

The 3D connexions driver is fairly flexible on the range of values the mouse sends to it for it to work. To have the most flexible range, both in positiva and negative directions, with debug set to 1, and analogRference(DEFAULT) I.e. 5 volts, the outputs should be reading around the value of 342. If the analogReference(INTERNAL) is set then, the mid range value should be around 684. You can adjust the distance of the sensor plate from the magnet plate to adjust this. Each corner can be adjusted independently.

<b>24-Nov-2024 Update</b>
I have found that with the magnets I'm using that a distance between the magnet plate and the sensors of just under 4mm works best. I have a small dowel stick that I've used as a pointer in some videos and its diameter is just under 4mm. If it just about fits, with a push, between the magnet plate and sensor plate then I'm about at the correct distance.

If the mouse works for tilting and twisting but not lateral movements of left/right and forward/backward, then the gap is too large and should be reduced. 

I have also added the TPU cap keeper that I added to Printables a few days ago. This, printed in TPU, fits on the knob cap and holds it to the knob, if loose. It wasn't needed for my first build but a subsequent print using a differet brand of filament, I found the cap was loose. This solves that problem.
<b>End-of-update</b>

This is what I've been able to discover that the button report can contain. The Spacemouse pro wireless supports 18 programmable buttons although the code indicates more. There are 4 bytes in the report and depending on what bit is set to 1 it causes the following to happen.
<TABLE BORDER="1">
<TR><B><TD>Bit Number</TD><TD>Byte 0</TD><TD>Byte 1</TD><TD>Byte 2</TD><TD>Byte 3</TD></B></TR>
<TR><B><TD>bit 0</TD></B><TD>Configuration Screen</TD><TD>Rotate 90 degrees</TD><TD>-----</TD><TD>Shift key</TD></TR>
<TR><B><TD>bit 1</TD></B><TD>Fit to screen</TD><TD>-----</TD><TD>-----</TD><TD>CTRL key</TD></TR>
<TR><B><TD>bit 2</TD></B><TD>Plan View</TD><TD>-----</TD><TD>-----</TD><TD>TAB key/Disable rotate</TD></TR>
<TR><B><TD>bit 3</TD></B><TD>-----</TD><TD>-----</TD><TD>-----</TD><TD>-----</TD></TR>
<TR><B><TD>bit 4</TD></B><TD>Right view</TD><TD>Hide design Browser</TD><TD>-----</TD><TD>-----</TD></TR>
<TR><B><TD>bit 5</TD></B><TD>Front view</TD><TD>Open File Explorer</TD><TD>-----</TD><TD>-----</TD></TR>
<TR><B><TD>bit 6</TD></B><TD>-----</TD><TD>Internet</TD><TD>ESC key</TD><TD>-----</TD></TR>
<TR><B><TD>bit 7</TD></B><TD>-----</TD><TD>Copy key</TD><TD>ALT key</TD><TD>-----</TD></TR>
</TABLE>

Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
