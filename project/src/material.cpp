#include "material.h"
#include <cmath>
#include <vector>
#include <array>
#include <mpi.h>


mpmMaterialPoint_2D_Classic::mpmMaterialPoint_2D_Classic()
    : fMass(1.0), fVolumeInitial(1.0), fVolume(1.0),
      fElasticModulus(1.0), fPoissonRatio(0.3),
      v2Centroid{0.0, 0.0}, v2Velocity{0.0, 0.0},
      v2Momentum{0.0, 0.0}, v2ExternalForce{0.0, 0.0},
      v2Restraint{0.0, 0.0},
      v2Corner{{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}},
      m22DeformationGradient{{{1.0, 0.0}, {0.0, 1.0}}}, // identity matrix i.e zero deformation
      m22DeformationGradientIncrement{{{1.0, 0.0}, {0.0, 1.0}}},
      v3Strain{0.0, 0.0, 0.0}, v3Stress{0.0, 0.0, 0.0} {}


std::vector<mpmMaterialPoint_2D_Classic> createMaterialDomain(int nprocs, int procno,const std::array<double,2>& fCenter, double fRadius, double fOffset)
{
    std::vector<mpmMaterialPoint_2D_Classic> thisMaterialDomain;

    // to equidistribute the material points in a circular domain
    double start_angle = 2 * M_PI * procno / nprocs;
    double end_angle = 2 * M_PI * (procno + 1) / nprocs;
    
    for (double fy = -fRadius + 0.5 * fOffset; fy <= fRadius - 0.5 * fOffset; fy += fOffset) {
        for (double fx = -fRadius + 0.5 * fOffset; fx <= fRadius - 0.5 * fOffset; fx += fOffset) {
            if (fx * fx + fy * fy < fRadius * fRadius && fx * fx + fy * fy > 0) {
                double angle = std::atan2(fy, fx); 
                if (angle < 0) angle += 2 * M_PI;
                if (angle >= start_angle && angle < end_angle) {
                    mpmMaterialPoint_2D_Classic thisMaterialPoint;
                    thisMaterialPoint.v2Centroid = {fCenter[0] + fx, fCenter[1] + fy};
                    thisMaterialDomain.push_back(thisMaterialPoint);
                }
            }
        }
    }
    
    return thisMaterialDomain;
}

