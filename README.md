# CSCI 5299 Computer Graphics - Homework 3: Lighting-and-Textures

## Program Overview

Homework 3 added lighting and texture effects to homework 2 while keeping the same orthographic, perspective, and first-person projection modes.

The scene now has textured land, river, road, house walls, roof, windows, trees, and an airplane. A moving light source is rotating in the sky lighting that shows ambient, diffuse, and specular lighting effects.

## How to run

### One command
```sh
make clean && make && ./hw3
```

### Step by step
1. Build the program: `make`
2. Run the program: `./hw3`
3. Clean files: `make clean`

## Keyboard Controls

### View and projection
- `m/M`: Toggle between orthographic, perspective, and first-person views
- `+/-`: Increase or decrease the field of view in perspective mode
- `PgUp/PgDn`: Zoom in and out
- `Left/Right/Up/Down`: Rotate the camera or adjust the view direction

### Lighting and shading
- `l`: Toggle lighting on/off
- `L`: Toggle smooth/flat shading
- `i/I`: Decrease/increase ambient light
- `d/D`: Decrease/increase diffuse light
- `s/S`: Decrease/increase specular light
- `e/E`: Decrease/increase emitted light
- `n/N`: Decrease/increase shininess
- `[/]`: Lower/raise the light elevation

### Texture and object selection
- `t/T`: Toggle textures on/off
- `o/O`: Switch objects

### Common keys
- `f/F`: Move forward in first-person mode
- `b/B`: Move backward in first-person mode
- `0`: Reset the view angles and starting position
- `a/A`: Toggle the axes display
- `Esc`: Exit the program

## Implementation Details

- The program adds OpenGL lighting with ambient, diffuse, specular, emission, and shininess controls.
- Textures are loaded from BMP files for the grass, river, road, house walls, roof, window, tree trunk, leaves and airplane.
- Object selection show individule objects one at a time for lighting inspection.
- Normal vectors are explicitly supplied for lighting, and `GL_NORMALIZE` is used to make sure rendering is not impacted by scaling.
- The first-person camera behavior is preserved from the previous assignment, including forward/backward movement and view-angle updates.

## Estimated Time

- 6.5 hours learning and understanding lighting and texture concepts
- 10 hours coding, testing, and refining the scene

## Links
- Canvas: https://canvas.colorado.edu/courses/135873/assignments/2723110
- GitHub Repo: https://github.com/yagu6173/Lighting-and-Textures
