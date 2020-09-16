// // Reduced Modeling of Arteries (RedMA)
// // Copyright (C) 2019  Luca Pegolotti
// //
// // RedMA is free software: you can redistribute it and/or modify
// // it under the terms of the GNU General Public License as published by
// // the Free Software Foundation, either version 3 of the License, or
// // (at your option) any later version.
// //
// // RedMA is distributed in the hope that it will be useful,
// // but WITHOUT ANY WARRANTY; without even the implied warranty of
// // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// // GNU General Public License for more details.
// //
// // You should have received a copy of the GNU General Public License
// // along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// #ifndef NAVIERSTOKESASSEMBLER_HPP
// #define NAVIERSTOKESASSEMBLER_HPP
//
// #include <redma/assemblers/StokesAssembler.hpp>
// #include <redma/solver/finite_element/SUPGStabilization.hpp>
// #include <redma/solver/assemblers/VMSStabilization.hpp>
// #include <redma/solver/assemblers/HFStabilization.hpp>
//
// namespace RedMA
// {
//
// class NavierStokesAssembler : public StokesAssembler
// {
// public:
//     NavierStokesAssembler(const DataContainer& data, SHP(TreeNode) treeNode);
//
//     virtual void setup() override;
//
//     virtual BlockMatrix getMass(const double& time,
//                                 const BlockVector& sol) override;
//
//     virtual BlockMatrixgetMassJacobian(const double& time,
//                                        const BlockVector& sol) override;
//
//     virtual BlockVector getRightHandSide(const double& time,
//                                          const BlockVector& sol) override;
//
//     virtual BlockMatrix getJacobianRightHandSide(const double& time,
//                                                  const BlockVector& sol) override;
//
//     virtual BlockVector getNonLinearTerm() override {return M_nonLinearTerm;}
//
//     virtual void RBsetup() override;
//
// protected:
//     void addConvectiveMatrixRightHandSide(const BlockVector& sol,
//                                           BlockMatrix& mat);
//
//     void addConvectiveTermJacobianRightHandSide(const BlockVector& sol,
//                                                 const BlockVector& lifting,
//                                                 BlockMatrix& rhs);
//
//     bool                                        M_useStabilization;
//     SHP(NavierStokesStabilization)              M_stabilization;
//     BlockVector<InVectorType>                   M_nonLinearTerm;
//
//     // this is relative to the rb part
//     std::vector<std::vector<BlockVector>>       M_nonLinearTermsDecomposition;
// };
//
// }
//
// #include "NavierStokesAssembler_imp.hpp"
//
// #endif // NAVIERSTOKESASSEMBLER_HPP