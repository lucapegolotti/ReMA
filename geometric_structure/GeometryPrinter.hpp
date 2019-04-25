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

#ifndef GEOMETRYPRINTER_HPP
#define GEOMETRYPRINTER_HPP

#include <stdio.h>
#include <string>
#include <queue>

#include <tinyxml2.h>

#include <TreeStructure.hpp>

namespace RedMA
{

class GeometryPrinter
{
public:
    GeometryPrinter();

    void saveToFile(TreeStructure& tree, std::string name,
                    std::shared_ptr<Epetra_Comm> comm);
};

}  // namespace RedMA

#endif  // GEOMETRYPRINTER_HPP