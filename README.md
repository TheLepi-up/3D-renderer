# 3D CPU-renderer for block based worlds based on Raycasting
I wondered what would happen, if I use raycasting in 3D. This is a technique commonly used to make 2D-Worlds look like 3D in old videogames like Doom. I used the same algorithm to collect all visible squares in a scene and then render only these squares.

## Implementation details
This project is one of my hobby projects and therefore uses 0% AI as that would be beside the point. I used C++, Cmake and SFML as I am most familiar with them.

main.cpp:
main event loop, window creation and initial setup.
Movements are handled here for now, but this should change once game.cpp is implemented.

logging.cpp, logging.h:
log functions and definitions for configuring log-levels and handling log calls.

renderer.h, renderer.cpp:
store player location and orientation relative to the scene and handle scene rendering.
rendering works in two passes: 
* Raycasting for identifying visible squares in the scene. Raycasting is based on the algorithm on [this website](https://lodev.org/cgtutor/raycasting.html). An array (shadowmap) is used to reduce the number of times raycasting is performed by skipping pixels that are covered by a visible tile. Due to the nature of a 3D grid, there should not be any missing squares.
* Tile rendering: all squares are sorted according to the distance from the viewing plane and then rendered in that order. Shadowmap is used again in order to not cover a close square by a more distant one.

game.h, game.cpp:
currnently empty. This will contain the world generation in the future.


## Future plans
* Implement procedural terrain generation and game.cpp for world generation. I would like to use spaghetti-cave generation in order to generate a maze of some sort.
* Optimize the renderer in order to be able to render bigger scenes smoothly.
* Implement proper movements in order to move along the viewing direction.
* Add some fog to cover the end of the scene.
* Optimize raycasting for areas of the screen where rays escape the scene.