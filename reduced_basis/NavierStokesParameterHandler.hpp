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

#ifndef NAVIERSTOKESPARAMETERHANDLER_HPP
#define NAVIERSTOKESPARAMETERHANDLER_HPP

#include <rb/reduced_basis/rbLifeV.hpp>
#include <rb/reduced_basis/rbSolver/ParameterHandler.hpp>

namespace RedMA
{


class NavierStokesParameterHandler : public rbLifeV::ParameterHandler
{

public:

    NavierStokesParameterHandler(const param_Type& _muMin,
                                 const param_Type& _muMax,
                                 bool _verbose = true);

    ~NavierStokesParameterHandler(){};

    inline void applyTheta()
    {
    }

    void setTheta();

protected:

};

}

#endif // namespace RedMA
