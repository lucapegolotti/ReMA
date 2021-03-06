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

#include <redma/RedMA.hpp>
#include <redma/assemblers/abstract/aAssembler.hpp>
#include <redma/assemblers/DefaultAssemblersLibrary.hpp>
#include <redma/problem/DataContainer.hpp>

using namespace RedMA;
// using namespace boost::filesystem;

int main(int argc, char **argv)
{
    // #ifdef HAVE_MPI
    // MPI_Init (nullptr, nullptr);
    // EPETRACOMM comm (new Epetra_MpiComm(MPI_COMM_WORLD));
    // #else
    // EPETRACOMM comm(new Epetra_SerialComm());
    // #endif
    //
    // DataContainer data;
    // data.setDatafile("datafiles/data");
    // data.setVerbose(comm->MyPID() == 0);
    // data.finalize();
    // std::string indir = "basis";
    // std::string outdir = "basisFunctions";
    // std::string format = "basis";
    // unsigned int nmodes = 4;
    // if (argc == 1)
    //     indir = "basis";
    // if (argc >= 2)
    //     indir = argv[1];
    // if (argc >= 3)
    //     outdir = argv[2];
    // if (argc >= 4)
    //     format = argv[3];
    // if (argc >= 5)
    //     nmodes = atoi(argv[4]);
    //
    // data.setValueString("exporter/outdir", outdir + "/");
    //
    //
    // std::set<std::string> meshTypes;
    // meshTypes.insert("tube_1x1_h0.08");
    // meshTypes.insert("tube_1x2_h0.08");
    // meshTypes.insert("tube_1x3_h0.08");
    // meshTypes.insert("bif_sym_alpha50_h0.10");
    // DefaultAssemblersLibrary library(data, meshTypes, comm);
    //
    // for (auto m : meshTypes)
    // {
    //     auto defAssembler = library.getDefaultAssembler(m);
    //     defAssembler->setup();
    //
    //     std::vector<std::vector<shp<VECTOREPETRA>>> basisFunctions;
    //
    //     for (int fieldIndex = 0; fieldIndex < 2; fieldIndex++)
    //     {
    //         std::vector<shp<VECTOREPETRA>> functions;
    //         std::string filename = indir + "/" + m + "/field" + std::to_string(fieldIndex) + "_piola." + format;
    //         if (!fs::esists(filename))
    //             filename = indir + "/" + m + "/field" + std::to_string(fieldIndex) + "." + format;
    //         std::ifstream infile(filename);
    //         std::string line;
    //         auto fespace = defAssembler->getFEspace(fieldIndex);
    //         unsigned int count = 0;
    //         while(std::getline(infile,line) && count < nmodes)
    //         {
    //             shp<VECTOREPETRA> newVector(new VECTOREPETRA(fespace->map()));
    //
    //             std::stringstream linestream(line);
    //             std::string value;
    //             unsigned int i = 0;
    //             while(getline(linestream,value,','))
    //             {
    //                 newVector->operator[](i) = std::atof(value.c_str());
    //                 i++;
    //             }
    //
    //             functions.push_back(newVector);
    //             count++;
    //         }
    //         basisFunctions.push_back(functions);
    //     }
    //
    //
    //     auto zeroVec = defAssembler->getZeroVector();
    //     for (int i = 0; i < nmodes; i++)
    //     {
    //         zeroVec.block(0).data() = basisFunctions[0][i];
    //         zeroVec.block(1).data() = basisFunctions[1][i];
    //
    //         defAssembler->exportSolution(i, zeroVec);
    //     }
    //
    // }

    return 0;
}
