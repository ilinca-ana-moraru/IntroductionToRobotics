# IntroductionToRobotics 
Introduction to Robotics - weekly homewoks

This repo will contain my weekly homeworks for my Introduction to Robotics course I'm taking at the Faculty of Mathematics and Computer Science, Univeristy of Bucharest.
For each homework, I will provide requirements, implementation details, code files, pictures and videos of the setup.

## Homework 4

### Requirements

Use the joystick to control the position ofthe segment and ”draw” on the display.  The movement between segments should be natural, meaning they should jump from the current positiononly to neighbors, but without passing through ”walls”.
The  initial  position  should  be  on  the  DP.  The  current position always blinks (irrespective of the fact that the segment is on or off).  Use the joystick to move from one position to neighbors. Short pressing the button toggles the segment state  from  ON  to  OFF  or  from  OFF  to  ON.  Long  pressing  the  button resets the entire display by turning all the segments OFF and moving the current position to the decimal point.

### Photo

![photo](https://github.com/ilinca-ana-moraru/IntroductionToRobotics/blob/main/homework4/7segment_display.jpeg)

### Video

https://www.youtube.com/watch?v=qkIAa9wdMFs

## Homework 3

### Requirements

Design  a  control  system  that  simulates  a  3-floor  elevator  using  the  Arduino platform. 

•LED Indicators: Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up. Additionally, another LED should represent the elevator’s operational state. It shouldblink when the elevator is moving and remain static when stationary

•Buttons: Implement 3 buttons that represent the call buttons from the3 floors. When pressed, the elevator should simulate movement towards the floor after a short interval (2-3 seconds)

•Buzzer: The buzzer should sound briefly during the following scenarios:

  –Elevator arriving at the desired floor

  –Elevator  doors  closing  and  movement 

### Photo

![photo](https://github.com/ilinca-ana-moraru/IntroductionToRobotics/blob/main/homework3/Elevator_photo.jpeg)

### Video

https://www.youtube.com/watch?v=3fIW2rPJU-E

## Homework 2

### Requirements

Use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. This control must leverage digital electronics. Specifically, you  need  to  read the  potentiometer’s  value  with  Arduino  and  then  write a mapped value to the LED pins.

### Photo

![photo](https://github.com/ilinca-ana-moraru/IntroductionToRobotics/blob/main/tema2/RGB%20photo.jpeg)

### Video
https://youtube.com/shorts/oJRcK_7D9mI?si=N4qC495yyYWKMDAx

