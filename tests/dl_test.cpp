#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/dl/dl.hpp>

#ifdef __APPLE__
#define LIB_SUFFIX "dylib"
#elif defined(_WIN32)
#define LIB_SUFFIX "dll"
#else
#define LIB_SUFFIX "so"
#endif

TEST_CASE("library symbols can be loaded and used", "[dl]")
{
    nitro::dl::dl test_library("libnitro_test_lib." LIB_SUFFIX);

    SECTION("nitro_cos is loadable")
    {
        REQUIRE_NOTHROW(auto cos = test_library.load<double(double)>("nitro_cos"));
    }

    SECTION("nitro_cos is callable")
    {
        auto cos = test_library.load<double(double)>("nitro_cos");

        REQUIRE(cos(0) == 1.0);
    }

    SECTION("undefined symbol throws")
    {
        REQUIRE_THROWS(test_library.load<double(double)>("nitro_not_defined"));
    }
}

extern "C" double nitro_binary_cos(double)
{
    return 1.0;
}

TEST_CASE("self binary symbols can be loaded and used", "[dl]")
{
    nitro::dl::dl test_library(nitro::dl::self);

    SECTION("nitro_binary_cos is loadable")
    {
        REQUIRE_NOTHROW(auto cos = test_library.load<double(double)>("nitro_binary_cos"));
    }

    SECTION("nitro_binary_cos is callable")
    {
        auto cos = test_library.load<double(double)>("nitro_binary_cos");

        REQUIRE(cos(0) == 1.0);
    }

    SECTION("undefined symbol throws")
    {
        REQUIRE_THROWS(test_library.load<double(double)>("nitro_binary_not_defined"));
    }
}
