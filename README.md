# Recreation of Pac-Man classic game
<br>For Linux:<br/>
Make sure you have SDL library https://www.libsdl.org/
<br>If you don't have it, use these commands:<br/>
<br>sudo apt-get install libsdl2-dev
<br>sudo apt-get install libsdl2-image-dev
<br>sudo apt-get install libsdl2-mixer-dev
<br> To run this program on Windows please install MSYS2 first https://www.msys2.org/
<br>
In case of successfull installation run make and start the program with ./pacman

Game rules:
<br>There are 240 food dots on the field. Level is finished when you eat all dots.
<br> Use navigation arrows or A-W-D-S to move around.
<br>There are 4 special food dots called energizers - when you eat them, ghost becomes frightened and you can eat ghost and get extra points.
<br>2 times per level special food - fruit appears in the game. It can give you big amount of extra points.
<br>If ghost catches you, you lose your life. You have 3 lifes per level. If you loose the level, you will go back to Level 1.
<br>Sometimes ghosts hunt you, sometimes they are just walking around. The higher is level the longer they hunt.
