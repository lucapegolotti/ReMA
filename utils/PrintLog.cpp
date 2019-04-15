#include "PrintLog.hpp"

namespace ReMA
{

void printlog(Color outColor, std::string text, bool verbose)
{
    if(verbose)
    {
        std::cout << "\033[;" << outColor+30 << "m";
        std::cout << text;
        std::cout << "\033[0m";
    }
}

template < typename T >
std::string to_string( const T& n )
{
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}

void printlog(Color outColor, int num, bool verbose)
{
    printlog(outColor, to_string(num), verbose);
}

}  // PrintLog
