#include "ZernikeBasisFunction.hpp"

namespace RedMA
{

ZernikeBasisFunction::
ZernikeBasisFunction(const GeometricFace& face,
                     unsigned int nMax) :
  BasisFunctionFunctor(face)
{
    M_nMax = nMax;
    M_R = face.M_radius;
    fillFactorials(nMax);

    for (int n = 0; n <= nMax; n++)
    {
        for (int m = -n; m <= n; m++)
        {
            if (!((n - std::abs(m)) % 2))
            {
                M_ms.push_back(m);
                M_ns.push_back(n);

                std::vector<int> curListCoefs;
                int mult = -1;
                int num;
                int den;
                for (int k = 0; k <= (n-std::abs(m))/2; k++)
                {
                    mult = mult * (-1);
                    num = mult * M_factorials[n - k];
                    den = M_factorials[k] * M_factorials[(n+std::abs(m))/2 - k] *
                          M_factorials[(n-std::abs(m))/2-k];
                    curListCoefs.push_back(num / den);
                }
                M_polyCoefs.push_back(curListCoefs);
            }
        }
    }
    M_nBasisFunctions = M_polyCoefs.size();
    M_type = "zernike";
}

void
ZernikeBasisFunction::
setIndex(const unsigned int& index)
{
    BasisFunctionFunctor::setIndex(index);
    if (M_ms[index] < 0)
        M_curFunction = (double(*)(double))&std::sin;
    else
        M_curFunction = (double(*)(double))&std::cos;
    computeOrthonormalizationCoefficient();
}

void
ZernikeBasisFunction::
computeOrthonormalizationCoefficient()
{
    int m = M_ms[M_index];
    int n = M_ns[M_index];

    std::vector<int> curCoefList = M_polyCoefs[M_index];
    std::vector<int> exponents;
    unsigned int nCoefs = curCoefList.size();

    for (int k = 0; k < nCoefs; k++)
    {
        exponents.push_back(n - 2*k);
    }

    double coeff = 0;
    for (int i = 0; i < nCoefs; i++)
    {
        for (int j = 0; j < nCoefs; j++)
        {
            unsigned c = exponents[i] + exponents[j] + 2;
            coeff += (1.0 * curCoefList[i] * curCoefList[j] * std::pow(M_R,2)) / c;
        }
    }

    if (m < 0)
        coeff *= (M_PI - std::sin(4. * M_PI * std::abs(m)) / (4. * std::abs(m)));
    else if (m == 0)
        coeff *= 2. * M_PI;
    else
        coeff *= (std::sin(4. * M_PI * std::abs(m)) / (4. * std::abs(m)) + M_PI);

    M_orthoCoefficient = std::sqrt(1./coeff);
}

void
ZernikeBasisFunction::
fillFactorials(unsigned int nMax)
{
    unsigned int curN = 1;
    M_factorials.push_back(curN);
    for (int i = 1; i <= nMax; i++)
    {
        curN *= i;
        M_factorials.push_back(curN);
    }
}

ZernikeBasisFunction::return_Type
ZernikeBasisFunction::
operator()(const Vector3D& pos)
{
    double theta;
    double r;

    getThetaAndRadius(pos, theta, r);

    double returnVal = 0;

    unsigned int n = M_ns[M_index];
    unsigned int m = std::abs(M_ms[M_index]);
    std::vector<int>& coefList = M_polyCoefs[M_index];

    for (int k = 0; k <= (n-m)/2; k++)
    {
        returnVal += coefList[k] * std::pow(r/M_R,n-2*k);
    }

    returnVal *= M_curFunction(m * theta) * M_orthoCoefficient;
    return returnVal;
}

}  // namespace RedMA
