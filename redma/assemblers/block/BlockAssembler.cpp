#include "BlockAssembler.hpp"

namespace RedMA
{
//
BlockAssembler::
BlockAssembler(const DataContainer& data, const TreeStructure& tree,
               SHP(DefaultAssemblers) defAssemblers) :
  aAssembler(data),
  M_tree(tree)
{
    this->M_defaultAssemblers = defAssemblers;
    setup();
}

void
BlockAssembler::
checkStabTerm(const SHP(aVector)& sol) const
{
    // for (auto as: M_dualAssemblers)
    //     as->checkStabilizationTerm(sol, M_primalAssemblers.size());
}

void
BlockAssembler::
setExporter()
{
    for (auto as: M_primalAssemblers)
        as.second->setExporter();
}

void
BlockAssembler::
initializeFEspaces()
{
    for (auto as: M_primalAssemblers)
        as.second->initializeFEspaces();
}

SHP(aVector)
BlockAssembler::
getLifting(const double& time) const
{
    SHP(BlockVector) retVec(new BlockVector(M_numberBlocks));

    for (auto as : M_primalAssemblers)
        retVec->setBlock(as.first,as.second->getLifting(time));

    return retVec;
}

void
BlockAssembler::
setDefaultAssemblers(SHP(DefaultAssemblers) defAssemblers)
{
    this->M_defaultAssemblers = defAssemblers;
    for (auto as : M_primalAssemblers)
        as.second->setDefaultAssemblers(this->M_defaultAssemblers);
}

void
BlockAssembler::
applyGlobalPiola(SHP(aVector) solution, bool inverse)
{
    unsigned int count = 0;
    for (auto as : M_primalAssemblers)
    {
        as.second->applyPiola(
            std::static_pointer_cast<BlockVector>(solution->block(count)),
            inverse);
        count = count + 1;
    }
}

void
BlockAssembler::
apply0DirichletBCsMatrix(SHP(aMatrix) matrix, double diagCoeff) const
{
    for (auto as : M_primalAssemblers)
        as.second->apply0DirichletBCsMatrix(
            std::static_pointer_cast<BlockMatrix>(matrix->block(as.first, as.first)),
            diagCoeff);
}

void
BlockAssembler::
apply0DirichletBCs(SHP(aVector) initialGuess) const
{
    unsigned int count = 0;
    for (auto as : M_primalAssemblers)
    {
        as.second->apply0DirichletBCs(
            std::static_pointer_cast<BlockVector>(initialGuess->block(as.first)));
    }
}

void
BlockAssembler::
applyDirichletBCs(const double& time, SHP(aVector) initialGuess) const
{
    for (auto as : M_primalAssemblers)
        as.second->applyDirichletBCs(time,
            std::static_pointer_cast<BlockVector>(initialGuess->block(as.first)));
}

SHP(aVector)
BlockAssembler::
getZeroVector() const
{
    SHP(BlockVector) retVec(new BlockVector(M_numberBlocks));

    for (auto as : M_primalAssemblers)
        retVec->setBlock(as.first,as.second->getZeroVector());

    unsigned int count = M_primalAssemblers.size();

    for (auto as : M_dualAssemblers)
    {
        auto newvec = as->getZeroVector();
        retVec->setBlock(count, newvec);
        count++;
    }
    // retVec->close();
    return retVec;
}

void
BlockAssembler::
exportSolution(const double& t, const SHP(aVector)& sol)
{
    for (auto as : M_primalAssemblers)
        as.second->exportSolution(t,
            std::static_pointer_cast<BlockVector>(sol->block(as.first)));
}

std::map<unsigned int,std::vector<double>>
BlockAssembler::
getRandomizibleParametersVectors()
{
    std::map<unsigned int,std::vector<double>> retMap;

    for (auto as : M_primalAssemblers)
        retMap[as.first] = as.second->getTreeNode()->M_block->
                getGeometricParametersHandler().getRandomizibleParametersValueAsVector();

    return retMap;
}

void
BlockAssembler::
setExtrapolatedSolution(const SHP(aVector)& exSol)
{
    for (auto as : M_primalAssemblers)
        as.second->setExtrapolatedSolution(
            std::static_pointer_cast<BlockVector>(exSol->block(as.first)));
}

void
BlockAssembler::
postProcess(const double& t, const SHP(aVector)& sol)
{
    for (auto as : M_primalAssemblers)
        as.second->postProcess(t,
            std::static_pointer_cast<BlockVector>(sol->block(as.first)));

    if (this->M_data("coupling/check_stabterm", false))
        checkStabTerm(sol);
}

SHP(aMatrix)
BlockAssembler::
getMass(const double& time, const SHP(aVector)& sol)
{
    SHP(BlockMatrix) mass(new BlockMatrix(M_numberBlocks, M_numberBlocks));

    for (auto as : M_primalAssemblers)
    {
        unsigned int ind = as.first;
        mass->setBlock(ind, ind, as.second->getMass(time, sol->block(ind)));
    }
    mass->close();
    return mass;
}

SHP(aMatrix)
BlockAssembler::
getMassJacobian(const double& time, const SHP(aVector)& sol)
{
    SHP(BlockMatrix) massJacobian(new BlockMatrix(M_numberBlocks, M_numberBlocks));

    for (auto as : M_primalAssemblers)
    {
        unsigned int ind = as.first;
        massJacobian->setBlock(ind,ind,as.second->getMassJacobian(time, sol->block(ind)));
    }

    return massJacobian;
}

SHP(aVector)
BlockAssembler::
getRightHandSide(const double& time, const SHP(aVector)& sol)
{
    SHP(BlockVector) rhs(new BlockVector(M_numberBlocks));

    for (auto as: M_primalAssemblers)
    {
        unsigned int ind = as.first;
        rhs->setBlock(ind,as.second->getRightHandSide(time, sol->block(ind)));
    }

    // add interface contributions
    for (auto as: M_dualAssemblers)
        as->addContributionRhs(time, rhs, std::static_pointer_cast<BlockVector>(sol),
            M_primalAssemblers.size());

    return rhs;
}

SHP(aVector)
BlockAssembler::
convertFunctionRBtoFEM(SHP(aVector) rbFunction,
                       EPETRACOMM comm) const
{
    SHP(BlockVector) retVec(new BlockVector(rbFunction->nRows()));

    for (auto as : M_primalAssemblers)
    {
        unsigned int ind = as.first;
        retVec->setBlock(ind,
            std::static_pointer_cast<aAssemblerRB>(as.second)->convertFunctionRBtoFEM(rbFunction->block(ind)));
    }

    if (rbFunction->nRows() > M_primalAssemblers.size())
    {
        for (auto as : M_dualAssemblers)
        {
            unsigned int indInterface = as->getInterface().M_ID + M_primalAssemblers.size();
            std::static_pointer_cast<BlockVector>(retVec->block(indInterface))->resize(1);
            retVec->block(indInterface)->setBlock(0,
                    DistributedVector::convertDenseVector(
                    std::static_pointer_cast<DenseVector>(rbFunction->block(indInterface)->block(0)),
                    comm));
        }
    }
    return retVec;
}

SHP(aVector)
BlockAssembler::
getNonLinearTerm()
{
    SHP(BlockVector) retVec(new BlockVector(M_primalAssemblers.size()));

    for (auto as : M_primalAssemblers)
        retVec->setBlock(as.first,as.second->getNonLinearTerm());

    return retVec;
}


SHP(aMatrix)
BlockAssembler::
getJacobianRightHandSide(const double& time, const SHP(aVector)& sol)
{
    SHP(BlockMatrix) jac(new BlockMatrix(M_numberBlocks, M_numberBlocks));

    for (auto as: M_primalAssemblers)
    {
        unsigned int ind = as.first;
        jac->setBlock(ind,ind,as.second->getJacobianRightHandSide(time,
                              sol->block(ind)));
    }

    for (auto as: M_dualAssemblers)
        as->addContributionJacobianRhs(time, jac,
            std::static_pointer_cast<BlockVector>(sol),
             M_primalAssemblers.size());

    return jac;
}

std::map<unsigned int, std::string>
BlockAssembler::
getIDMeshTypeMap() const
{
    std::map<unsigned int, std::string> retMap;
    for (auto as: M_primalAssemblers)
    {
        std::string meshname = as.second->getTreeNode()->M_block->getMeshName();
        unsigned int dashpos = meshname.find("/");
        unsigned int formatpos = meshname.find(".mesh");
        std::string actualmeshname = meshname.substr(dashpos + 1, formatpos - dashpos - 1);
        retMap[as.first] = actualmeshname;
    }

    return retMap;
}

void
BlockAssembler::
setup()
{
    typedef std::map<unsigned int, SHP(TreeNode)>         NodesMap;
    typedef aAssembler                                    InnerAssembler;
    typedef std::vector<SHP(TreeNode)>                    NodesVector;

    printlog(GREEN, "[BlockAssembler] initializing block assembler ... \n", this->M_data.getVerbose());

    NodesMap nodesMap = M_tree.getNodesMap();
    // allocate assemblers
    for (NodesMap::iterator it = nodesMap.begin(); it != nodesMap.end(); it++)
    {
        SHP(InnerAssembler) newAssembler;
        newAssembler = AssemblerFactory(this->M_data, it->second);
        newAssembler->setup();
        M_primalAssemblers[it->second->M_ID] = newAssembler;
    }
    // allocate interface assemblers
    unsigned int interfaceID = 0;
    for (NodesMap::iterator it = nodesMap.begin(); it != nodesMap.end(); it++)
    {
        NodesVector children = it->second->M_children;

        unsigned int countOutlet = 0;
        unsigned int myID = it->second->M_ID;

        SHP(InnerAssembler) fatherAssembler = M_primalAssemblers[myID];

        unsigned int countChildren = 0;
        for (NodesVector::iterator itVector = children.begin();
             itVector != children.end(); itVector++)
        {
            if (*itVector)
            {
                unsigned int otherID = (*itVector)->M_ID;
                SHP(InnerAssembler) childAssembler = M_primalAssemblers[otherID];
                Interface newInterface(fatherAssembler, myID,
                                       childAssembler, otherID,
                                       interfaceID);
                newInterface.M_indexOutlet = countChildren;
                SHP(InterfaceAssembler) inAssembler;
                inAssembler.reset(new InterfaceAssembler(this->M_data, newInterface));
                M_dualAssemblers.push_back(inAssembler);
                interfaceID++;
            }
            countChildren++;
        }
    }

    if (!std::strcmp(this->M_data("bc_conditions/inletdirichlet", "weak").c_str(),"weak"))
    {
        SHP(InnerAssembler) inletAssembler = M_primalAssemblers[0];

        // we set the inlet to child such that we are consistent with the normal orientation
        // with respect to flow direction
        Interface newInterface(nullptr, -1, inletAssembler, 0,
                                               interfaceID);

        SHP(InterfaceAssembler) inletInAssembler;
        inletInAssembler.reset(new InletInflowAssembler(this->M_data, newInterface));

        M_dualAssemblers.push_back(inletInAssembler);
        interfaceID++;
    }

    M_numberBlocks = M_primalAssemblers.size() + M_dualAssemblers.size();

    printlog(GREEN, "done\n", this->M_data.getVerbose());
}


}