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

<br>More information about game mechanics:
<br>The game window is splited of tiles. Ghost catches pacman if their centers are in the same tile.
<br>The weak place of this logic: if Pacman meets ghost at the edge of tiles, on the next step Pacman can jump to the ghost tile, ang ghost jump to the Pacman tile. That's why you should check tiles twice - after each Pacman step and after each ghost step.
<br>Ghosts are always running to some target tile. When they walk, they run to the corner tiles. When they hunt Pacman:
<br>- Red ghost runs to Pacman active tile;
<br>- Pink ghost runs to the one 4 tiles before Pacman;
<br>- Orange ghost runs Pacman active tile if it's not father then 8 tiles from him;
<br>- Cyan ghost runs Pacman but also follows red ghost direction.
<br>Every ghost has different lenth of "shy mode" (when it sits in the ghost house).
<br>At Level 1 they follow the next cylce:
<br>7 sec of walk - 20 sec of hunt - 7 sec of walk - 20 sec of hunt - 5 sec of walk - 20 sec of hunt - 5 sec of walk - forever hunt
<br>
<br> What happens on the higher levels?
<br>1) Walk time gets shorter;
<br>2) Hunt time gets longer;
<br>3) Shy mode (time in the house) gets shorter
<br>4) Frightened more (when you can eat ghosts) gets shorter and dissapear on level 10;
<br>5) Fruit bonus grows.
