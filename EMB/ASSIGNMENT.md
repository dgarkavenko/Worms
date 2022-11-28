# Embark Worms

## Assignment

You have been tasked with finishing a game for a new console.

Game rules:

* The game takes place on a 1920 by 1080 units big game world.
* There is one player controlled worm and one or more AI controlled worms in the world.
* Going outside the world boundaries or colliding with another worm means death.
* Each worm starts out as 5 segments long.
* There is no upper bound on the worm's length.
* There are at least 500 units of food spread out in the world at the start of the game.
* Eating (colliding with) one food unit will grow a worm by one segment.
* Food may be grouped into piles with more than one food unit, if desired.
* When a worm dies, each segment is converted to one food unit and drops where the worm dies.
* The player wins the game by being the last worm standing.

Controlling the player worm:

* The worm's head is always centered at the origin of the screen.
* The worm constantly moves towards the mouse cursor. I.e. it can't stop or stand still.
* When the left mouse button is pressed, the worm's speed is boosted.
* When boosting, the worm loses segments which are converted to food. I.e. losing one segment means one food unit must be dropped.

The game designer has made up very strict rules for the AI:

* It can learn about the world by sending out sensor rays from the head.
* It can sense walls, other worms and food at a maximum of 100 units distance.
* It can send out maximum 8 sensor rays every update.
* It is not allowed to "cheat" by knowing anything other than what it can learn from its sensor rays.

Create a game which follows the above specification and is attractive to our players.

## The Library

The code under WormsLib should be considered immutable. It can be modified to work with a certain compiler or environment, but each change should be clearly documented and justified.

The code under WormsGame can be changed at will and that's where all new game code should be placed.

The code and APIs in Worms.h must be used by the game.

WormsAudioHelp.h and WormsVideoHelp.h are optional help classes.

Make sure vs2019/EmbarkWorms.sln can be built in both Release and Debug.

If the game for some reason cannot be built with the Visual Studio solution, then exact steps to build the game on Windows must be included.

Do not include any prebuilt binaries, object files or other intermediate files in the submission.

Please do not make the game or source code public on Github, for example. We want to reuse this assignment for a long time and don't want implementations to be available online.

## Console Constraints

The new game console has no storage, so it's not possible to load or save any assets, like images or sound files.

It also has no network card.

The console has no GPU, only a CPU.

The exact specs for the CPU are unknown as of this writing, but rumors on tech forums hints at a 4-core, 64-bit CPU running at 2GHz, with support for AVX2.

The video output is 960x540 pixels scaled up to 1920x1080.

Audio is stereo only.

## Building

### Windows

* Install Visual Studio 2019 Community Edition (or Professional)
* Open `vs2019\EmbarkWorms.sln`
* Build either the `Debug` or `Release` configuration

### Linux

* Install CMake
* Install a C++17 capable compiler
* CMake will check for additional dependencies, like `Xcb`, `Xkbcommon` and `asound`
* Build with

    ```bash
    mkdir build ; cd build
    cmake ../linux
    cmake --build .
    ```
