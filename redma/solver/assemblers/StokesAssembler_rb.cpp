#include "StokesAssembler.hpp"

namespace RedMA
{

template <>
void
StokesAssembler<DenseVector, DenseMatrix>::
apply0DirichletBCsMatrix(BlockMatrix<DenseMatrix>& matrix, double diagCoeff) const
{
}

template <>
void
StokesAssembler<DenseVector, DenseMatrix>::
apply0DirichletBCs(BlockVector<DenseVector>& vector) const
{
}

template <>
void
StokesAssembler<DenseVector, DenseMatrix>::
applyDirichletBCs(const double& time, BlockVector<DenseVector>& vector) const
{
}

template <>
BlockMatrix<DenseMatrix>
StokesAssembler<DenseVector,DenseMatrix>::
assembleMass(BlockMDEIMStructure* structure)
{
    M_mdeimMass->setFESpace(M_velocityFESpace, 0);
    M_mdeimMass->setFESpace(M_pressureFESpace, 1);

    BlockMatrix<MatrixEp> reducedMass = assembleReducedMass(&M_mdeimMass->getMDEIMStructure());
    BlockMatrix<DenseMatrix> mass = M_mdeimMass->assembleProjectedMatrix(reducedMass);

    return mass;
}

template <>
BlockMatrix<DenseMatrix>
StokesAssembler<DenseVector,DenseMatrix>::
assembleStiffness(BlockMDEIMStructure* structure)
{
    M_mdeimStiffness->setFESpace(M_velocityFESpace, 0);
    M_mdeimStiffness->setFESpace(M_pressureFESpace, 1);

    BlockMatrix<MatrixEp> reducedStiffness = assembleReducedStiffness(&M_mdeimStiffness->getMDEIMStructure());
    BlockMatrix<DenseMatrix> stiffness = M_mdeimStiffness->assembleProjectedMatrix(reducedStiffness);

    return stiffness;
}

template <>
BlockMatrix<DenseMatrix>
StokesAssembler<DenseVector,DenseMatrix>::
assembleDivergence(BlockMDEIMStructure* structure)
{
    M_mdeimDivergence->setFESpace(M_velocityFESpace, 0);
    M_mdeimDivergence->setFESpace(M_pressureFESpace, 1);

    BlockMatrix<MatrixEp> reducedDivergence = assembleReducedDivergence(&M_mdeimDivergence->getMDEIMStructure());
    BlockMatrix<DenseMatrix> divergence = M_mdeimDivergence->assembleProjectedMatrix(reducedDivergence);

    return divergence;
}

template <>
void
StokesAssembler<DenseVector, DenseMatrix>::
exportSolution(const double& t, const BlockVector<DenseVector>& sol)
{
}

template <>
BlockVector<DenseVector>
StokesAssembler<DenseVector,DenseMatrix>::
getZeroVector() const
{
    BlockVector<DenseVector> retVec;

    return retVec;
}

template <>
BlockVector<RBVECTOR>
StokesAssembler<RBVECTOR, RBMATRIX>::
getLifting(const double& time) const
{
    BlockVector<RBVECTOR> lifting;

    return lifting;
}

}
