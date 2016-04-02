Game of Life 84+CE v 1.01
2016 Merthsoft Creations
shaun@shaunmcfall.com

Conway's Game of Life for the TI-84+CE
https://en.wikipedia.org/wiki/Conway's_Game_of_Life

The Game of Life is a cellular automaton created by John Conway in 1970. This is an implementation
for the 84+CE graphing calculator.

Requires the CE libraries made by MateoConLechuga:
https://github.com/CE-Programming/libraries/releases/tag/v3.0

Controls:
Arrow keys - Move cursor around
2nd - Toggle current cell state
Clean - Clears the screen
+ - Step one generation of the simulation
Vars - Produces a random board
Mode - Enter setup
Enter - Run or pause the simulation (you may need to hold this down in order to pause)
Del - Exit

Settings:
Aside from aethetic things like colors and cell-size, there are also some more advanced settings
available to the user:

Rules:
Conway's Game of Life can be modified with different rule sets. See https://en.wikipedia.org/wiki/Life-like_cellular_automaton
for more information. 
This implementation comes with 19 pre-defined rule sets, and also lets you implement your own custom rules.

Topologies:
There are seven topologies to choose from, which change the wrapping mode when the cells get to the edge of the boards.
There is an explanation if picture form of each topology when selecting them. Green points illustrate points where 
"things go weird". Arrows of matching colors indicate edges that are stitched together.

Source code:
The source is maintained in a git repository available here: https://bitbucket.org/merthsoft/gol-84-ce

Credits:
Many thanks to Mateo for the CE toolchain, libraries, and CEmu, all of which made this possible. He also fixed
many bugs for me I found along the way.

Thanks to elfprince for his help in implementing the topologies, as well as providing the images for said topologies.

Thanks to Tari for all his programming help. I'm bad at C, and he is significantly better :)

Revision History:

v 1.01 - Fix a memory leak in key_helper

v 1 - Initial release