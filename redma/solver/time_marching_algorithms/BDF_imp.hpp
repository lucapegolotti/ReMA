namespace RedMA
{

template <class InVectorType, class InMatrixType>
BDF<InVectorType, InMatrixType>::
BDF(const DataContainer& data,
    SHP(aAssembler<InVectorType COMMA InMatrixType>) assembler) :
  aTimeMarchingAlgorithm<InVectorType, InMatrixType>(data, assembler),
  M_order(data("time_discretization/order",2))
{
    setup();
}

template <class InVectorType, class InMatrixType>
void
BDF<InVectorType, InMatrixType>::
setup()
{
    printlog(GREEN, "[BDF] initializing time advancing scheme ...", this->M_data.getVerbose());
    M_coefficients.reserve(M_order);

    for (unsigned int i = 0; i < M_order; i++)
    {
        M_prevSolutions.push_back(this->M_assembler->getZeroVector());
    }

    if (M_order == 1)
    {
        M_coefficients[0] = 1.0;
        M_rhsCoeff = 1.0;
    }
    else if (M_order == 2)
    {
        M_coefficients[0] = -4.0/3.0;
        M_coefficients[1] = 1.0/3.0;
        M_rhsCoeff = 2.0/3.0;
    }
    else if (M_order == 3)
    {
        M_coefficients[0] = -18.0/11.0;
        M_coefficients[1] = 9.0/11.0;
        M_coefficients[2] = -2.0/11.0;
        M_rhsCoeff = 6.0/11.0;
    }
    else
    {
        throw new Exception("BDF scheme of requested order not implemented");
    }
    printlog(GREEN, "done\n", this->M_data.getVerbose());
}

template <class InVectorType, class InMatrixType>
BlockVector<InVectorType>
BDF<InVectorType, InMatrixType>::
advance(const double& time, double& dt, int& status)
{
    typedef BlockVector<InVectorType>               BV;
    typedef BlockMatrix<InMatrixType>               BM;

    // we set the initial guess equal to the last solution
    BV initialGuess;
    initialGuess.softCopy(M_prevSolutions[0]);

    FunctionFunctor<BV,BV> fct(
        [this,time,dt](BV sol)
    {
        BM mass(this->M_assembler->getMass(time+dt, sol));
        BV f(this->M_assembler->getRightHandSide(time+dt, sol));
        BV prevContribution;

        unsigned int count = 0;
        for (BV vec : M_prevSolutions)
        {
            prevContribution += vec * M_coefficients[count];
            count++;
        }

        BV retVec;
        retVec.softCopy(mass * (sol + prevContribution));
        f *= (-1. * M_rhsCoeff * dt);
        retVec += f;

        return retVec;
    });

    FunctionFunctor<BV,BM> jac(
        [this,time,dt](BV sol)
    {
        BM retMat = this->M_assembler->getMass(time+dt, sol);
        BM jacRhs;

        jacRhs = this->M_assembler->getJacobianRightHandSide(time+dt, sol);
        jacRhs *= (-1. * M_rhsCoeff * dt);
        retMat += jacRhs;
        return retMat;
    });

    return this->M_systemSolver.solve(fct, jac, initialGuess, status);
}

}
