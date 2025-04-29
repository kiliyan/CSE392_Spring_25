#pragma once

#include "grid.h"         
#include "material.h"      
#include <cmath>
#include <array>
#include <utility> 

double getShapeValue_Classic(const mpmMaterialPoint_2D_Classic& materialPoint, int index, const mpmGrid& grid);
std::array<double, 2> getShapeGradient_Classic(const mpmMaterialPoint_2D_Classic& materialPoint,int index, const mpmGrid& grid);
std::array<std::array<double, 2>, 2> matrixMultiply_2D(const std::array<std::array<double, 2>, 2>& matA,const std::array<std::array<double, 2>, 2>& matB);

                                                                    
                                                                   

