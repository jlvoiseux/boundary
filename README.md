# Boundary

A small boundary representation geometry kernel and solid modelling interface.

Based on [An Introduction to Solid Modeling](https://www.amazon.com/Introduction-Solid-Modeling-Martti-Mantyla/dp/0716780151), by Martti Mantyla.
Raylib Dear ImGui [backend](https://github.com/alfredbaudisch/raylib-cimgui) by Alfred Reinold Baudisch

## Syntax
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

## Examples

### Cube
```
MVFS 1 1 1 0.0 0.0 0.0
SMEV 1 1 1 2 2.0 0.0 0.0
SMEV 1 1 2 3 2.0 2.0 0.0
SMEV 1 1 3 4 0.0 2.0 0.0
SMEF 1 1 1 4 2
SWEEP 1 2 0.0 0.0 2.0
```

### Cylinder
```
MVFS 1 1 1 2.0 0.0 0.0
CIRCLE 1 0.0 0.0 2.0 0.0 32
SWEEP 1 2 0.0 0.0 2.0
```

### Sphere
```
MVFS 1 1 1 -2.0 0.0 0.0
ARC 1 1 1 0.0 0.0 2.0 0.0 180.0 0.0 32
RSWEEP 1 32 1.0 0.0 0.0
```

### Torus
```
MVFS 1 1 1 0.5 1.5 0.0
CIRCLE 1 0.0 1.5 0.5 0 32
RSWEEP 1 32 1.0 0.0 0.0
```
