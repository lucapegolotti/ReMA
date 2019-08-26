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

#include <Epetra_ConfigDefs.h>
#ifdef EPETRA_MPI
#include <mpi.h>
#include <Epetra_MpiComm.h>
#else
#include <Epetra_SerialComm.h>
#endif

#include <functional>

#include <GlobalSolver.hpp>
#include <NavierStokesAssembler.hpp>
#include <PseudoFSI.hpp>

#include <lifev/core/filter/GetPot.hpp>

using namespace RedMA;

#define COEFF 10

// double maxLaw(double t)
// {
//     double poly = 0;
//     const double coeffs[8] = {-743.0, 2524.6, -3318.4, 2087.0, -606.25, 49.5, 6.6, 0.0};
//
//     double mon = 1.0;
//     for (int i = 0; i < 8; i++)
//     {
//         poly += coeffs[7-i] * mon;
//         mon *= t;
//     }
//     return poly * COEFF;
// }
//
// double maxLawDt(double t)
// {
//     double poly = 0;
//     const double coeffs[7] = {-743.0, 2524.6, -3318.4, 2087.0, -606.25, 49.5, 6.6};
//
//     double mon = 1.0;
//     for (int i = 0; i < 7; i++)
//     {
//         poly += coeffs[6-i] * mon;
//         mon *= t;
//     }
//     return poly * COEFF;
// }

double maxLaw(double t)
{
    const double T = 3e-3;
    const double omega = 2.0 * M_PI / (T);
    const double Pmax = 13300.0;
    if (t <= T)
    {
        return -0.5 * (1.0 - cos(omega * t) ) * Pmax;
    }
    return 0;
}

double maxLawDt(double t)
{
    const double T = 3e-3;
    const double omega = 2.0 * M_PI / (T);
    const double Pmax = 13300.0;
    if (t <= T)
    {
        return -0.5 * omega * sin(omega * t) * Pmax;
    }
    return 0.0;
}

int main(int argc, char **argv)
{
    #ifdef HAVE_MPI
    MPI_Init (nullptr, nullptr);
    std::shared_ptr<Epetra_Comm> comm (new Epetra_MpiComm(MPI_COMM_WORLD));
    #else
    std::shared_ptr<Epetra_Comm> comm(new Epetra_SerialComm());
    #endif

    GetPot datafile("data");
    bool verbose = comm->MyPID() == 0;
    GlobalSolver<PseudoFSI> gs(datafile, comm, verbose);
    gs.setExportNorms("norms_nonconforming.txt");
    gs.setLawInflow(std::function<double(double)>(maxLaw));
    gs.setLawDtInflow(std::function<double(double)>(maxLawDt));
    gs.solve();

    return 0;
}