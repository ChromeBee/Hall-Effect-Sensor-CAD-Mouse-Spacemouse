<h2> All the files associated with the attempt to use a 3D printed wave spring with the mouse are located here.</h2>

OK. So I had to see how a 3D printed wave spring would work in the hall effect space mouse.
![Wave spring printed parts](https://github.com/user-attachments/assets/5429c0d6-7534-40a2-943a-e4261f4a24c3)

I designed a wave spring as described in the video (stacked sine waves) and mounts to hold it in the correct position in the mouse.

![wave spring assembly](https://github.com/user-attachments/assets/600e8ae4-f929-4eb7-8579-7f858d252498)
![wave spring installed](https://github.com/user-attachments/assets/1084dbcb-3558-4714-b582-f2c3e5be8e0c)

Unfortunately, the wave spring in its current iteration needs stifening (printed with 85a TPU). With the knob on, it wobbles too much to be useful. I was concerned that this may be the case. Anyway, it you want to experiment with this then the STL and design files are in the wave spring subdirectory.
UPDATE: I've printed it again in (95A TPU and it works!!)

<b>18-Aug-2025 Update</b> Ok, so I've modified the TPU wave spring design to have three wave springs and this works when printed in 85A TPU!!! It is too stiff when printed in 95A TPU.

![Triple wave spring](https://github.com/user-attachments/assets/1d17a9b8-3a66-4988-aa0c-eb9938d2bfc3)
![Triple wave spring installed](https://github.com/user-attachments/assets/ad70e04f-9e76-433a-a429-d9a688ac2d5c)

It is a bit difficult to see in the photos so I have taked a screen shot from Fusion to show it clearer.

![Tripple Wave Spring drawing](https://github.com/user-attachments/assets/47afc3cd-8896-4d5e-9ebd-baeed8c60288)

I didn't explain on the initial update that the knob was modified slightly to enlarge the mounting tabs so that they hold the top of the wave spring assembly in place. I modified the knob for the smaller case version so that meant I had to print the rest of the case as the mouse mechanism I originally used for these experiments was originally in the original sized case. So this is what it now looks like.

![Triple wave spring cased v2](https://github.com/user-attachments/assets/6c2fd217-0c8d-43fe-b5fa-61b1a7b83dd1)

The feel of the wave spring mouse is different to the normal TPU 'spring' version but closer to the original normal spring version. It is easier to move lateraly without accidentaly rotating at the same time. Is it too easy to move? That would be up to the individual. It seemed too sensitive in 123Design but I had never used a spacemouse with that program before but it is fine with Fusion and On-shape. I did turn down the sensitivity through the 3DConnexion menu.

I sized the wave spring straight from the CAD design, so the knob is at the correct height for the case. It would be relatively easy to create other sizes, if needed, and all the separate components are available in the design file to make your own.

This seems to be stretching Fusion and there are notable pauses if you change any of the variables used in the design to resize individual parts of the spring. Feel free to experiment.

<b>26-Aug-2025 Update</b> OK a bit of a redesign. Looking at the original still from the Treverse video, I thought I could create the spring using overlapping spirals and this would likely put less strain on Fusion when processing it. So I present version 2.

![spiral spring parts](https://github.com/user-attachments/assets/41c96b47-ee06-436a-b8a2-e8ad50d831b8)

The original version was a bit ackward to mount so I designed this so that the magnet plate didn't have to be mounted on the spring before the spring was mounted in the mouse body.

![spiral spring mounted](https://github.com/user-attachments/assets/af59dcd6-44c5-4bd5-8615-10907272e6ae)

Now the top of the spring can be folded and pushed throuh the magnet plate.

![Spiral spring mouse assembled](https://github.com/user-attachments/assets/faeab1bf-2f57-4e1e-8625-09703867fa1a)

A cap is placed on top of the spring and this holds the spring in place when the knob is screwed on.

![spiral spring with cap mounted](https://github.com/user-attachments/assets/5f97596c-b68e-4b94-af3d-44cf30f01043)
![spiral spring mouse from above](https://github.com/user-attachments/assets/50957b02-869c-4fa1-83f7-13027e2a2d85)

The design is parametric so if you change any of the Wave spring variables in the fusion design you can change the stiffness, length, width or other features of the spring.

I have two versions of the spring. A single spring for printing in 95A TPU and a triple spring version for 85A TPU. Print them standing vertically without support. All files are in this directory.
