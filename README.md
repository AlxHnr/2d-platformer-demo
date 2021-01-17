![screenshot](https://user-images.githubusercontent.com/8235638/104827688-ca5b0700-5860-11eb-89d0-66032ca04072.gif)

Vector-based 2D platformer demo. Physics is running at a fixed tickrate. Linear interpolation is
used to render in-between states. The speed of the physics simulation can be slowed down or sped up
by an arbitrary factor at runtime. Collision detection uses the separating axis theorem. Objects
with a high velocity are processed in substeps (multisampling) to prevent clipping/tunneling trough
walls.

# Building and running the demo

CMake and a C++17 compiler is required. Most dependencies will be downloaded and compiled during the
build process.

```sh
mkdir build/
cd build/
cmake ..
cmake --build . --parallel "$(nproc)"
cmake --build . --target run
```

## Controls

* **left/right arrow keys** - Move
* **up** - Jump
* **r** - Reset game
* **mouse wheel** - Zoom
* **ctrl + mouse wheel** - Rotate camera
* **left mouse button** - Place solid block
* **right mouse button** - Place dynamic block
