#include <nitro/dl/dl.hpp>

#include <cassert>
#include <iostream>

#ifdef __APPLE__
#define LIB_SUFFIX "dylib"
#elif defined(_WIN32)
#define LIB_SUFFIX "dll"
#else
#define LIB_SUFFIX "so"
#endif

int main()
{

    nitro::dl::dl test_library("libnitro_test_lib." LIB_SUFFIX);

    auto cos = test_library.load<double(double)>("nitro_cos");

    assert(cos(0) == 1.0);

    return 0;
}
