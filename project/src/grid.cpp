#include "grid.h"
#include <cmath>
#include <cstdio>

// Constructor for mpmGrid
mpmGrid::mpmGrid(double fGL_x, double fGL_y, int iN_x, int iN_y)
{

    GridPoints_v2Fixed{false};
    GridPoints_position{0.0};
    GridPoints_mass(0.0);
    GridPoints_force{0.0};
    GridPoints_momentum{0.0};
    v2Length_Grid = {fGL_x, fGL_y};
    v2Nodes = {iN_x, iN_y};
    iNodes = iN_x * iN_y;

    v2Length_Cell[0] = fGL_x / (iN_x - 1.0);
    v2Length_Cell[1] = fGL_y / (iN_y - 1.0);

    v2Length_CellI[0] = 1.0 / v2Length_Cell[0];
    v2Length_CellI[1] = 1.0 / v2Length_Cell[1];

    // GridPoints.resize(iNodes); // x y both

    for (int j = 1; j <= iN_y; ++j)
    {
        for (int i = 1; i <= iN_x; ++i)
        {
            double x = (i - 1) * v2Length_Cell[0];
            double y = (j - 1) * v2Length_Cell[1];

            int index = index2DTo1D(i, j, iN_x, iN_y);

            //GridPoints[index] = mpmGridPoint();
            //GridPoints[index].v2Fixed = {false, false};
            //GridPoints[index].v2Position = {x, y};

            // Set position
            GridPoints_position[2 * index + 0] = x; // x-coordinate
            GridPoints_position[2 * index + 1] = y; // y-coordinate

            // Set fixed flags (false for both x and y directions)
            GridPoints_v2Fixed[2 * index + 0] = false;
            GridPoints_v2Fixed[2 * index + 1] = false;
        }
    }
}

// Utility function: convert 2D index to 1D
int mpmGrid::index2DTo1D(int i, int j, int nColumns, int nRows) {
    int index = nColumns * (j - 1) + (i - 1); // Note: 0-based indexing
    if (index >= nRows * nColumns || index < 0) {
        printf("Index out of bounds: i, j: %d, %d \n", i, j);
    }
    return index;
}
// Get adjacent grid points for a material point
std::vector<int> mpmGrid::getAdjacentGridPoints(const mpmMaterialPoint_2D_Classic &thisMaterialPoint) const
{
    std::array<double, 2> v2Coordinate = thisMaterialPoint.v2Centroid;
    double fLength_Cell_x = v2Length_CellI[0];
    double fLength_Cell_y = v2Length_CellI[1];

    int iBottomLeft_i = static_cast<int>(std::floor(v2Coordinate[0] * fLength_Cell_x) + 1.0);
    int iBottomLeft_j = static_cast<int>(std::floor(v2Coordinate[1] * fLength_Cell_y) + 1.0);

    if (iBottomLeft_j < 0 || iBottomLeft_j > v2Nodes[1])
    {
        printf("Index out of bounds: j: %d \n", iBottomLeft_j);
        printf("v2Coordinate[2]: %e \n", v2Coordinate[1]);
    }

    int iIndex = index2DTo1D(iBottomLeft_i, iBottomLeft_j, v2Nodes[0], v2Nodes[1]);
    int jIndex = index2DTo1D(iBottomLeft_i, iBottomLeft_j + 1, v2Nodes[0], v2Nodes[1]);

    std::vector<int> thisAdjacentGridPoints = {iIndex, jIndex, iIndex + 1, jIndex + 1};

    return thisAdjacentGridPoints;
}
