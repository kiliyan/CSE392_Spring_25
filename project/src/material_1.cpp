#include "material.h"
#include <cmath>
#include <vector>
#include <array>
#include <mpi.h>

// Constructor definition
mpmMaterialPoint_2D_Classic::mpmMaterialPoint_2D_Classic()
    : fMass(1.0), fVolumeInitial(1.0), fVolume(1.0),
      fElasticModulus(1.0), fPoissonRatio(0.3),
      v2Centroid{0.0, 0.0}, v2Velocity{0.0, 0.0},
      v2Momentum{0.0, 0.0}, v2ExternalForce{0.0, 0.0},
      v2Restraint{0.0, 0.0},
      v2Corner{{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}},
      m22DeformationGradient{{{1.0, 0.0}, {0.0, 1.0}}}, // identity matrix i.e zero deformation
      m22DeformationGradientIncrement{{{1.0, 0.0}, {0.0, 1.0}}},
      v3Strain{0.0, 0.0, 0.0}, v3Stress{0.0, 0.0, 0.0}
{
}

// Function definition for creating circular material domain
std::vector<mpmMaterialPoint_2D_Classic> createMaterialDomain(int nprocs, int procno, const std::array<double, 2> &fCenter, double fRadius, double fOffset)

{
    std::vector<double> Coordinates;

    if (procno == 0)
    {
        // Just in case radius is not a multiple of offset
        fRadius = std::floor(fRadius / fOffset) * fOffset;
        // odd is x and even is y
        for (double fy = -fRadius + 0.5 * fOffset; fy <= fRadius - 0.5 * fOffset; fy += fOffset)
            for (double fx = -fRadius + 0.5 * fOffset; fx <= fRadius - 0.5 * fOffset; fx += fOffset)
            {
                if ((fx * fx + fy * fy) < (fRadius * fRadius))
                {

                    Coordinates.push_back(fCenter[0] + fx);
                    Coordinates.push_back(fCenter[1] + fy);
                }
            }
    }

    MPI_Datatype MPI_Point;
    MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_Point);
    MPI_Type_commit(&MPI_Point);

    int num_coords = 0;

    if (procno == 0)
    {
        num_coords = Coordinates.size() / 2;
    }

    MPI_Bcast(&num_coords, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk_size = num_coords / (nprocs - 1); // Divide the data equally among all processes but the root
    int sendCounts[nprocs];
    int remainder = num_coords % (nprocs - 1); // This much data will be sent to the root process
    sendCounts[0] = remainder;
    int displacements[nprocs] = {0};
 

    for (int i = 1; i < nprocs; i++)
    {
        sendCounts[i] = chunk_size;
        displacements[i] = 2 * (remainder + (i - 1) * chunk_size);
    }

    std::vector<double> localCoordinates(sendCounts[procno] * 2);

    if (!procno)
        MPI_Scatterv(Coordinates.data(), sendCounts, displacements, MPI_Point, localCoordinates.data(), remainder, MPI_Point, 0, MPI_COMM_WORLD);
    else
        MPI_Scatterv(Coordinates.data(), sendCounts, displacements, MPI_Point, localCoordinates.data(), chunk_size, MPI_Point, 0, MPI_COMM_WORLD);

    std::vector<mpmMaterialPoint_2D_Classic> thisMaterialDomain;
    mpmMaterialPoint_2D_Classic thisMaterialPoint;

    for (int i = 0; i < localCoordinates.size() / 2; i += 2)
    {
        thisMaterialPoint.v2Centroid = {localCoordinates[i], localCoordinates[i + 1]};
        thisMaterialDomain.push_back(thisMaterialPoint);
    }

    return thisMaterialDomain;
}
