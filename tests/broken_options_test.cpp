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

    SECTION("Missing values for option with default doesn't throw")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt3").default_value("hello");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt3") == "hello");
    }

    SECTION("Missing values for option without default throws")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS(parser.parse(argc, argv));
    }

    SECTION("Giving a value with a '=' should work")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt1=12" };

        nitro::broken_options::parser parser;

        parser.option("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }
}

TEST_CASE("Short-cut arguments can get parsed from command line", "[broken_options]")
{
    SECTION("Simple string short named values work")
    {
        int argc = 3;
        const char* argv[] = { "", "-a", "abc" };

        nitro::broken_options::parser parser;

        parser.option("opt2").short_name("a");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt2") == "abc");
    }

    SECTION("Parsing short named integers values works")
    {
        int argc = 3;
        const char* argv[] = { "", "-o", "12" };

        nitro::broken_options::parser parser;

        parser.option("opt1").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("Setting default value for short named works")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt3").short_name("o").default_value("hello");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt3") == "hello");
    }
}

TEST_CASE("Multiple arguments should work", "[broken_options]")
{
    SECTION("Giving an option unexcepted twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "13" };

        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS(parser.parse(argc, argv));
    }

    SECTION("Giving an option mixed with short unexcepted twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "-o", "13" };

        nitro::broken_options::parser parser;

        parser.option("opt3").short_name("o");

        REQUIRE_THROWS(parser.parse(argc, argv));
    }

    SECTION("Giving an identical option twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS(parser.parse(argc, argv));
    }

    SECTION("Giving two different values for the same option should work")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "13" };

        nitro::broken_options::parser parser;

        parser.multi_option("opt3");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt3") == 2);

        REQUIRE(options.get("opt3", 0) == "12");
        REQUIRE(options.get("opt3", 1) == "13");
    }

    SECTION("Giving equal values twice for the same option should work")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::broken_options::parser parser;

        parser.multi_option("opt3");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt3") == 2);

        REQUIRE(options.get("opt3", 0) == "12");
        REQUIRE(options.get("opt3", 1) == "12");
    }
}

TEST_CASE("positional arguments should work", "[broken_options]")
{
    SECTION("-- should trigger everything to be positional")
    {
        int argc = 6;
        const char* argv[] = { "", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 4);

        REQUIRE(options.get(0) == "--opt1");
        REQUIRE(options.get(1) == "12");
        REQUIRE(options.get(2) == "--opt2");
        REQUIRE(options.get(3) == "13");
    }
}
