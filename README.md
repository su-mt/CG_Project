# CG_Project

3D Rendering of Cube and Pyramid with Z-Buffer, Shadows and Interactive Controls

## Description

Interactive 3D application for geometric object visualization, written in C using X11 library. The project demonstrates fundamental computer graphics principles:

- **3D Transformations** (scaling, rotation, translation)
- **Projections** (perspective, orthographic, isometric)
- **Z-Buffering** for correct depth rendering
- **Shadow Casting** using projection method
- **Double Buffering** to eliminate flickering



## System Requirements

### Supported Platforms:
- **macOS** (Intel x86_64 and Apple Silicon arm64)
- **Linux** (x86_64, arm64)

### Dependencies:
- X11 libraries
- GCC or Clang compiler
- Make utility

## Installation and Build

### Automatic dependency installation

```bash
make install-deps
```

### Project build

```bash
# Clone repository
git clone https://github.com/su-mt/CG_Project
cd CG_Project

# Build
make

# Build and run
make run

# Get system information
make info

# Clean
make clean
```

## Controls

### Camera Movement:
- `W` / `S` - move up/down
- `A` / `D` - move left/right
- `Z` / `X` - zoom in/out

### Camera Rotation:
- `Q` / `E` - turn left/right (around Y axis)
- `R` / `F` - tilt up/down (around X axis)
- `,` / `.` - roll left/right (around Z axis)

### Scaling:
- `+` - increase scale
- `-` - decrease scale

### Additional Functions:
- `P` - switch projection type (perspective/orthographic/isometric)
- `Q` (Shift+Q) - exit program


## Troubleshooting

- Ensure XQuartz/X11 server is installed and running
- `export DISPLAY=:0`
- Check library installation: `pkg-config --exists x11`


## Configuration Parameters

Main constants can be changed in file `src/include/geometry.h`:

```c
#define WIDTH 800               // Window width
#define HEIGHT 600              // Window height
#define FOV 750.0              // Field of view
#define VIEWER_DISTANCE 1000.0  // Distance to viewer
#define DELTA_TRANSLATION 10.0  // Translation step
#define DELTA_ROTATION (M_PI/60.0)  // Rotation step
```