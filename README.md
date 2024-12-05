Based on [An Introduction to Solid Modeling](https://www.amazon.com/Introduction-Solid-Modeling-Martti-Mantyla/dp/0716780151), by Martti Mantyla.
Raylib Dear ImGui [backend](https://github.com/alfredbaudisch/raylib-cimgui) by Alfred Reinold Baudisch

## Examples
### Cube
MVFS 1 1 1 0.0 0.0 0.0
SMEV 1 1 1 2 2.0 0.0 0.0
SMEV 1 1 2 3 2.0 2.0 0.0
SMEV 1 1 3 4 0.0 2.0 0.0
SMEF 1 1 1 4 2
SWEEP 2 0.0 0.0 2.0

### Cylinder

//bdSolid* cube = Cube(1, 2.0f, 2.0f, 2.0f);
//bdSolid* cyl = Cyl(1, 2.0f, 2.0f, 32);
//bdSolid* sphere = Sphere(1, 2.0f, 32, 32);
//bdSolid* torus = Torus(1, 1.0f, 2.0f, 32, 32);
