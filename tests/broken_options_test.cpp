#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/broken_options/parser.hpp>

TEST_CASE("Simple named arguments can get parsed from command line", "[broken_options]")
{
    SECTION("Trying to parse unknown arguments will throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt1", "12", "--opt2", "abc" };

        nitro::broken_options::parser parser;

        REQUIRE_THROWS(parser.parse(argc, argv));
    }

    SECTION("Simple string values work")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt2", "abc" };

        nitro::broken_options::parser parser;

        parser.option("opt2");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt2") == "abc");
    }

    SECTION("Parsing integer values works")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt1", "12" };

        nitro::broken_options::parser parser;

        parser.option("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("Setting default value works")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt3").default_value("hello");

        auto options = parser.parse(1, argv);

        REQUIRE(options.get("opt3") == "hello");
    }

    SECTION("Missing values for option without default throws")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS(parser.parse(1, argv));
    }
}