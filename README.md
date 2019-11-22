Recreating Pac-Man
==================

# Building #

To build this project you'll should use the following tools:

* C++ compiler (like GCC)
* Make program (like GNU Make)
* pkg-config

You'll also need the libraries SDL2, SDL2 Image and SDL2 Mixer: https://www.libsdl.org/

## Installing Build Tools on GNU/Linux ##

On a Debian based system you may use the following command to install everything needed to build the game:
   
    sudo apt-get install g++ make pkg-config libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev

## Installing Build Tools on Windows ##

First download and install MSYS2: https://www.msys2.org/

Start C:\msys64\mingw64.exe and use the following command to install everything needed to build the game:

    pacman -S mingw-w64-x86_64-{SDL2,SDL2_image,SDL2_mixer,pkg-config} make

## Compiling ##

After install the build tools, you can go to the project directory and use the `make` command (where the Makefile is) to compile the game.  After compiling you can run the game by typing: `./pacman`

![Screenshot-(98)](https://user-images.githubusercontent.com/50684862/64925988-da904b80-d800-11e9-954d-616fedf801ab.jpg)

# Gameplay #

There are 240 dots on the field.  Level is finished when Pacman eats all the dots.  Use arrow keys or A-W-D-S to move around.

If a ghost catches Pacman, he loses a life.  The game begins from Level 1 when Pacman loses all of his lifes.

There are 4 special dots called energizers.  When Pacman eats them, the ghosts become frightened and Pacman can eat them and earn extra points.  2 times per level a special fruit appears, which can be eaten for a big amount of extra points.

Sometimes ghosts hunt Pacman and sometimes they are just floating around.  On higher levels they spend more time hunting than floating.

![Screenshot-(99)](https://user-images.githubusercontent.com/50684862/64925996-f1cf3900-d800-11e9-84ba-78946ca5b160.jpg)

## Technicals

The game window is split into tiles.  A ghost catches Pacman if their centers are on the same tile.  Ghosts are always moving to a target tile.  While not hunting, they go to corner tiles, but when they hunt Pacman, each ghosts have different target:

* Red ghost runs to the Pacman active tile
* Pink ghost runs to the one 4 tiles before Pacman
* Orange ghost runs to the Pacman active tile if it's not farther then 8 tiles from Pacman
* Cyan ghost runs to Pacman but also follows red ghost’s direction

Every ghost has different duration of "shy mode" (when it is in the ghost house).

What happens on the higher levels?

1. Float time gets shorter
2. Hunt time gets longer
3. Shy mode gets shorter
4. Frightened mode gets shorter and dissapear on level 10
5. Fruit points get higher
