# Game of Life 84+CE v 1.4

2024 [Merthsoft Creations](shaun@shaunmcfall.com)

[Conway's Game of Life](https://en.wikipedia.org/wiki/Conway's_Game_of_Life) is a cellular automaton created by John Conway in 1970. This is an implementation
for the 84+CE graphing calculator.

Requires [the CE Standard Libraries](https://github.com/CE-Programming/libraries/releases/).

## Controls

| key | action |
|-|-|
|Arrow keys | Move cursor around |
| 2nd | Toggle current cell state |
| Clear | Clears the screen |
| + | Step one generation of the simulation |
|Vars | Produces a random board |
|Mode | Enter setup |
|Enter | Toggle running
|Del | Exit

## Settings

Aside from aethetic things like colors and cell-size, there are also some more advanced settings available to the user:

### Rules

Conway's Game of Life can be modified with [different rule sets](https://en.wikipedia.org/wiki/Life-like_cellular_automaton). This implementation comes with 19 pre-defined rule sets, and also lets you implement your own custom rules.

### Topologies

There are seven topologies to choose from, which change the wrapping mode when the cells get to the edge of the boards. There is an explanation in picture form of each topology when selecting them. Green points illustrate points where "things go weird". Arrows of matching colors indicate edges that are stitched together.

## Source code

The source is maintained in a [git repository available on github](https://github.com/merthsoft/gol-84-ce)

## Credits

- Thanks to everyone maintaining the [toolchain]((https://github.com/CE-Programming/toolchain)).
- Thanks to elfprince for his help in implementing the topologies, as well as providing the images for said topologies.
- Thanks to Tari for all his programming help. I'm bad at C, and he is significantly better :)

## Revision History

| version | change note |
| - | - |
| 1.4 | Memory cleanup |
|     | Improved controls |
|     | Improved drawing |
| 1.3 | Performance change to plane topology |
|     | No stamps for now |
| 1.2 | Upgrade to v8.8 of toolchain |
|     | Saving/loading of board |
|     | Stamps |
| 1.01 | Fix a memory leak in key_helper |
| 1 | Initial release |
