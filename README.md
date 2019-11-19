# Recreation of Pac-Man classic game
<br>For Linux:<br/>
Make sure Pacman have SDL library installed https://www.libsdl.org/
<br>If Pacman don't have it, use these commands:<br/>
<br>sudo apt-get install libsdl2-dev
<br>sudo apt-get install libsdl2-image-dev
<br>sudo apt-get install libsdl2-mixer-dev<br/>
<br>For Windows:
Make sure Pacman install MSYS2 first https://www.msys2.org/
<br> To add SDL library to MSYS2, use this command:
<br>pacman -S mingw-w64-x86_64-{SDL2,SDL2_image,SDL2_mixer,pkg-config} make<br/>
<br>After successful installation, compile using given Makefile and start the game by typing ./pacman
![Screenshot-(98)](https://user-images.githubusercontent.com/50684862/64925988-da904b80-d800-11e9-954d-616fedf801ab.jpg)
<br>Game rules:
<br>There are 240 dots on the field. Level is finished when Pacman eats all dots.
<br> Use arrow keys or A-W-D-S to move around.
<br>There are 4 special food dots called energizers. When Pacman eats them, ghosts become frightened mode and Pacman can eat ghosts. Extra points are given when Pacman actually eats ghosts.
<br>Extra points are given when Pacman actually eats ghosts. It gives large amount of extra points.
<br>If a ghost catches Pacman, Pacman loses its life. There are 3 lives per level. If Pacman get caught during its last life, game will go back to Level 1.
<br>Sometimes ghosts hunt Pacman but sometimes they are just floating around. Higher the level is the longer they hunt Pacman.
![Screenshot-(99)](https://user-images.githubusercontent.com/50684862/64925996-f1cf3900-d800-11e9-84ba-78946ca5b160.jpg)
<br>More information about game mechanics:
<br>The game window is split into tiles. The ghost catches Pacman if their centers are in the same tile.
<br>The weak place of this logic is if Pacman meets the ghost at the edge of the tile, on the next step Pacman can jump to the ghost tile, and ghost jump to the Pacman tile. That's why you should check tiles twice - after each Pacman step and after each ghost step.
<br>Ghosts are always running to some target tile. While floating, they run to the corner tiles. When they hunt Pacman, each ghosts have different target.
<br>- Red ghost runs to the Pacman active tile;
<br>- Pink ghost runs to the one 4 tiles before Pacman;
<br>- Orange ghost runs to the Pacman active tile if it's not farther then 8 tiles from Pacman;
<br>- Cyan ghost runs to Pacman but also follows red ghost’s direction.
<br>Every ghost has different lenth of "shy mode" (when it is in the ghost house).
<br>At Level 1 they follow the next cylce:
<br>7 sec of walk → 20 sec of hunt → 7 sec of walk → 20 sec of hunt → 5 sec of walk → 20 sec of hunt → 5 sec of walk → hunt forever
<br>
<br> What happens on the higher levels?
<br>1) Float time gets shorter;
<br>2) Hunt time gets longer;
<br>3) Shy mode gets shorter;
<br>4) Frightened mode gets shorter and dissapear on level 10;
<br>5) Fruit points get higher.
