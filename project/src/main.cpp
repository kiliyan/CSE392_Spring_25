
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include "cxxopts.hpp"
#include "material.h"
#include "grid.h"
#include "basis.h"
#include <mpi.h>

int main(int argc, char **argv)
{

    cxxopts::Options options("cxxopts", " Elastic 2D collision using MPM");
    options.add_options()("h,help", "usage information")("o,offset", "distance between material points", cxxopts::value<double>()->default_value("0.0125"))("g,grid", "grid cells in each row/column", cxxopts::value<int>()->default_value("20"));

    auto result = options.parse(argc, argv);
    if (result.count("help") > 0)
    {
        std::cout << options.help() << '\n';
        return 0;
    }

    auto fOffset = result["offset"].as<double>();
    auto nGrid = result["grid"].as<int>();

    MPI_Init(&argc, &argv); // Initialize MPI

    int procno, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &procno); // Get process rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // Get number of processes

    // Problem parameters
    const double fRadius = 0.2;
    const double fGravity = 0.0;
    const double density = 1000.0;
    const double youngModulus = 1000.0;
    const double poissonRatio = 0.3;
    double InitialEnergy = 0.0;
    const std::vector<double> velocity = {.1, .1};

    // Grid creation (Lx, Ly, nx, ny) This is not parllelised. Discussed in the pdf
    mpmGrid thisGrid(1.0, 1.0, nGrid + 1, nGrid + 1);

    // All material points
    std::vector<mpmMaterialPoint_2D_Classic> allMaterialPoint;

    // First material domain
    auto Materialdomain1 = createMaterialDomain(nprocs, procno, {0.2, 0.2}, 0.2, fOffset);
    for (auto &mp : Materialdomain1)
    {
        double fVolume = fOffset * fOffset;
        double fMass = density * fVolume;
        mp.fMass = fMass;
        mp.fVolumeInitial = fVolume;
        mp.fVolume = fVolume;
        mp.fElasticModulus = youngModulus;
        mp.fPoissonRatio = poissonRatio;

        mp.v2Velocity = {velocity[0], velocity[1]};
        mp.v2Momentum = {fMass * mp.v2Velocity[0], fMass * mp.v2Velocity[1]};
        mp.v2ExternalForce = {0.0, -fGravity * fMass};

        allMaterialPoint.push_back(mp);
        InitialEnergy += 0.5 * (mp.v2Velocity[0] * mp.v2Velocity[0] + mp.v2Velocity[1] * mp.v2Velocity[1]) * mp.fMass;
    }
    // Second material domain
    auto Materialdomain2 = createMaterialDomain(nprocs, procno, {0.8, 0.8}, 0.2, fOffset);
    for (auto &mp : Materialdomain2)
    {
        double fVolume = fOffset * fOffset;
        double fMass = density * fVolume;
        mp.fMass = fMass;
        mp.fVolumeInitial = fVolume;
        mp.fVolume = fVolume;
        mp.fElasticModulus = youngModulus;
        mp.fPoissonRatio = poissonRatio;

        mp.v2Velocity = {-velocity[0], -velocity[1]};
        mp.v2Momentum = {fMass * mp.v2Velocity[0], fMass * mp.v2Velocity[1]};
        mp.v2ExternalForce = {0.0, -fGravity * fMass};

        allMaterialPoint.push_back(mp);
        InitialEnergy += 0.5 * (mp.v2Velocity[0] * mp.v2Velocity[0] + mp.v2Velocity[1] * mp.v2Velocity[1]) * mp.fMass;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double TotalInitialEnergy = 0.0;
    MPI_Reduce(InitialEnergy, TotalInitialEnergy, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD)

        // ---------------------------------------------------------------------------
        // Information about created domain
        // double fMassTotal = 0.0;
        // for (const auto &mp : allMaterialPoint)
        //{
        //    fMassTotal += mp.fMass;
        //}
        // std::cout << "Initial configuration:\n";
        // std::cout << "    Mass: " << std::scientific << fMassTotal << "\n";
        // std::cout << "Total number of material points: " << allMaterialPoint.size() << "\n";

        // ---------------------------------------------------------------------------
        // Timers
        // ---------------------------------------------------------------------------

        double fTimeEnd = 3.5;
    double fTimeIncrement = 1.0e-3;
    double fMarkedParticle_y = allMaterialPoint[0].v2Centroid[1];
    int time_steps = static_cast<int>(fTimeEnd / fTimeIncrement);
    std::vector<double> Energy(2 * time_steps, 0.0); // 0: Kinetic, 1: Strain

    // ---------------------------------------------------------------------------
    // Main analysis loop

    for (int i = 0; i < time_steps; i++)
    {
        std::cout << "Time: " << i * fTimeIncrement << "\n";
        // tic();

        // Reset grid
        for (int idx = 0; idx < thisGrid.iNodes; idx++)
        {
            thisGrid.GridPoints_mass[idx] = 0.0;
            thisGrid.GridPoints_momentum[2 * idx] = 0.0;
            thisGrid.GridPoints_momentum[2 * idx + 1] = 0.0;
            thisGrid.GridPoints_force[2 * idx] = 0.0;
            thisGrid.GridPoints_force[2 * idx + 1] = 0.0;
        }

        // Material to grid
        for (auto &mp : allMaterialPoint)
        {
            auto adjacentGridPoints = thisGrid.getAdjacentGridPoints(mp);
            for (auto idx : adjacentGridPoints)
            {
                // mpmGridPoint &gp = thisGrid.GridPoints[idx];
                auto fShapeValue = getShapeValue_Classic(mp, idx, thisGrid);
                auto v2ShapeGradient = getShapeGradient_Classic(mp, idx, thisGrid);

                thisGrid.GridPoints_mass[idx] += fShapeValue * mp.fMass;
                thisGrid.GridPoints_momentum[2 * idx] += fShapeValue * mp.fMass * mp.v2Velocity[0];
                thisGrid.GridPoints_momentum[2 * idx + 1] += fShapeValue * mp.fMass * mp.v2Velocity[1];

                double fVolume = mp.fVolume;
                thisGrid.GridPoints_force[2 * idx] -= fVolume * (v2ShapeGradient[0] * mp.v3Stress[0] + v2ShapeGradient[1] * mp.v3Stress[2]);
                thisGrid.GridPoints_force[2 * idx + 1] -= fVolume * (v2ShapeGradient[1] * mp.v3Stress[1] + v2ShapeGradient[0] * mp.v3Stress[2]);

                thisGrid.GridPoints_force[2 * idx] += fShapeValue * mp.v2ExternalForce[0];
                thisGrid.GridPoints_force[2 * idx + 1] += fShapeValue * mp.v2ExternalForce[1];
            }
        }

        // Update grid momentum and apply BCs
        for (int idx = 0; idx < thisGrid.iNodes; idx++)
        {
            thisGrid.GridPoints_momentum[2 * idx] += thisGrid.GridPoints_force[2 * idx] * fTimeIncrement;
            thisGrid.GridPoints_momentum[2 * idx + 1] += thisGrid.GridPoints_force[2 * idx + 1] * fTimeIncrement;

            if (GridPoints_v2Fixed[2 * idx])
            {
                thisGrid.GridPoints_momentum[2 * idx] = 0.0;
                thisGrid.GridPoints_force[2 * idx] = 0.0;
            }
            if (GridPoints_v2Fixed[2 * idx + 1])
            {
                thisGrid.GridPoints_momentum[2 * idx + 1] = 0.0;
                thisGrid.GridPoints_force[2 * idx + 1] = 0.0;
            }
        }

        // pack , send and  unpack data

        std::vector<double> information(5 * thisGrid.iNodes, 0.0);
        // Perform MPI_Allreduce across all processes to sum up mass, momentum, and force
        for (int idx = 0; idx < thisGrid.iNodes; idx++)
        {
            information[5 * idx + 0] = thisGrid.GridPoints_mass[idx];
            information[5 * idx + 1] = thisGrid.GridPoints_momentum[2 * idx];
            information[5 * idx + 2] = thisGrid.GridPoints_momentum[2 * idx + 1];
            information[5 * idx + 3] = thisGrid.GridPoints_force[2 * idx];
            information[5 * idx + 4] = thisGrid.GridPoints_force[2 * idx + 1];
        }

        MPI_Allreduce(MPI_IN_PLACE, information.data(), 5 * thisGrid.iNodes, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        for (int idx = 0; idx < thisGrid.iNodes; idx++)
        {
            thisGrid.GridPoints_mass[idx] = information[5 * idx];
            thisGrid.GridPoints_momentum[2 * idx] = information[5 * idx + 1];
            thisGrid.GridPoints_momentum[2 * idx + 1] = information[5 * idx + 2];
            thisGrid.GridPoints_force[2 * idx] = information[5 * idx + 3];
            thisGrid.GridPoints_force[2 * idx + 1] = information[5 * idx + 4];
        }

        // Grid to material
        for (auto &mp : allMaterialPoint)
        {
            auto adjacentGridPoints = thisGrid.getAdjacentGridPoints(mp);
            std::array<double, 2> v2CentroidIncrement = {0.0, 0.0};

            for (auto idx : adjacentGridPoints)
            {
                // mpmGridPoint &gp = thisGrid.GridPoints[idx];
                auto fShapeValue = getShapeValue_Classic(mp, idx, thisGrid);
                auto v2ShapeGradient = getShapeGradient_Classic(mp, idx, thisGrid);

                double tmass = thisGrid.GridPoints_mass[idx]; // temporary assignment
                std::vector<double> gp_momentum = {thisGrid.GridPoints_momentum[2 * idx], thisGrid.GridPoints_momentum[2 * idx + 1]};
                std::array<double, 2> v2GridPointVelocity = {0.0, 0.0};
                if (tmass > 1.0e-8)
                {
                    v2GridPointVelocity = {gp_momentum[0] / tmass, gp_momentum[1] / tmass};
                    v2CentroidIncrement[0] += (fShapeValue * gp_momentum[0] / tmass) * fTimeIncrement;
                    v2CentroidIncrement[1] += (fShapeValue * gp_momentum[1] / tmass) * fTimeIncrement;
                    mp.v2Velocity[0] += (fShapeValue * thisGrid.GridPoints_force[2 * idx] / tmass) * fTimeIncrement;
                    mp.v2Velocity[1] += (fShapeValue * thisGrid.GridPoints_force[2 * idx + 1] / tmass) * fTimeIncrement;
                }

                // F + =
                for (int i = 0; i < 2; ++i)
                    for (int j = 0; j < 2; ++j)
                        mp.m22DeformationGradientIncrement[i][j] += fTimeIncrement * v2GridPointVelocity[i] * v2ShapeGradient[j];
            }

            mp.v2Centroid[0] += v2CentroidIncrement[0];
            mp.v2Centroid[1] += v2CentroidIncrement[1];

            mp.m22DeformationGradient = matrixMultiply_2D(
                mp.m22DeformationGradientIncrement, mp.m22DeformationGradient);

            std::array<double, 3> v3StrainIncrement = {mp.m22DeformationGradientIncrement[0][0] - 1.0,
                                                       mp.m22DeformationGradientIncrement[1][1] - 1.0,
                                                       mp.m22DeformationGradientIncrement[0][1] + mp.m22DeformationGradientIncrement[1][0]};

            mp.m22DeformationGradientIncrement = {{{1.0, 0.0}, {0.0, 1.0}}};

            double fE = mp.fElasticModulus;
            double fNu = mp.fPoissonRatio;
            double fConstant = fE / (1.0 + fNu) / (1.0 - 2.0 * fNu);

            mp.v3Strain[0] += v3StrainIncrement[0];
            mp.v3Strain[1] += v3StrainIncrement[1];
            mp.v3Strain[2] += v3StrainIncrement[2];

            mp.v3Stress[0] += fConstant * ((1.0 - fNu) * v3StrainIncrement[0] + fNu * v3StrainIncrement[1]);
            mp.v3Stress[1] += fConstant * ((1.0 - fNu) * v3StrainIncrement[1] + fNu * v3StrainIncrement[0]);
            mp.v3Stress[2] += fConstant * ((0.5 - fNu) * v3StrainIncrement[2]);

            double determinant = mp.m22DeformationGradient[0][0] * mp.m22DeformationGradient[1][1] - mp.m22DeformationGradient[0][1] * mp.m22DeformationGradient[1][0];
            mp.fVolume = determinant * mp.fVolumeInitial;
            mp.v2Momentum[0] = mp.v2Velocity[0] * mp.fMass;
            mp.v2Momentum[1] = mp.v2Velocity[1] * mp.fMass;
        }

        // fProfiler_Grid2Particle += toq();
        // fResultTime += fTimeIncrement ;

        double fStrainEnergy = 0.0;
        double fKineticEnergy = 0.0;
        for (auto thisMaterialPoint : allMaterialPoint)
        {
            fStrainEnergy += 0.5 * thisMaterialPoint.v3Strain[0] * thisMaterialPoint.v3Stress[0] * thisMaterialPoint.fVolume;
            fStrainEnergy += 0.5 * thisMaterialPoint.v3Strain[1] * thisMaterialPoint.v3Stress[1] * thisMaterialPoint.fVolume;
            fStrainEnergy += 0.5 * thisMaterialPoint.v3Strain[2] * thisMaterialPoint.v3Stress[2] * thisMaterialPoint.fVolume;

            fKineticEnergy += 0.5 * (thisMaterialPoint.v2Velocity[0] * thisMaterialPoint.v2Velocity[0] + thisMaterialPoint.v2Velocity[1] * thisMaterialPoint.v2Velocity[1]) * thisMaterialPoint.fMass;
        }

        Energy[2 * i] = fKineticEnergy;
        Energy[2 * i + 1] = fStrainEnergy;
    }
    // (Skipping plot and energy tracking for now)

    std::vector<double> GlobalEnergy;
    if (procno == 0) GlobalEnergy.resize(2 * time_steps, 0.0);
    
    MPI_Reduce(Energy.data(), GlobalEnergy.data(), 2 * time_steps, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (!procno)
    {
        double error = 0.0;
        std::ofstream outFile("results.csv");
        outFile << "Time, Kinetic Energy, Strain Energy, Total Energy" << std::endl;
        for (int i = 0; i < time_steps; i++)
        {
            error += std::max(std::abs(TotalInitialEnergy - GlobalEnergy[2 * i] - GlobalEnergy[2 * i + 1]), 1e-8);
            outFile << i * fTimeIncrement << ", " << GlobalEnergy[2i] << ", " << GlobalEnergy[2 * i + 1] << ", " << GlobalEnergy[2 * i] + GlobalEnergy[2 * i + 1] << std::endl;
        }
        outFile.close();
        std::cout << "Initial energy: " << TotalInitialEnergy << "\n";
        std::cout << "Mean error: " << error / (fTimeEnd / fTimeIncrement) << "\n";
        std::cout << "Error: " << error << "\n";
    }
    MPI_Finalize(); // Finalize MPI
    return 0;
}
