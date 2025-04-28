
#pragma once

#include <array>
#include <vector>
#include <mpi.h>

// Material point container class
class mpmMaterialPoint_2D_Classic
{
public:
    double fMass, fVolumeInitial, fVolume, fElasticModulus, fPoissonRatio;

    std::array<double, 2> v2Centroid, v2Velocity, v2Momentum, v2ExternalForce, v2Restraint;
    std::array<std::array<double, 2>, 2> m22DeformationGradient, m22DeformationGradientIncrement;
    std::array<std::array<double, 2>, 4> v2Corner; // 4 corners, each 2D
    std::array<double, 3> v3Strain;                // xx, yy, xy
    std::array<double, 3> v3Stress;

    // Constructor declaration
    mpmMaterialPoint_2D_Classic();
};

// Function to create a circular material domain
std::vector<mpmMaterialPoint_2D_Classic> createMaterialDomain(int nprocs, int procno, const std::array<double, 2> &fCenter, double fRadius, double fOffset);
