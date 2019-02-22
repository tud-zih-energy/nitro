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

    SECTION("can format with one string argument")
    {
        const char* ref_str = "Hello World\n";

        std::string out = nitro::format("{}\n") % "Hello World";

        REQUIRE(out == ref_str);
    }

    SECTION("can format with multiple string arguments")
    {
        const char* ref_str = "Hello World\n";

        std::string out = nitro::format("{} {}\n") % "Hello" % "World";

        REQUIRE(out == ref_str);
    }

    SECTION("can format with one int arguments")
    {
        const char* ref_str = "Hello 8\n";

        std::string out = nitro::format("Hello {}\n") % 8;

        REQUIRE(out == ref_str);
    }

    SECTION("can format with multiple int arguments")
    {
        const char* ref_str = "Hello 8 42\n";

        std::string out = nitro::format("Hello {} {}\n") % 8 % 42;

        REQUIRE(out == ref_str);
    }

    SECTION("can format a slightly more complex example")
    {
        const char* ref_str = "Hello 8 world, the answer is 42!\n";

        std::string out = nitro::format("Hello {} {}, the answer is {}!\n") % 8 % "world" % 42;

        REQUIRE(out == ref_str);
    }
}

TEST_CASE("Holding it wrong gives exceptions", "[format]")
{
    SECTION("Formatting with more arguments than placeholders throws")
    {
        auto fmt = nitro::format("Hello {}\n") % 8 % 42;

        REQUIRE_THROWS(fmt.str());
    }

    SECTION("Formatting with less arguments than placeholders throws")
    {
        auto fmt = nitro::format("Hello {} {}\n") % 8;

        REQUIRE_THROWS(fmt.str());
    }
}
