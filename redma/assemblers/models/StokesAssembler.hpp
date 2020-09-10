// Reduced Modeling of Arteries (RedMA)
// Copyright (C) 2019  Luca Pegolotti
//
// RedMA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RedMA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef STOKESASSEMBLER_HPP
#define STOKESASSEMBLER_HPP

#include <redma/assemblers/abstract/aAssembler.hpp>
#include <redma/array/DistributedVector.hpp>
#include <redma/array/SparseMatrix.hpp>
#include <redma/geometry/BuildingBlock.hpp>

#include <redma/reduced_basis/BlockMDEIM.hpp>

#include <lifev/eta/expression/Integrate.hpp>
#include <lifev/core/filter/Exporter.hpp>
#include <lifev/core/filter/ExporterVTK.hpp>
#include <lifev/core/filter/ExporterHDF5.hpp>

#include <boost/filesystem.hpp>
// #include <Python.h>

// class BlockMDEIM;

namespace RedMA
{

class StokesAssembler
{
public:
    StokesAssembler(const DataContainer& data, SHP(TreeNode) treeNode);

    BlockVector getForcingTerm(const double& time) const;

    void addNeumannBCs(BlockVector& input, const double& time,
                       const BlockVector& sol);

    BlockMatrix assembleReducedStiffness(SHP(BCManager) bcManager,
                                         BlockMDEIMStructure* structure = nullptr);

    BlockMatrix assembleReducedMass(SHP(BCManager) bcManager,
                                    BlockMDEIMStructure* structure = nullptr);

    BlockMatrix assembleReducedDivergence(SHP(BCManager) bcManager,
                                          BlockMDEIMStructure* structure = nullptr);

    std::map<unsigned int, double> computeFlowRates(const BlockVector& sol,
                                                    bool verbose = false);

    // these are int_{Gamma_N} phi_i * n inlets (just to check that it is preserved)
    // and outlets (for boundary conditions)
    void assembleFlowRateVectors();

    // compute the jacobian of int_{Gamma_N}(int_{Gamma_N} u_i * n)phi_j
    void assembleFlowRateJacobians(SHP(BCManager) bcManager);

    SHP(VECTOREPETRA) assembleFlowRateVector(const GeometricFace& face);

    SHP(MATRIXEPETRA) assembleFlowRateJacobian(const GeometricFace& face);

    void addBackFlowStabilization(BlockVector input,
                                  const BlockVector& sol,
                                  const unsigned int& faceFlag);

    void exportNorms(double t);

    void initializePythonStructures();

    void computeWallShearStress(SHP(VECTOREPETRA) velocity,
                                SHP(VECTOREPETRA) WSS,
                                EPETRACOMM comm);

    void apply0DirichletBCsMatrix(SHP(BCManager) bcManager,
                                  BlockMatrix& matrix, double diagCoeff);

protected:

    BlockVector buildZeroVector() const;

    std::string                                       M_name;
    DataContainer                                     M_data;
    SHP(TreeNode)                                     M_treeNode;
    BlockMatrix                                       M_mass;
    BlockMatrix                                       M_stiffness;
    BlockMatrix                                       M_divergence;
    SHP(FESPACE)                                      M_velocityFESpace;
    SHP(FESPACE)                                      M_pressureFESpace;
    SHP(ETFESPACE3)                                   M_velocityFESpaceETA;
    SHP(ETFESPACE1)                                   M_pressureFESpaceETA;
    double                                            M_density;
    double                                            M_viscosity;
    SHP(VECTOREPETRA)                                 M_velocityExporter;
    SHP(VECTOREPETRA)                                 M_WSSExporter;
    SHP(VECTOREPETRA)                                 M_pressureExporter;
    SHP(MATRIXEPETRA)                                 M_massWall;
    SparseMatrix                                      M_massVelocity;
    SparseMatrix                                      M_massPressure;
    // first index is face flag
    std::map<unsigned int, SHP(VECTOREPETRA)>         M_flowRateVectors;
    std::map<unsigned int, BlockMatrix>               M_flowRateJacobians;

    BlockVector                                       M_extrapolatedSolution;

    // rb structures
    SHP(BlockMDEIM)                                   M_mdeimMass;
    SHP(BlockMDEIM)                                   M_mdeimStiffness;
    SHP(BlockMDEIM)                                   M_mdeimDivergence;
    SHP(RBBases)                                      M_bases;
    // PyObject*                                      M_pFunc;
    // PyObject*                                      M_pModule;

    SHP(VECTOREPETRA)                                 M_xs;
    SHP(VECTOREPETRA)                                 M_ys;
    SHP(VECTOREPETRA)                                 M_zs;
};

}

#endif // STOKESASSEMBLER_HPP
