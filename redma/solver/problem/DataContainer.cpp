#include "DataContainer.hpp"

namespace RedMA
{

DataContainer::
DataContainer()
{

}

void
DataContainer::
setDatafile(const std::string& datafile)
{
    M_datafile.reset(new GetPot(datafile));
}

void
DataContainer::
setInflow(const std::function<double(double)>& inflow)
{
    M_inflow = inflow;
}

void
DataContainer::
setInflowDt(const std::function<double(double)>& inflowDt)
{
    M_inflowDt = inflowDt;
}

std::function<double(double)>
DataContainer::
getDistalPressure(const unsigned int& outletIndex) const
{
    auto it = M_distalPressures.find(outletIndex);

    if (it == M_distalPressures.end())
        throw new Exception("Error in DataContainer: requested Distal pressure not present");

    return it->second;
}

void
DataContainer::
setDistalPressure(const std::function<double(double)>& pressure,
                  const unsigned int& indexOutlet)
{
    M_distalPressures[indexOutlet] = pressure;
}

void
DataContainer::
setVerbose(bool verbose)
{
    M_verbose = verbose;

}

std::string
DataContainer::
operator()(std::string location, std::string defValue) const
{
    return M_datafile->operator()(location.c_str(), defValue.c_str());
}

int
DataContainer::
operator()(std::string location, int defValue) const
{
    return M_datafile->operator()(location.c_str(), defValue);
}

double
DataContainer::
operator()(std::string location, double defValue) const
{
    return M_datafile->operator()(location.c_str(), defValue);
}

void
DataContainer::
setValue(std::string location, std::string value)
{
    M_datafile->set(location.c_str(), value.c_str());
}

void
DataContainer::
setValue(std::string location, int value)
{
    M_datafile->set(location.c_str(), value);
}

void
DataContainer::
setValue(std::string location, double value)
{
    M_datafile->set(location.c_str(), value);
}

}
