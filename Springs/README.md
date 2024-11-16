<h2>Springs</h2>
I have had a large bag of 0.2 mm tension or extension springs for years. The smallest length of spring in the bag, 12mm, is what I've used for this project.
The exact same spring as I've used seems to be difficult to find at a reasonable price. Even though I don't need them, I thought I would try a couple of different sizes to see how they perform.
I bought some 0.4x12x4mm and some 0.3x10x3mm springs from Amazon UK.

![DSC05641 small](https://github.com/user-attachments/assets/b557b989-aa73-4ffe-8ec8-fdf90fde122c)

I mounted the springs on the bolt plate/magnet plate/wire holder as they would be in the completed CAD mouse but left off the sensor plate. 
In the photo the purple/pink one on the left has the 0.4x12x4mm springs, the grey one in the middle has the 0.3x10x3mm springs and the final blue one on the right is my prototype with the 0.2x12x4mm springs.
All work but the effort needed to move them varies. The 0.4x12x4mm springs, in my opinion, requires too much force to move.
The 0.3x10x3mm springs seem to require just slightly less force to the 0.4x12x4mm spring. The shorter length meaning that the spring is stretched more and offers more resistance
The 0.2x12x4mm springs are very light to move in comparison to the other two.

The strings are stretched to about 20mm in the home position so a longer spring should work and being under less tension, it should be easier to move.
However it needs to be short enough to pull the magnet plate back to the home position when the mouse knob is released. Remember that the tension can be adjusted somewhat by moving the wire holder further up or down the M4 bolt.

Another way to reduce the tension is to install a second wire holder on the bolt for the lower springs. The picture below shows an example of this. Although in the example it is just above their original position. The wire plate could be moved further up the M4 bolt.

![DSC05642 crop small](https://github.com/user-attachments/assets/21b25c33-1a8d-4b6a-8e5c-dcf800928678)

Another example with the plate moved further up the M4 bolt.
![DSC05650 small](https://github.com/user-attachments/assets/ae8c9640-dc88-4dad-a2c9-780758ed08b7)

I thought of another way of reducing the tension when using 0.3x10x3mm springs. Fewer springs.
![DSC05654 small](https://github.com/user-attachments/assets/c5f64f1b-a733-479d-b98f-a3aed2728153)

I removed alternate springs from the upper and lower sections. This seems to work very well. It still centres and moves in all directions but easier.

A search of eBay.co.uk for 0.2mm tension springs does provide listings of some suppliers of 0.2x10x3mm and 0.2x15x3mm springs. I haven't tried them but both sizes should work (in theory).

-------------
YouTube recommended a video on the IndyMower to me. https://youtu.be/UnKOp4rhu7I?si=PoE8raFwQZSphymA
In this video Nicodem Bartnik used TPU springs as part of a bumper sensor. YouTube has been recommending a lot of TPU videos to me recently so I took the hint and tried to 3D print TPU springs for the mouse. When I was going through the original thought experiments prior to starting anything, I had considered using those small rubber bands that were popular about 10 years ago for making frendship bracelets and other such items.

The TPU spring I've come up with is printed flat and has a custom Wire holder (now TPU holder).

![DSC05643 small](https://github.com/user-attachments/assets/a46e72b2-84ab-46b5-a0a6-1b11b3927232)
![DSC05644 small](https://github.com/user-attachments/assets/a6cd8de4-b3b7-4d07-8615-f3432566a3f8)
![DSC05646 small](https://github.com/user-attachments/assets/72917c7b-ed99-4669-9185-92f2293e56f8)

This "spring" has a square cross section of 2mm across. Although it works, the tension is too much to allow the mouse to move freely. So I reduced the size to 1mm cross section.
![DSC05648 small](https://github.com/user-attachments/assets/6cf38788-b0ef-4000-b1e2-63cc995bc56c)

This works much better. Certain movements are of equivalent tension to the 0.3mm springs while other movements are easier. It certainly feels usable.

I'm going to stick with my 0.2mm springs but I've put the STL files for the 1mm crossection TPU spring and the matching holder in a subdirectory here should anybody wish to try it.

<b>18-Aug-2024 Update:</b> For some reason, I can't leave this alone. I modified the design of the TPU spring and created STL files for a 0.8mm and a 0.6mm version. I have put these in the spring STL directory. I was going to leave it at that but then I decided to print them. I've tried both in one of the test prints. The 0.6mm one feels quite like the 0.2mm springs I'm using and the 0.8mm one just adds a bit more tension. I don't know if these will last long term but they are cheap to make and worth a try if you have difficulty getting the springs.

<b>16-Nov-2024 Update:</b> I think I've cracked it and now have the perfect TPU "spring" version. It might even be better than the normal spring version.
![Mechanisim and case](https://github.com/user-attachments/assets/08ad6060-fd84-4ed6-ae66-6b23b99dffa4)

This uses two TPU "springs" that are slightly longer and with thinner mounting hooks so that two of these "springs" can be used at the same time. One of the issues with the TPU springs was that a pendulum like rotatary oscillation could occur. It was simple to eliminate by twisting the two TPU hooks located above and below the magnet plate in opposite directions. This twist in the springs would stop the oscillation but also put a rotational strain on the M4 nuts which could cause them to loosen over time. This design counteracts that but applies the same principle.

<b>Additional parts needed:</b>
<li>2 prints of either of the "Thin TPU spring 0.6x16mm.stl" or "Thin TPU spring 0.6x17mm.stl" obviously printed in TPU.</li>
<li>1 print of TPU Bolt Plate printed in PLA. This is just a shorter version of the orininal bolt plate without spring mounting slots. The original can still be used.</li>
<li>2 prints of the TPU hook.</li>
<li>1 print of the magnet plate 0.3 from the original model. This is the magnet plate for 0.3mm springs.</li>
<br>
The two "springs" are mounted so that one twists the mechanisim on one direction while the second twists it in the other cancelling out the rotational strain.
I hope seeing this in pictures helps to explain what I mean.

![Parts for TPU assembly](https://github.com/user-attachments/assets/d452f351-b8ae-4aac-8be4-302a6b2049e3)


One spring is mounted as normal to the TPU hook plate.

![Added to lower bolt plate](https://github.com/user-attachments/assets/d6c378ad-6c8b-4033-816d-b11c239d6f0b)

The second one is also added to the same hook plate. I have mounted the first one with the end hooks on the bottom TPU Hook mount and the second with the centre on the mount. That is just how I did it and it doesnt really matter how it is done. I then mounted this to the bolt plate using an M4 40mm bolt and an M4 nut. <b>Note the spring is mounted so the ends are in the middle of the area that sits below the sensors.</b>

![Magnet plate added](https://github.com/user-attachments/assets/e6a8fbba-f543-4e39-bb58-59a7c58bc0b2)

To add clarity, I have added the second TPU hook plate and magnet plate at this point but it should be mounted to the arduino plate and passed through the sensor plate before these are added. The magnet plate is connected as normal to one of the TPU springs.
![Second set conneted to magnet plate](https://github.com/user-attachments/assets/a18e6fb2-1cf8-4c05-8385-6eedf9a3d5b4)

The second spring is now connected to the magnet plate but its mounts are connected to an ajacent slot to the previous spring. In my case I rotated the magnet plate slightly counter clockwise and located the spring in the next slot.

![completed mechanisim](https://github.com/user-attachments/assets/c0a5f9f5-816a-4156-b3b1-1f58324910fd)

I then did the same again with the end of this spring when mounting it to the top TPU hook, rotating it counter clockwise so the end fits in the next slot along from the one that is vertically above the slot on the magnet plate I have just fitted the middle part of the spring.

![completed mechanisim](https://github.com/user-attachments/assets/2145aaec-fbc4-4394-bfa4-a96c123b238d)

The printed washer and final M4 nut go on top. The nuts should be tighted in such a way that each magnet is centered over a sensor pair.

![Assembled device front and high](https://github.com/user-attachments/assets/0cb13960-e1b4-4d88-bf74-fa717aa7ebc7)

I have taken the idea from the build by Rares Paul on Printables and soldered the Vcc leads of each of the sensor pairs together and conected the Vcc of each pair together with a wire then repeated the same for the ground leads. While Rares Paul used hot melt glue to insulate the soldered leads, I used liquid insulation because I had some. The result, I think, looks quite neat.
