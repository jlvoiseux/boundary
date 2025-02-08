# Boundary
There is a certain elegance in the **boundary representation of solids**. A relatively small set of basic topological elements
(edges, faces, vertices), together with a compact kernel of basic operations, allows for accurate representation of a wide
range of 3D shapes. This kernel implements the set of fundamental operations described in Martti Mäntylä's "[Introduction to
Solid Modelling](https://books.google.fi/books/about/An_Introduction_to_Solid_Modeling.html)" in C, with a basic ImGui + Raylib user interface.

## Screenshots
![boundary](https://github.com/user-attachments/assets/10879f0d-d7e3-4866-aa35-91e1c4e38dbb)
<img width="959" alt="Screenshot 2025-02-08 230928" src="https://github.com/user-attachments/assets/a2a01ff0-55e1-4fe7-8983-fb00d12cd9f2" />
<img width="958" alt="Screenshot 2025-02-08 230859" src="https://github.com/user-attachments/assets/0954444b-674f-4b87-8b0a-0f0c04a21d49" />


## Usage
### Syntax
```
// Initialization operator (Make Vertex Face Solid)
Op      int         int         int         float   float   float
MVFS    SOLID_ID    FACE_ID     VERT_ID   POS_X   POS_Y   POS_Z

// Kill Vertex Face Solid
Op      int
KVFS    SOLID_ID

// Make Edge Vertex
Op      int         int         int         int     int     int     int     float   float   float
MEV     SOLID_ID    FACE1_ID    FACE2_ID    V1_ID   V2_ID   V3_ID   V4_ID   POS_X   POS_Y   POS_Z

// Simple Make Edge Vertex (adds a dangling edge)
Op      int         int         int         int     float   float   float
SMEV    SOLID_ID    FACE_ID     V1_ID       V4_ID   POS_X   POS_Y   POS_Z

// Kill Edge Vertex
Op      int         int         int         int
KEV     SOLID_ID    FACE_ID     V1_ID       V2_ID

// Make Edge Face
Op      int         int         int         int     int     int     int
MEF     SOLID_ID    FACE1_ID    V1_ID       V2_ID   V3_ID   V4_ID   FACE2_ID

// Simple Make Edge Face
Op      int         int         int         int     int
SMEF    SOLID_ID    FACE1_ID    V1_ID       V3_ID   FACE2_ID

// Kill Edge Face
Op      int         int         int         int
KEF     SOLID_ID    FACE_ID     V1_ID       V2_ID

// Kill Edge Make Ring (splits loop)
Op      int         int         int         int
KEMR    SOLID_ID    FACE_ID     V1_ID       V2_ID

// Make Edge Kill Ring (joins loops)
Op      int         int         int         int     int     int
MEKR    SOLID_ID    FACE_ID     V1_ID       V2_ID   V3_ID   V4_ID

// Simple Make Edge Kill Ring
Op      int         int         int         int
SMEKR   SOLID_ID    FACE_ID     V1_ID       V3_ID

// Kill Face Make Ring Hole (merges faces by creating inner loop)
Op      int         int         int
KFMRH   SOLID_ID    FACE1_ID    FACE2_ID

// Make Face Kill Ring Hole
Op      int         int         int         int     int
MFKRH   SOLID_ID    FACE1_ID    V1_ID       V2_ID   FACE2_ID

// Sweep face along vector
Op      int         int         float       float   float
SWEEP   SOLID_ID    FACE_ID     DIR_X       DIR_Y   DIR_Z

// Rotational sweep
Op      int         int         float       float   float
RSWEEP  SOLID_ID    NUM_FACES   AXIS_X      AXIS_Y  AXIS_Z

// Create arc
Op      int         int         int         float   float   float   float   float   float   int
ARC     SOLID_ID    FACE_ID     VERT_ID     CEN_X   CEN_Y   RAD     H       PHI1    PHI2    NUM_SEGS

// Create circle
Op      int         float       float       float   float   int
CIRCLE  SOLID_ID    CEN_X       CEN_Y       RAD     H       NUM_SEGS
```

### Examples

#### Cube
```
MVFS 1 1 1 0.0 0.0 0.0
SMEV 1 1 1 2 2.0 0.0 0.0
SMEV 1 1 2 3 2.0 2.0 0.0
SMEV 1 1 3 4 0.0 2.0 0.0
SMEF 1 1 1 4 2
SWEEP 1 2 0.0 0.0 2.0
```

#### Cylinder
```
MVFS 1 1 1 2.0 0.0 0.0
CIRCLE 1 0.0 0.0 2.0 0.0 32
SWEEP 1 2 0.0 0.0 2.0
```

#### Sphere
```
MVFS 1 1 1 -2.0 0.0 0.0
ARC 1 1 1 0.0 0.0 2.0 0.0 180.0 0.0 32
RSWEEP 1 32 1.0 0.0 0.0
```

#### Torus
```
MVFS 1 1 1 0.5 1.5 0.0
CIRCLE 1 0.0 1.5 0.5 0 32
RSWEEP 1 32 1.0 0.0 0.0
```

## References and dependencies
- Based on [An Introduction to Solid Modeling](https://www.amazon.com/Introduction-Solid-Modeling-Martti-Mantyla/dp/0716780151), by Martti Mantyla.
- Raylib ([source](https://github.com/raysan5/raylib), [license](https://github.com/raysan5/raylib?tab=Zlib-1-ov-file#readme))
- cImGui ([source](https://github.com/cimgui/cimgui), [license](https://github.com/cimgui/cimgui?tab=MIT-1-ov-file#readme))
- Raylib C Dear ImGui backend ([source](https://github.com/alfredbaudisch/raylib-cimgui), [license](https://github.com/alfredbaudisch/raylib-cimgui?tab=Zlib-1-ov-file#readme))
- Unity ([source](https://github.com/ThrowTheSwitch/Unity), [license](https://github.com/ThrowTheSwitch/Unity?tab=MIT-1-ov-file#readme))
