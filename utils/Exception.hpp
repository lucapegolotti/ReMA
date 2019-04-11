// Reduced Modeling of Arteris
// Copyright (C) 2019  Luca Pegolotti
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <exception>
#include <string>

namespace ReMA
{

class Exception: public std::exception
{
public:
    explicit Exception(const char* message) :
      M_msg(message)
    {
    }

    explicit Exception(const std::string& message) :
      M_msg(message)
    {
    }

    virtual ~Exception() throw ()
    {
    }

    virtual const char* what() const throw ()
    {
       return M_msg.c_str();
    }

protected:
    std::string M_msg;
};

}  // namespace ReMA
