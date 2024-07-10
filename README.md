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

That is the operation now here is the implementation.
![2HE Spacemouse small ](https://github.com/ChromeBee/Hall-Effect-Sensor-CAD-Mouse-Spacemouse/assets/141455861/5f6114a7-bd73-48bb-8b9a-cf5aaafbc14b)



