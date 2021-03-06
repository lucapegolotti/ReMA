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

#ifndef MDEIMCHECKER_HPP
#define MDEIMCHECKER_HPP

#include <redma/RedMA.hpp>
#include <redma/problem/DataContainer.hpp>
#include <redma/problem/GlobalProblem.hpp>
#include <redma/geometry/GeometryPrinter.hpp>

#include <redma/reduced_basis/BlockMDEIM.hpp>

#include <redma/RedMA.hpp>

namespace RedMA
{

class MDEIMChecker
{
public:

    MDEIMChecker(const DataContainer& data, EPETRACOMM comm);

    void checkOnlineMDEIM();

private:

    void loadMDEIMs();

    DataContainer                                     M_data;
    EPETRACOMM                                        M_comm;

    std::map<std::string, std::vector<BlockMDEIM>>    M_blockMDEIMsMap;
};

}  // namespace RedMA

#endif  // MDEIMChecker
