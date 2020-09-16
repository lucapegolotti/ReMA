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

#ifndef INVERSEOPERATOR_HPP
#define INVERSEOPERATOR_HPP

#include <redma/RedMA.hpp>
#include <redma/problem/DataContainer.hpp>
#include <redma/solver/system_solver/LinearOperator.hpp>
#include <redma/solver/system_solver/PreconditionerOperator.hpp>

#include <lifev/core/algorithm/SolverAztecOO.hpp>
#include <lifev/core/linear_algebra/InvertibleOperator.hpp>

#include <Teuchos_ParameterList.hpp>
#include <Teuchos_XMLParameterListHelpers.hpp>

namespace RedMA
{

class InverseOperator
{
    typedef LifeV::Operators::InvertibleOperator        InvertibleOperatorType;

public:
    InverseOperator(const DataContainer& data);

    void setOperator(SHP(LinearOperator) oper);

    void setPreconditioner(SHP(PreconditionerOperator) prec);

    void setSolverOptions();

    int invert(const SHP(aVector)& rhs, SHP(aVector)& sol);

private:
    DataContainer                                   M_data;
    SHP(InvertibleOperatorType)                     M_invOper;
    SHP(Teuchos::ParameterList)                     M_pListLinSolver;
    Teuchos::RCP<Teuchos::ParameterList>            M_solversOptions;
    SHP(BlockMaps)                                  M_maps;
};

}

#endif // INVERSEOPERATOR_HPP