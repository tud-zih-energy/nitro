#include <cmath>

extern "C"
{
    double nitro_cos(double x)
    {
        return std::cos(x);
    }
}
