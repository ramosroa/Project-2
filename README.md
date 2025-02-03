# Project 2 System Behavior
### by Alexander Ramos-Rodriguez & Nick Komisky

This system features improvements to the advanced student-driver ignition system and introduces a headlight control system. We made two behavioral changes to the advanced driver safety system. The first change implemented was allowing the student driver to reattempt to successfully start the car after a previous failure to do so. The system previously only allowed for a singular start attempt and locked the student driver out of the ignition system after one failed attempt. The second added behavior was the ability to turn the car OFF by pressing the ignition button again after the ignition is already enabled. We also implemented an entirely new headlight control system. This system relied on a headlight mode selector knob with three positions: ON, OFF, and Auto. The ON mode simply kept the headlights ON constantly, while the OFF mode kept them OFF at all times. Lastly, the Auto mode turns the headlights ON only in low light situations, such as at night, to allow the driver to see the road. Lastly, it should be noted that the headlight mode can be adjusted while the car ignition is not activated, but the lights will never be on unless ignition is activated.

## Ignition Subsystem
| Behavior  | Test Result | Comment |
| ------------- | ------------- |--------------|
| Ignition Enabled LED (green LED) (Required both seat buttons pressed and both seatbelt switches in ON position)  | PASS  | 100% success for LED lighting when both buttons and switches are HIGH and for when at least one is LOW |
| Engine Started LED (blue LED)  | FAIL  | Requires the seat buttons to be held for a moment after the ignition button is pressed. Works ~95% of the time but occasionally fails if a seat button is released basically instantly after ignition button is pressed|
| Engine ON unless ignition button pressed again  | PASS  | 100% success in keeping engine ON regardless of any button presses or senor inputs other than the ignition button |
| Engine OFF once ignition button pressed while engine running | PASS  | 100% success in shutting OFF ignition with a second button press. Is not affected by seated or seat belt button states. |

## Headlight Subsystem
| Behavior | Test Result | Comment |
|--------------|--------------|--------------|
| ON & OFF Modes | PASS | Both modes keep the headlights in their respective state unless the engine is OFF |
| AUTO Mode | PASS | Headlights are OFF unless lack of light is detected, can be hard to achieve threshold lack of light in better lit rooms, but behavior is consistent overall |
| Headlights OFF whenever engine is OFF | PASS | Headlights turn OFF when ON if the engine is turned OFF. Stays OFF when engine is OFF regardless of button inputs |
