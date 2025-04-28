#pragma once

#include <array>
#include <vector>
#include "material.h" // Assuming this includes the definition of mpmMaterialPoint_2D_Classic

// Grid container class
class mpmGrid
{
public:
    std::array<int, 2> v2Nodes;
    int iNodes;
    std::array<double, 2> v2Length_Cell, v2Length_CellI, v2Length_Grid;
    std::vector<double> GridPoints_position, GridPoints_force, GridPoints_mass, GridPoints_momentum, GridPoints_v2Fixed;

    mpmGrid(double fGL_x, double fGL_y, int iN_x, int iN_y);

    // Utility function: convert 2D index to 1D
    static int index2DTo1D(int i, int j, int nColumns, int nRows);

    // Get adjacent grid points for a material point
    std::vector<int> getAdjacentGridPoints(const mpmMaterialPoint_2D_Classic &thisMaterialPoint) const;
};
