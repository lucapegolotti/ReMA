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

#ifndef GLOBALASSEMBLER_HPP
#define GLOBALASSEMBLER_HPP

#include <fstream>

#include <AbstractFunctor.hpp>

#include <AssemblerFactory.hpp>

#include <TreeStructure.hpp>
#include <AbstractAssembler.hpp>
#include <BlockMatrix.hpp>

#include <lifev/core/array/MapVector.hpp>
#include <lifev/core/array/MatrixEpetraStructured.hpp>
#include <lifev/core/array/VectorEpetraStructured.hpp>
#include <lifev/core/array/MatrixEpetraStructuredUtility.hpp>

namespace RedMA
{

class GlobalAssembler : public AbstractAssembler
{
    typedef std::shared_ptr<TreeNode>                       TreeNodePtr;
    typedef LifeV::MapEpetra                                MapEpetra;
	typedef std::shared_ptr<MapEpetra>                      MapEpetraPtr;
    typedef LifeV::VectorEpetra                             Vector;
    typedef std::shared_ptr<Vector>                         VectorPtr;
    typedef LifeV::MatrixEpetra<double>                     Matrix;
    typedef std::shared_ptr<Matrix>                         MatrixPtr;
    typedef std::shared_ptr<Epetra_Comm>                    commPtr_Type;
    typedef std::shared_ptr<AbstractAssembler>              AbstractAssemblerPtr;

    typedef std::function<double(double const&,
                                 double const&,
                                 double const&,
                                 double const&,
                                 unsigned int const& )>     Function;

public:
    GlobalAssembler(const GetPot& datafile, commPtr_Type comm,
                    bool verbose = false);

    void setup(TreeStructure& tree);

    void buildPrimalStructures(TreeStructure& tree);

    void buildDualStructures(TreeStructure& tree);

    MapEpetraPtr getGlobalMap() const;

    BlockMatrix getGlobalMass();

    BlockMatrix getGlobalMassJac();

    BlockMatrix getGlobalMassJacVelocity();

    BlockMatrix getJacobianF(bool addCoupling,
                                   double* diagonalCoefficient = nullptr);

    BlockMatrix getJacobianFprec(bool addCoupling,
                                       double* diagonalCoefficient = nullptr);

    VectorPtr computeF();

    VectorPtr computeFder();

    // the diagonal coefficient is for the boundary conditions (if null, no
    // bcs are applied)
    BlockMatrix assembleGlobalMass(bool addCoupling,
                                         double* diagonalCoefficient = nullptr);

    void setTimeAndPrevSolution(const double& time, VectorPtr solution,
                                bool doAssembly = true);

    void applyBCsRhsRosenbrock(VectorPtr rhs, VectorPtr utilde,
                               const double& time, const double& dt,
                               const double& alphai, const double& gammai);

    template<typename FunctionType>
    void applyBCsVector(VectorPtr rhs, const double& coeff, const double& time,
                        FunctionType bcFunction);

    void setLawInflow(std::function<double(double)> maxLaw);

    void setLawDtInflow(std::function<double(double)> maxLawDt);

    void setExactSolution(AbstractFunctor* exactSolution);

    void exportSolutions(const double& time, VectorPtr solution);

    void appendNormsToFile(const double& time, VectorPtr solution,
                           std::ofstream& outFile);

    void appendErrorsToFile(const double& time, VectorPtr solution,
                            std::ofstream& outFile);

    void setTimeIntegrationOrder(unsigned int order);

    void setTimestep(double dt);

    void checkResidual(VectorPtr solution, VectorPtr prevSolution, double dt);

    void postProcess();

    VectorPtr getInitialCondition();

    void printMeshSize(std::string filename);

    void setForcingFunction(Function forcingFunction,
                            Function forcingFunctionDt);

private:
    template<typename FunctionType>
    void fillGlobalMatrix(BlockMatrix& matrixToFill,
                          bool addCoupling,
                          FunctionType getMatrixMethod,
                          double* diagonalCoefficient);

    template<typename FunctionType>
    void fillGlobalVector(VectorPtr& vectorToFill,
                          FunctionType getVectorMethod);

    std::vector<std::pair<unsigned int, AbstractAssemblerPtr> > M_assemblersVector;
    std::map<unsigned int, AbstractAssemblerPtr>                M_assemblersMap;
    GetPot                                                      M_datafile;
    commPtr_Type                                                M_comm;
    bool                                                        M_verbose;
    MapEpetraPtr                                                M_globalMap;
    BlockMatrix                                           M_massMatrix;
    BlockMatrix                                           M_updatedMassMatrix;
    std::vector<unsigned int>                                   M_dimensionsVector;
    std::vector<std::pair<unsigned int, unsigned int> >         M_interfaces;
    std::vector<unsigned int>                                   M_offsets;
    unsigned int                                                M_nPrimalBlocks;
    std::vector<MapEpetraPtr>                                   M_maps;
    bool                                                        M_updateMassMatrix;
};

template<typename FunctionType>
void
GlobalAssembler::
fillGlobalMatrix(BlockMatrix& matrixToFill, bool addCoupling,
                 FunctionType getMatrixMethod, double* diagonalCoefficient)
{
    typedef std::vector<std::pair<unsigned int, AbstractAssemblerPtr> >
                AssemblersVector;

    unsigned int totalNumberBlocks = M_nPrimalBlocks + M_interfaces.size();
    matrixToFill.resize(totalNumberBlocks, totalNumberBlocks);
    matrixToFill.setMaps(M_maps, M_maps);
    // we start with the primal blocks
    unsigned int countBlocks = 0;
    for (typename AssemblersVector::iterator it = M_assemblersVector.begin();
         it != M_assemblersVector.end(); it++)
    {
        unsigned int blockIndex = it->first;
        unsigned int numberBlocks = it->second->numberOfBlocks();
        for (int i = 0; i < numberBlocks; i++)
        {
            for (int j = 0; j < numberBlocks; j++)
            {
                AbstractAssembler& curAssembler = *it->second;
                MatrixPtr localMatrix = (curAssembler.*getMatrixMethod)(i, j);
                if (diagonalCoefficient)
                    curAssembler.applyBCsMatrix(localMatrix, *diagonalCoefficient,
                                                i, j);
                // Attention: this does not work if number of blocks is not constant
                // over all the domains
                matrixToFill.copyBlock(countBlocks + i,
                                       countBlocks + j,
                                       localMatrix);
            }
        }
        countBlocks += numberBlocks;
    }

    // if required add the coupling blocks
    if (addCoupling)
    {
        typedef std::vector<std::pair<unsigned int, unsigned int> >
                InterfacesVector;

        unsigned int offset = M_nPrimalBlocks;

        for (InterfacesVector::iterator it = M_interfaces.begin();
             it != M_interfaces.end(); it++)
        {
            unsigned int indices[2] = {it->first, it->second};

            for (int i = 0; i < 2; i++)
            {
                AbstractAssembler& curAssembler = *M_assemblersMap[indices[i]];
                MatrixPtr Qt = curAssembler.getQT(indices[(i+1) % 2]);
                unsigned int blockCoupling = curAssembler.getIndexCoupling();

                unsigned int blockIndex = blockCoupling +
                                indices[i] * curAssembler.numberOfBlocks();

                curAssembler.applyBCsMatrix(Qt, 0.0,
                                            blockCoupling, blockCoupling);
                matrixToFill.copyBlock(blockIndex, offset, Qt);

                MatrixPtr Q = curAssembler.getQ(indices[(i+1) % 2]);
                matrixToFill.copyBlock(offset, blockIndex, Q);
            }
            offset++;
        }
    }
}

template<typename FunctionType>
void
GlobalAssembler<AbstractAssembler>::
fillGlobalVector(VectorPtr& vectorToFill, FunctionType getVectorMethod)
{
    using namespace LifeV::MatrixEpetraStructuredUtility;

    typedef std::vector<std::pair<unsigned int, AbstractAssemblerPtr> >
                AssemblersVector;

    typedef std::vector<MapEpetraPtr>                    MapVector;

    vectorToFill.reset(new Vector(*M_globalMap));
    vectorToFill->zero();

    unsigned int offset = 0;
    for (typename AssemblersVector::iterator it = M_assemblersVector.begin();
         it != M_assemblersVector.end(); it++)
    {
        std::vector<VectorPtr> localSolutions;
        MapVector maps = it->second->getPrimalMapVector();

        AbstractAssembler& curAssembler = *it->second;
        std::vector<VectorPtr> localVectors;
        localVectors = (curAssembler.*getVectorMethod)();

        unsigned int index = 0;

        for (MapVector::iterator itmap = maps.begin();
             itmap != maps.end(); itmap++)
        {
            LifeV::MapEpetra& curLocalMap = **itmap;
            // we fill only if the vector corresponding to map i exists!
            if (localVectors[index])
            {
                vectorToFill->subset(*localVectors[index], curLocalMap, 0,
                                     offset);
            }
            offset += curLocalMap.mapSize();
            index++;
        }

        // deal with the dual part. This is trickier because more assemblers
        // contribute to the same subsets of vectorfill
        index = 0;
        maps = it->second->getDualMapVector();
        std::vector<unsigned int> indices = it->second->getInterfacesIndices();
        for (MapVector::iterator itmap = maps.begin();
             itmap != maps.end(); itmap++)
        {
            LifeV::MapEpetra& curLocalMap = **itmap;
            VectorPtr aux(new Vector(curLocalMap));
            aux->subset(*vectorToFill, curLocalMap,
                        M_offsets[M_nPrimalBlocks + indices[index]], 0);
            *aux += 0;
            localVectors[index + it->second->numberOfBlocks()]->zero();
            vectorToFill->subset(*aux, curLocalMap, 0,
                                  M_offsets[M_nPrimalBlocks + indices[index]]);
            index++;
        }
    }
}

template<typename FunctionType>
void
GlobalAssembler<AbstractAssembler>::
applyBCsVector(VectorPtr rhs, const double& coeff, const double& time,
               FunctionType bcFunction)
{
    typedef std::pair<unsigned int, AbstractAssemblerPtr>    Pair;
    typedef std::vector<Pair>                            AssemblersVector;
    typedef std::shared_ptr<LifeV::MapEpetra>            MapEpetraPtr;
    typedef std::vector<MapEpetraPtr>                    MapVector;

    unsigned int offset = 0;
    for (typename AssemblersVector::iterator it = M_assemblersVector.begin();
         it != M_assemblersVector.end(); it++)
    {
        std::vector<VectorPtr> rhss;
        std::vector<VectorPtr> utildes;
        MapVector maps = it->second->getPrimalMapVector();
        unsigned int suboffset = 0;
        for (MapVector::iterator itmap = maps.begin();
             itmap != maps.end(); itmap++)
        {
            LifeV::MapEpetra& curLocalMap = **itmap;
            VectorPtr subRhs;
            subRhs.reset(new Vector(curLocalMap));
            subRhs->zero();
            subRhs->subset(*rhs, curLocalMap, offset + suboffset, 0);
            rhss.push_back(subRhs);
            suboffset += curLocalMap.mapSize();
        }
        // apply bcs
        AbstractAssembler& curAssembler = *it->second;
        (curAssembler.*bcFunction)(rhss, coeff, time);

        suboffset = 0;
        unsigned int count = 0;
        // copy back to global vectors
        for (MapVector::iterator itmap = maps.begin();
             itmap != maps.end(); itmap++)
        {
            LifeV::MapEpetra& curLocalMap = **itmap;
            rhs->subset(*rhss[count], curLocalMap, 0, offset + suboffset);
            suboffset += curLocalMap.mapSize();
            count++;
        }
        offset += suboffset;
    }
}


}  // namespace RedMA

#endif  // GLOBALASSEMBLER_HPP
