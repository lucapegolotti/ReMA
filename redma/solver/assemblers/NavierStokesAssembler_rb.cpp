#include "NavierStokesAssembler.hpp"

namespace RedMA
{

template <>
void
NavierStokesAssembler<DenseVector,DenseMatrix>::
addConvectiveMatrixRightHandSide(const BlockVector<DenseVector>& sol,
                                 BlockMatrix<DenseMatrix>& mat)
{
    LifeV::LifeChrono chrono;
    chrono.start();

    std::string msg = "[NavierStokesAssembler] assembling convective matrix ...";
    printlog(YELLOW, msg, this->M_data.getVerbose());

    using namespace LifeV;
    using namespace ExpressionAssembly;

    SHP(MATRIXEPETRA)  convectiveMatrix(new MATRIXEPETRA(M_velocityFESpace->map()));
    SHP(VECTOREPETRA)  velocityRepeated(new VECTOREPETRA(M_velocityFESpace->map(),
                                                         Repeated));

    M_bases->reconstructFEFunction(velocityRepeated, sol.block(0), 0);

    integrate(elements(M_velocityFESpaceETA->mesh()),
               M_velocityFESpace->qr(),
               M_velocityFESpaceETA,
               M_velocityFESpaceETA,
               value(this->M_density) *
               dot(value(M_velocityFESpaceETA , *velocityRepeated) * grad(phi_j),
               phi_i)
             ) >> convectiveMatrix;
    convectiveMatrix->globalAssemble();

    BlockMatrix<MatrixEp> convectiveMatrixWrap(2,2);
    convectiveMatrixWrap.block(0,0).data() = convectiveMatrix;

    this->M_bcManager->apply0DirichletMatrix(convectiveMatrixWrap, this->getFESpaceBCs(),
                                             this->getComponentBCs(), 0.0);

    DenseMatrix convectiveMatrixProjected = M_bases->matrixProject(convectiveMatrixWrap.block(0,0),
                                                                   0, 0);

    mat.block(0,0) -= convectiveMatrixProjected;

    msg = "done, in ";
    msg += std::to_string(chrono.diff());
    msg += " seconds\n";
    printlog(YELLOW, msg, this->M_data.getVerbose());
}

template <>
void
NavierStokesAssembler<DenseVector,DenseMatrix>::
addConvectiveTermJacobianRightHandSide(const BlockVector<DenseVector>& sol,
                                       const BlockVector<DenseVector>& lifting,
                                       BlockMatrix<DenseMatrix>& mat)
{
    LifeV::LifeChrono chrono;
    chrono.start();

    std::string msg = "[NavierStokesAssembler] assembling convective jacobian matrix ...";
    printlog(YELLOW, msg, this->M_data.getVerbose());

    using namespace LifeV;
    using namespace ExpressionAssembly;

    SHP(MATRIXEPETRA)  convectiveMatrix(new MATRIXEPETRA(M_velocityFESpace->map()));
    SHP(VECTOREPETRA)  velocityRepeated(new VECTOREPETRA(M_velocityFESpace->map(),
                                                         Repeated));

    M_bases->reconstructFEFunction(velocityRepeated, sol.block(0), 0);

    integrate(elements(M_velocityFESpaceETA->mesh()),
               M_velocityFESpace->qr(),
               M_velocityFESpaceETA,
               M_velocityFESpaceETA,
               value(this->M_density) *
               dot(
               (
               value(M_velocityFESpaceETA , *velocityRepeated) * grad(phi_j) +
               phi_j * grad(M_velocityFESpaceETA , *velocityRepeated)
               ),
               phi_i)
             ) >> convectiveMatrix;

    convectiveMatrix->globalAssemble();

    BlockMatrix<MatrixEp> convectiveMatrixWrap(2,2);
    convectiveMatrixWrap.block(0,0).data() = convectiveMatrix;

    this->M_bcManager->apply0DirichletMatrix(convectiveMatrixWrap, this->getFESpaceBCs(),
                                             this->getComponentBCs(), 0.0);

    DenseMatrix convectiveMatrixProjected = M_bases->matrixProject(convectiveMatrixWrap.block(0,0), 0, 0);

    mat.block(0,0) -= convectiveMatrixProjected;

    msg = "done, in ";
    msg += std::to_string(chrono.diff());
    msg += " seconds\n";
    printlog(YELLOW, msg, this->M_data.getVerbose());
}

template <>
BlockMatrix<DenseMatrix>
NavierStokesAssembler<DenseVector, DenseMatrix>::
getMass(const double& time, const BlockVector<DenseVector>& sol)
{
    BlockMatrix<DenseMatrix> retMat;
    retMat.hardCopy(this->M_mass);

    return retMat;
}

template <>
BlockMatrix<DenseMatrix>
NavierStokesAssembler<DenseVector, DenseMatrix>::
getMassJacobian(const double& time, const BlockVector<DenseVector>& sol)
{
    BlockMatrix<DenseMatrix> retMat(this->M_nComponents, this->M_nComponents);

    return retMat;
}

// template <>
// BlockVector<DenseVector>
// NavierStokesAssembler<DenseVector, DenseMatrix>::
// getRightHandSide(const double& time, const BlockVector<DenseVector>& sol)
// {
//     BlockVector<DenseVector> retVec;
//     BlockMatrix<DenseMatrix> systemMatrix;
//
//     systemMatrix.resize(this->M_nComponents, this->M_nComponents);
//     systemMatrix += this->M_stiffness;
//     systemMatrix += this->M_divergence;
//     systemMatrix *= (-1.0);
//
//     this->addConvectiveMatrixRightHandSide(sol, systemMatrix);
//
//     retVec.softCopy(systemMatrix * sol);
//
//     // this->addNeumannBCs(retVec, time, sol);
//
//     return retVec;
// }

template <>
BlockVector<DenseVector>
NavierStokesAssembler<DenseVector, DenseMatrix>::
getRightHandSide(const double& time, const BlockVector<DenseVector>& sol)
{
    LifeV::LifeChrono chrono;
    chrono.start();

    std::string msg = "[NavierStokesAssembler] computing rhs ...";
    printlog(YELLOW, msg, this->M_data.getVerbose());

    using namespace LifeV;
    using namespace ExpressionAssembly;

    BlockVector<DenseVector> retVec;
    BlockMatrix<DenseMatrix> systemMatrix;

    systemMatrix.resize(this->M_nComponents, this->M_nComponents);
    systemMatrix += this->M_stiffness;
    systemMatrix += this->M_divergence;
    systemMatrix *= (-1.0);

    retVec.softCopy(systemMatrix * sol);

    SHP(VECTOREPETRA)  convectiveTerm(new VECTOREPETRA(M_velocityFESpace->map()));
    SHP(VECTOREPETRA)  velocityRepeated(new VECTOREPETRA(M_velocityFESpace->map()));

    if (M_extrapolatedSolution.nRows() == 0)
        M_bases->reconstructFEFunction(velocityRepeated, sol.block(0), 0);
    else
        M_bases->reconstructFEFunction(velocityRepeated, M_extrapolatedSolution.block(0), 0);

    integrate(elements(M_velocityFESpaceETA->mesh()),
               M_velocityFESpace->qr(),
               M_velocityFESpaceETA,
               value(this->M_density) *
               dot(value(M_velocityFESpaceETA , *velocityRepeated) *
                   grad(M_velocityFESpaceETA , *velocityRepeated),
               phi_i)
             ) >> convectiveTerm;
    convectiveTerm->globalAssemble();

    BlockVector<VectorEp> convectiveTermWrap(2);
    convectiveTermWrap.block(0).data() = convectiveTerm;

    M_bcManager->apply0DirichletBCs(convectiveTermWrap,
                                    getFESpaceBCs(),
                                    getComponentBCs());

    BlockVector<DenseVector> convectiveTermProjected = M_bases->leftProject(convectiveTermWrap);

    retVec -= convectiveTermProjected;
    // this->addNeumannBCs(retVec, time, sol);

    msg = "done, in ";
    msg += std::to_string(chrono.diff());
    msg += " seconds\n";
    printlog(YELLOW, msg, this->M_data.getVerbose());

    return retVec;
}

template <>
BlockMatrix<DenseMatrix>
NavierStokesAssembler<DenseVector, DenseMatrix>::
getJacobianRightHandSide(const double& time,
                         const BlockVector<DenseVector>& sol)
{
    BlockMatrix<DenseMatrix> retMat;
    retMat = StokesAssembler<DenseVector,DenseMatrix>::getJacobianRightHandSide(time, sol);

    // this->addConvectiveTermJacobianRightHandSide(sol, this->getZeroVector(), retMat);

    return retMat;
}


}
