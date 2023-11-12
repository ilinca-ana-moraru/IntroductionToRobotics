# IntroductionToRobotics 
Introduction to Robotics - weekly homewoks

This repo will contain my weekly homeworks for my Introduction to Robotics course I'm taking at the Faculty of Mathematics and Computer Science, Univeristy of Bucharest.
For each homework, I will provide requirements, implementation details, code files, pictures and videos of the setup.

<details>
<summary> Homework 5
</summary>
<br>
  
### Requirements

Using the 4 digit 7 segment display and 3 buttons, implement a stopwatch timer that counts in 10ths of a second
and has a save lap functionality. Your buttons should have the following functionalities:
– Button 1: Start / pause.
– Button 2: Reset (if in pause mode). Reset saved laps (if in lap
viewing mode).
– Button 3: Save lap (if in counting mode), cycle through last saved
laps (up to 4 laps).

#### Workflow

1. Display shows ”000.0”. When pressing the Start button, the timer should start.
2. During timer counter, each time you press the lap button, the timer’s valuen should be saved in memory, up to 4 laps. Pressing the 5th time should override the 1st saved one. If you press the resetbutton while timer works, nothing happens. If you press the pausebutton, the timer stops.
3. In Pause Mode, the lap flag button doesn’t work anymore. Pressing the reset button resets to 000.0.
4. After reset, the flag buttons can be pressed to cycle through the lap times. Each time you press the flag button, it displays to the next saved lap. Pressing it continuously should cycle through it continuously. Pressing the reset button while in this state resets all flags and takes the timer back to ”000.0”.

### Photo

![photo](https://github.com/ilinca-ana-moraru/IntroductionToRobotics/blob/main/homework5/StopwatchTimer.jpeg)

### Video 

https://www.youtube.com/watch?v=qyF0SFH7i9U
</details>

<details>
<summary>Homework 4</summary>
<br>
  
### Requirements

Use the joystick to control the position ofthe segment and ”draw” on the display.  The movement between segments should be natural, meaning they should jump from the current positiononly to neighbors, but without passing through ”walls”.
The  initial  position  should  be  on  the  DP.  The  current position always blinks (irrespective of the fact that the segment is on or off).  Use the joystick to move from one position to neighbors. Short pressing the button toggles the segment state  from  ON  to  OFF  or  from  OFF  to  ON.  Long  pressing  the  button resets the entire display by turning all the segments OFF and moving the current position to the decimal point.

### Photo

![photo](https://github.com/ilinca-ana-moraru/IntroductionToRobotics/blob/main/homework4/7segment_display.jpeg)

### Video

https://www.youtube.com/watch?v=qkIAa9wdMFs

</details>

<details>
<summary>Homework 3</summary>
<br>
  
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

</details>

<details>
<summary>Homework 2</summary>
<br>
  
### Requirements

Use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. This control must leverage digital electronics. Specifically, you  need  to  read the  potentiometer’s  value  with  Arduino  and  then  write a mapped value to the LED pins.

### Photo

![photo](https://github.com/ilinca-ana-moraru/IntroductionToRobotics/blob/main/tema2/RGB%20photo.jpeg)

### Video
https://youtube.com/shorts/oJRcK_7D9mI?si=N4qC495yyYWKMDAx</details>




