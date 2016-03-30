Subject 	: CSCI420 - Computer Graphics 
Assignment 2: Simulating a Roller Coaster
Author		: Vivek Raj Saxena
USC ID 		: 6834311974

Description: In this assignment, we use Catmull-Rom splines along with OpenGL texture mapping to create a roller coaster simulation.

Core Credit Features: (Answer these Questions with Y/N; you can also insert comments as appropriate)
======================

1. Uses OpenGL core profile, version 3.2 or higher - YES

2. Completed all Levels:
  Level 1 : - YES
  level 2 : - YES
  Level 3 : - YES
  Level 4 : - YES
  Level 5 : - YES

3. Used Catmull-Rom Splines to render the Track -  YES

4. Rendered a Rail Cross Section - YES - Textured Quad strip

5. Rendered the camera at a reasonable speed in a continuous path/orientation - YES, riding orientation always up, perpendicular to tangent

6. Run at interactive frame rate (>15fps at 1280 x 720) - YES

7. Understandably written, well commented code - YES

8. Attached an Animation folder containing not more than 1000 screenshots - YES

9. Attached this ReadMe File - YES

Extra Credit Features: (Answer these Questions with Y/N; you can also insert comments as appropriate)
======================

1. Render a T-shaped rail cross section - NO

2. Render a Double Rail - Rail is Flat wide quad strip

3. Made the track circular and closed it with C1 continuity - YES

4. Added OpenGl lighting - NO

5. Any Additional Scene Elements? (list them here) - NO

6. Generate track from several sequences of splines - NO , but multiple tracks can be displayed

7. Draw splines using recursive subdivision - NO, but number of subdivided points is proportional to the segment length

8. Modify velocity with which the camera moves - NO

9. Create tracks that mimic a real world coaster - NO 

10. Render environment in a better manner - NO 

Additional Features: (Please document any additional features you may have implemented other than the ones described above)
1.  You may walk around when not riding. Sky is a half-spherical shape, not a simple box
2.

Open-Ended Problems: (Please document approaches to any open-ended problems that you have tackled)
1.
2.

Keyboard/Mouse controls: (Please document Keyboard/Mouse controls if any)
1. Press left mouse button to look around
2. 'W', 'A', 'S', 'D' for move in non RIDE mode, SPACE toggles ride on/off

Names of the .cpp files you made changes to:
1. only hw.1.cpp
2.

Comments : (If any)
Each part of the track is made of segments such that no segment is longer than some fixed size.
Because each segment advances the texture coordinates for the track by some fixed value, sometimes its more and sometimes less scaled. 
X makes single screenshot and Z makes 200 screenshots
