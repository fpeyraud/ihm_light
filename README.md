# ihm_light
Basic MMI (IHM) for Tryphon racks

## Description :

Composed of 2 buttons and 4 multi-color LEDs, this interface can be used to
- boot a computer,
- trigger the reset button
- trigger an NMI (short PWON button press)
- trigger a forced shutdown (long PWON button press)
- send button down and button up events to the computer via an USB serial connection
- light LEDs individually or all at once according to commands sent via the USB serial connection

## The hardware
The hardware is composed of an Arduino Pro Micro as [sold by Sparkfun](https://www.sparkfun.com/products/12640) or others on Ebay, coupled to a shield which board and schematics is given in the {board} directory. The shield is a double sided PCB which should be easily realised online.

## The code
The goal has been to stay in the arduino paradigm as much as possible whule providing a complete functionnality with as few blocking functions (delay()) as I could. Oh no it's probably not the best code, but it works pretty well.

## How to use the overall project

When the board is powered up, the leds start scrolling until the serial connection is established.
When established, the board acts on the following commands

* `Lwxyz` : Light leds all at once, wxyz represent the color of each led. It can be set according to the following table:
  * 0 : Off
  * R : Red
  * G : Green
  * B : Blue
  * O : Orange
  * P : Purple
  * C : Cyan
  * W : White
  * r : blinking red
  * g : blinking green
  * b : blinking blue
  * o : blinking orange
  * p : blinking purple
  * c : blinking cyan
  * w : blinking white
* `BRn` : Set brightness from 0 (min) to 15 (max)
* `SLnc` : Light one LED number n with color c as desribed above
* `PWOFF` : Trigger a hard Power Off 
* `RESET` : Trigger the reset button
* `NMI` : Trigger a short PWON press

When the buttons are pressed, L or R are sent on the USB serial port, and l or r are sent when buttons are released.

When both L and R buttons are pressed for 3 seconds, the board enters an internal menu with four options. The currently selected option is represented bu the red LED. Option selection is changed by pressing the L button. The currently option is validated by pressing the R button. The functions for each option is :
1. exit menu
2. NMI
3. RESET
4. Power OFF

When no button is pressed for a certain time after entering the menu, it is automatically exited and LED state is restored as before entering the menu.

## And....

and that's all it does so far. Of course we could add a lot more, but for the time being, it just fits my need ;) Feel free to fork and pull request. Thanks for your attendance ;)
