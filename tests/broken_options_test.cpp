#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/broken_options/parser.hpp>

TEST_CASE("Using argc, argv from main does compile")
{
    int argc;
    char** argv = nullptr;

    nitro::broken_options::parser parser;

    decltype(parser.parse(argc, argv))* foo;
    (void)foo;
}

TEST_CASE("Usage descriptions work")
{
    SECTION("Description for options work")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        parser.toggle("tog", "some toggle").short_name("t");
        parser.toggle("togg", "some other toggle").short_name("u");

        parser.option("opt", "some opt").short_name("o");
        parser.option("opt_with_d", "some opt with a default")
            .short_name("d")
            .default_value("default value");

        parser.option("opt_nos", "some opt without a short, but a default")
            .default_value("default value");
        parser.option("opt_nosd", "some opt without short and default");

        parser.option("opt_long",
                      "an option with an very very very very very very very very very "
                      "very very very very very very very very very very very very very "
                      "very very very very very very very very long description");

        parser
            .option("some_long_named_option",
                    "an option with an very very very very very very very very very "
                    "very very very very very very very very very very very very very "
                    "very very very very very very very very long description")
            .short_name("x")
            .default_value("some very long default parameter for this fucking thing");

        parser.multi_option("mopt", "some multi opt").short_name("m");

        parser.usage(s);

        REQUIRE(
            s.str() ==
            R"EXPECTED(usage: app_name [-tu] --opt --opt_long [--opt_nos] --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt

about

arguments:
  -m [ --mopt ] arg                       some multi opt
  -o [ --opt ] arg                        some opt
  --opt_long arg                          an option with an very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very long
                                          description
  --opt_nos [=default value]              some opt without a short, but a
                                          default
  --opt_nosd arg                          some opt without short and default
  -d [ --opt_with_d ] [=default value]    some opt with a default
  -x [ --some_long_named_option ] [=some very long default parameter for this fucking thing]
                                          an option with an very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very long
                                          description
  -t [ --tog ]                            some toggle
  -u [ --togg ]                           some other toggle
)EXPECTED");
    }
}

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

    SECTION("Defining an option also as a multi_option should not work")
    {
        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS(parser.multi_option("opt3"));
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

    SECTION("Getting positional arguments in reverse should work")
    {
        int argc = 6;
        const char* argv[] = { "", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 4);

        REQUIRE(options.get(-4) == "--opt1");
        REQUIRE(options.get(-3) == "12");
        REQUIRE(options.get(-2) == "--opt2");
        REQUIRE(options.get(-1) == "13");
    }

    SECTION("Given ignore_unknown(true), all unknown options should be interpreted as positionals")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;

        parser.ignore_unknown();

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 6);

        REQUIRE(options.get(0) == "--unknown");
        REQUIRE(options.get(1) == "value");
        REQUIRE(options.get(2) == "--opt1");
        REQUIRE(options.get(3) == "12");
        REQUIRE(options.get(4) == "--opt2");
        REQUIRE(options.get(5) == "13");
    }
}

TEST_CASE("Giving values with an '=' should work ")
{

    SECTION("for named options")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt1=12" };

        nitro::broken_options::parser parser;

        parser.option("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("for short options")
    {
        int argc = 2;
        const char* argv[] = { "", "-o=12" };

        nitro::broken_options::parser parser;

        parser.option("opt1").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("for toggles")
    {
        int argc = 2;
        const char* argv[] = { "", "-o=12" };

        nitro::broken_options::parser parser;

        parser.toggle("opt1").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt1"));
    }
}

TEST_CASE("Defining an option twice should return the same object twice")
{
    SECTION("for options")
    {
        nitro::broken_options::parser parser;

        auto obj1 = &parser.option("opt1");
        auto obj2 = &parser.option("opt1");

        REQUIRE(obj1 == obj2);
    }

    SECTION("for multi_options")
    {
        nitro::broken_options::parser parser;

        auto obj1 = &parser.multi_option("opt1");
        auto obj2 = &parser.multi_option("opt1");

        REQUIRE(obj1 == obj2);
    }

    SECTION("for toggles")
    {
        nitro::broken_options::parser parser;

        auto obj1 = &parser.toggle("opt1");
        auto obj2 = &parser.toggle("opt1");

        REQUIRE(obj1 == obj2);
    }
}

TEST_CASE("Trying to redefine short_name")
{
    SECTION("for options should throw")
    {
        nitro::broken_options::parser parser;

        parser.option("opt1").short_name("o");

        REQUIRE_THROWS(parser.option("opt1").short_name("p"));
    }

    SECTION("for multi_options should throw")
    {
        nitro::broken_options::parser parser;

        parser.multi_option("opt1").short_name("o");

        REQUIRE_THROWS(parser.multi_option("opt1").short_name("p"));
    }

    SECTION("for toggle should throw")
    {
        nitro::broken_options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_THROWS(parser.toggle("opt1").short_name("p"));
    }

    SECTION("for options with same short name should NOT throw")
    {
        nitro::broken_options::parser parser;

        parser.option("opt1").short_name("o");

        REQUIRE_NOTHROW(parser.option("opt1").short_name("o"));
    }

    SECTION("for multi_options with same short name should NOT throw")
    {
        nitro::broken_options::parser parser;

        parser.multi_option("opt1").short_name("o");

        REQUIRE_NOTHROW(parser.multi_option("opt1").short_name("o"));
    }

    SECTION("for toggle with same short name should NOT throw")
    {
        nitro::broken_options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_NOTHROW(parser.toggle("opt1").short_name("o"));
    }
}

TEST_CASE("Toggles should work", "[broken_options]")
{
    SECTION("for named options")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt1" };

        nitro::broken_options::parser parser;

        parser.toggle("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt1"));
    }

    SECTION("for short options")
    {
        int argc = 2;
        const char* argv[] = { "", "-o" };

        nitro::broken_options::parser parser;

        parser.toggle("opt2").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt2"));
    }

    SECTION("given multiple times")
    {
        int argc = 4;
        const char* argv[] = { "", "-o", "-o", "-o" };

        nitro::broken_options::parser parser;

        parser.toggle("opt2").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt2") == 3);
    }

    SECTION("when not given")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.toggle("opt2").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("opt2"));
    }

    SECTION("for multiple toggles in one arg")
    {
        int argc = 2;
        const char* argv[] = { "", "-ab" };

        nitro::broken_options::parser parser;

        parser.toggle("opt_a").short_name("a");
        parser.toggle("opt_b").short_name("b");
        parser.toggle("opt_c").short_name("c");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt_a"));
        REQUIRE(options.given("opt_b"));
        REQUIRE(!options.given("opt_c"));
    }
}
