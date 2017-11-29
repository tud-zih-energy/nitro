#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/format/format.hpp>

TEST_CASE("Simple format strings", "[format]")
{
    SECTION("can be printed to stdout")
    {
        std::cout << nitro::format("Hello World") << std::endl;
    }

    SECTION("can implicitly convert to std::string")
    {
        const char* ref_str = "Hello World\n";

        auto nf = nitro::format(ref_str);

        std::string str = nf;

        REQUIRE(str == nf.str());
        REQUIRE(str == ref_str);
    }
}
