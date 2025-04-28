#include "basis.h"
#include "grid.h"          // Assuming you have classes like mpmGrid, mpmGridPoint
#include "material.h" // Assuming you have mpmMaterialPoint_2D_Classic
#include <cmath>
#include <array>
#include <utility> // for std::pair
#include <vector>



 double getShapeValue_Classic(const mpmMaterialPoint_2D_Classic& materialPoint, int index,const mpmGrid& grid) 
 {
    double fShapeValue = 0.0;

    std::array<double, 2> v2Distance = {materialPoint.v2Centroid[0] - grid.GridPoints_position[2*index],materialPoint.v2Centroid[1] - grid.GridPoints_position[2*index+1]};
    std::array<double, 2> v2CellLength = grid.v2Length_Cell;

    std::array<double, 2> v2ShapeValue = {0.0, 0.0};
    v2ShapeValue[0] = 1.0 - std::abs(v2Distance[0]) / v2CellLength[0];
    v2ShapeValue[1] = 1.0 - std::abs(v2Distance[1]) / v2CellLength[1];

    if (v2ShapeValue[0] < 0.0) v2ShapeValue[0] = 0.0;
    if (v2ShapeValue[1] < 0.0) v2ShapeValue[1] = 0.0;

    fShapeValue = v2ShapeValue[0] * v2ShapeValue[1];

    return fShapeValue;
}

std::array<double, 2> getShapeGradient_Classic(const mpmMaterialPoint_2D_Classic& materialPoint, int index, const mpmGrid& grid)
 {
    std::array<double, 2> v2Result = {0.0, 0.0};

    std::array<double, 2> v2Distance = {materialPoint.v2Centroid[0] - grid.GridPoints_position[2*index],materialPoint.v2Centroid[1] - grid.GridPoints_position[2*index+1]};
    std::array<double, 2> v2CellLength = grid.v2Length_Cell;

    std::array<double, 2> v2ShapeValue = {0.0, 0.0};
    v2ShapeValue[0] = 1.0 - std::abs(v2Distance[0]) / v2CellLength[0];
    v2ShapeValue[1] = 1.0 - std::abs(v2Distance[1]) / v2CellLength[1];

    if (v2ShapeValue[0] < 0.0) v2ShapeValue[0] = 0.0;
    if (v2ShapeValue[1] < 0.0) v2ShapeValue[1] = 0.0;

    v2Result[0] = -v2ShapeValue[1] * (v2Distance[0] >= 0.0 ? 1.0 : -1.0) / v2CellLength[0];
    v2Result[1] = -v2ShapeValue[0] * (v2Distance[1] >= 0.0 ? 1.0 : -1.0) / v2CellLength[1];

    return v2Result;
}


std::array<std::array<double, 2>, 2> matrixMultiply_2D(const std::array<std::array<double, 2>, 2>& matA,const std::array<std::array<double, 2>, 2>& matB)
 {
    std::array<std::array<double, 2>, 2> result = {{{0.0, 0.0}, {0.0, 0.0}}};

    for (int i = 0; i < 2; ++i) 
     for (int j = 0; j < 2; ++j) 
      for (int k = 0; k < 2; ++k) 
        result[i][j] += matA[i][k] * matB[k][j];
            
    return result;
}









