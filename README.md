# Arduino Micro Arcade - Joust

I programmed this project as my final project for EECS183!
For this project, I decided to program a reimagined version of the classic arcade game Joust! 
To play you need to use an Arduino Mega, two potentiometers, two buttons, a 64 x 32 LED Screen and optionally
in order to use Bluetooth mode, you need a Bluetooth capable Android device and a UART Bluetooth module (I 
personally used the SparkFun Bluetooth Modem - BlueSMiRF Gold). For wiring, player one's potentiometer must 
be connected to Analog Pin 6, player two's connected to Analog Pin 7. Player one's button must be attached 
to Digital Pin 6, player two's connected to Digital Pin 7. The Bluetooth module's RX pin must be connected to
Digital Pin 50, the TX pin must be connected to Digital Pin 51.

## Controls:
Players control their direction and speed with a potentiometer and can fly by holding down 
or pressing their button. The player can go through platforms from below but not above and 
can loop around the screen horizontally and vertically by going far enough in either direction.
To kill an enemy, one must land on top of them or stab them from behind. Whenever two characters'
swords collide, both characters will bounce backwards.

## Menu/UI:
The game launches with a title screen displaying "Joust" and a player character moving across the screen.
To enter the mode select, press the player one's button. This will take you to a screen where you can use 
the player one's potentiometer to select 1 or 2 player mode. The selected mode will be surronded by a 
yellow box. To confirm your selection, press the player one's button again. Pressing 2 Player will launch 
a player versus player mode whereas pressing 1 player allows you to choose to play with either classic controls
or Bluetooth controls. See the Bluetooth section for how to play with Bluetooth but make your selection like how you 
selected 1 player or 2 player. Finishing a game will return you back to the title screen.

## Bluetooth Controls:
In order to play with Bluetooth controls, you must have an Android device with Bluetooth capabilities and download
the apk for the Bluetooth app (which should be included in the repository, if that has issues, here's a [Google Drive
link](https://drive.google.com/file/d/1stkX1Wa8NP_r3pyhg6UlNON5bldiFzwN/view?usp=sharing)). The Bluetooth app
will allow you to connect to the Bluetooth module by pressing scan and then pressing on the name of the 
Bluetooth module. Launch the Bluetooth mode by selecting it via the menu with the button and potentiometer.
From here, in order to control your character, you can use the slider to select what direction/speed you want to go
(ex: the further left, the faster you'll move to the left). Keep in mind the slider only registers the START and END 
result of where the slider is (ex: dragging the slider and letting go won't send all the middle values between the 
two points you dragged it, it will only send the first value and the last). To Fly, press the fly button. To stop 
flying, press the same button which will now be labeled "Stop Flying". If you dislike how dark the app is, you can 
press Light Mode to change the colors on the screen to be brighter and if that bothers you you can press Night Mode 
to return back to the normal setting. If you would like to diconnect your device, press the disconnect button. This 
app was made in MIT App Inventor by Mitchell Kuppersmith (mkuppers).

## Game modes:

### 1 Player:
The objective is to kill as many waves of AI controlled opponents as you can by stabbing them in the back 
or landing on top of them. For each kill, the player earns one point. Whenever you finish a wave, a random 
stage will be selected for the next wave. The game runs until the player runs out of lives at which point 
the player's score is shown and compared to the high score. If playing without Bluetooth, each wave will have
3 enemies. Due to the difficulty in controlling via Bluetooth, in Bluetooth mode there is only one enemy per wave.

### 2 Player:
2 Player Versus is a 1v1 multiplayer version of Joust with each player controlling their own character.
The objective is to kill the other player by either landing your bird on top of theirs or impaling them 
in the back with your sword. Whenever a player is killed, both player's lives will be displayed and a 
random stage will be decided for the next round. Whichever player loses all their lives first loses the game. 



