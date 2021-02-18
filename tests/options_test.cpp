#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/options/parser.hpp>
#include <nitro/options/user_input.hpp>

TEST_CASE("user_input class works as intended", "[options]")
{
    SECTION("double dash gets parsed correctly")
    {
        nitro::options::user_input arg("--");

        REQUIRE(arg.is_double_dash());
    }

    SECTION("double dash does not get confised with a named argument")
    {
        nitro::options::user_input arg("--not-a-double-dash");

        REQUIRE(!arg.is_double_dash());
    }

    SECTION("double dash does not get confised with a short")
    {
        nitro::options::user_input arg("-v");

        REQUIRE(!arg.is_double_dash());
    }

    SECTION("Values get parsed as such")
    {
        nitro::options::user_input arg("some_value");
        REQUIRE(arg.is_value());
        REQUIRE(arg.value() == "some_value");
        REQUIRE_THROWS_AS(arg.name(), nitro::options::parser_error);
    }

    SECTION("Values get not confused with named arguments")
    {
        nitro::options::user_input arg("--some-arg");
        REQUIRE(!arg.is_value());
    }

    SECTION("Values get not confused with short list")
    {
        nitro::options::user_input arg("-abcd");
        REQUIRE(!arg.is_value());
    }

    SECTION("short arguments get recognized properly")
    {
        REQUIRE(nitro::options::user_input("-a").is_short());
        REQUIRE(nitro::options::user_input("-ab").is_short());
        REQUIRE(!nitro::options::user_input("--ab").is_short());
        REQUIRE(!nitro::options::user_input("ab").is_short());
        REQUIRE(!nitro::options::user_input("--").is_short());
    }

    SECTION("named arguments get recognized properly")
    {
        REQUIRE(!nitro::options::user_input("-a").is_named());
        REQUIRE(!nitro::options::user_input("-ab").is_named());
        REQUIRE(nitro::options::user_input("--ab").is_named());
        REQUIRE(!nitro::options::user_input("ab").is_named());
        REQUIRE(!nitro::options::user_input("--").is_named());
    }

    SECTION("arguments get recognized properly")
    {
        REQUIRE(nitro::options::user_input("-a").is_argument());
        REQUIRE(nitro::options::user_input("-ab").is_argument());
        REQUIRE(nitro::options::user_input("--ab").is_argument());
        REQUIRE(!nitro::options::user_input("ab").is_argument());
        REQUIRE(!nitro::options::user_input("--").is_argument());
    }

    SECTION("arguments get properly parsed")
    {
        nitro::options::user_input short_arg("-a");
        REQUIRE(short_arg.is_argument());
        REQUIRE(short_arg.is_short());
        REQUIRE(!short_arg.is_named());
        REQUIRE(!short_arg.has_value());
        REQUIRE(short_arg.name() == "-a");
        REQUIRE_THROWS_AS(short_arg.as_named(), nitro::options::parser_error);
        REQUIRE(short_arg.as_short_list().size() == 1);
        REQUIRE(short_arg.as_short_list().count("a") == 1);

        nitro::options::user_input named_arg("--ab");
        REQUIRE(named_arg.is_argument());
        REQUIRE(!named_arg.is_short());
        REQUIRE(named_arg.is_named());
        REQUIRE(!named_arg.has_value());
        REQUIRE(named_arg.name() == "--ab");
        REQUIRE(named_arg.as_named() == "ab");
        REQUIRE_THROWS_AS(named_arg.as_short_list(), nitro::options::parser_error);

        nitro::options::user_input short_list("-ab");
        REQUIRE(short_list.is_argument());
        REQUIRE(short_list.is_short());
        REQUIRE(!short_list.is_named());
        REQUIRE(!short_list.has_value());
        REQUIRE(short_list.name() == "-ab");
        REQUIRE_THROWS_AS(short_list.as_named(), nitro::options::parser_error);
        REQUIRE(short_list.as_short_list().size() == 2);
        REQUIRE(short_list.as_short_list().count("a") == 1);
        REQUIRE(short_list.as_short_list().count("b") == 1);
    }

    SECTION("arguments with values get properly parsed")
    {
        nitro::options::user_input short_arg("-a=5");
        REQUIRE(short_arg.is_argument());
        REQUIRE(short_arg.is_short());
        REQUIRE(!short_arg.is_named());
        REQUIRE(short_arg.has_value());
        REQUIRE(short_arg.value() == "5");
        REQUIRE(short_arg.name() == "-a");
        REQUIRE(short_arg.data() == "-a=5");

        nitro::options::user_input named_arg("--ab=5");
        REQUIRE(named_arg.is_argument());
        REQUIRE(!named_arg.is_short());
        REQUIRE(named_arg.is_named());
        REQUIRE(named_arg.has_value());
        REQUIRE(named_arg.value() == "5");
        REQUIRE(named_arg.name() == "--ab");
        REQUIRE(named_arg.data() == "--ab=5");
    }

    SECTION("Trying to break with edge cases")
    {
        nitro::options::user_input without_value("--ab=");

        REQUIRE(without_value.has_value());
        REQUIRE(without_value.value() == "");
        REQUIRE(without_value.as_named() == "ab");

        REQUIRE_THROWS_AS(nitro::options::user_input("-="), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("--="), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("---="), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("---ab="), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("---a=b"), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("---=ab"), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("--=ab"), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(nitro::options::user_input("-=ab"), nitro::options::parsing_error);
    }
}

TEST_CASE("Using argc, argv from main does compile", "[options]")
{
    int argc = 0;
    char** argv = nullptr;

    nitro::options::parser parser;

    decltype(parser.parse(argc, argv))* foo = nullptr;
    (void)foo;
}

TEST_CASE("The result of parse is default-constructable, copyable, and moveable", "[options]")
{
    int argc = 1;
    const char* argv[] = { "" };

    nitro::options::parser parser;

    nitro::options::arguments res;

    res = parser.parse(argc, argv);
    auto res2 = std::move(res);

    (void)res2;
}

TEST_CASE("Usage descriptions work", "[options]")
{
    SECTION("Description for arguments work")
    {
        nitro::options::parser parser("app_name", "about");

        parser.accept_positionals(3);
        parser.positional_metavar("command line");

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

        parser.option("env-opt", "This is an option to set cool stuff.").env("ENV_OPT");
        parser.option("env-opt-2").env("ENV_OPT2");

        std::string expected =
            R"EXPECTED(usage: app_name [-tu] --env-opt --env-opt-2 --opt --opt_long [--opt_nos]
                --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt
                [command line ...]

about


arguments:
  -t [ --tog ]                          some toggle
  -u [ --togg ]                         some other toggle
  -o [ --opt ] ARG                      some opt
  -d [ --opt_with_d ] ARG [=default value]
                                        some opt with a default
  --opt_nos ARG [=default value]        some opt without a short, but a default
  --opt_nosd ARG                        some opt without short and default
  --opt_long ARG                        an option with an very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very long description
  -x [ --some_long_named_option ] ARG [=some very long default parameter for this fucking thing]
                                        an option with an very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very long description
  -m [ --mopt ] ARG                     some multi opt
  --env-opt ARG                         This is an option to set cool stuff.
                                        Can be set using the environment
                                        variable 'ENV_OPT'.
  --env-opt-2 ARG                       Can be set using the environment
                                        variable 'ENV_OPT2'.
)EXPECTED";

        std::stringstream s;
        parser.usage(s);
        auto actual = s.str();

        CHECK(expected.size() == actual.size());

        REQUIRE(expected == actual);
    }
}

TEST_CASE("Groups", "[options]")
{
    SECTION("Usage output with groups is correct")
    {
        nitro::options::parser parser("app_name", "about");

        std::stringstream s;

        parser.accept_positionals(3);
        parser.positional_metavar("command line");

        auto& grp1 = parser.group("group1", "some text");
        auto& grp2 = parser.group("group2");

        grp1.toggle("tog", "some toggle").short_name("t");
        grp1.toggle("togg", "some other toggle").short_name("u");

        grp1.option("opt", "some opt").short_name("o");
        grp1.option("opt_with_d", "some opt with a default")
            .short_name("d")
            .default_value("default value");

        grp1.option("opt_nos", "some opt without a short, but a default")
            .default_value("default value");
        parser.group("group1").option("opt_nosd", "some opt without short and default");

        parser.group("group1").option(
            "opt_long", "an option with an very very very very very very very very very "
                        "very very very very very very very very very very very very very "
                        "very very very very very very very very long description");

        grp2.option("some_long_named_option",
                    "an option with an very very very very very very very very very "
                    "very very very very very very very very very very very very very "
                    "very very very very very very very very long description")
            .short_name("x")
            .default_value("some very long default parameter for this fucking thing");

        grp2.multi_option("mopt", "some multi opt").short_name("m");

        parser.group("group2")
            .option("env-opt", "This is an option to set cool stuff.")
            .env("ENV_OPT");
        parser.group("group2").option("env-opt-2").env("ENV_OPT2");

        parser.usage(s);

        auto actual = s.str();

        std::string expected =
            R"EXPECTED(usage: app_name [-tu] --env-opt --env-opt-2 --opt --opt_long [--opt_nos]
                --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt
                [command line ...]

about


group1:

some text

  -t [ --tog ]                          some toggle
  -u [ --togg ]                         some other toggle
  -o [ --opt ] ARG                      some opt
  -d [ --opt_with_d ] ARG [=default value]
                                        some opt with a default
  --opt_nos ARG [=default value]        some opt without a short, but a default
  --opt_nosd ARG                        some opt without short and default
  --opt_long ARG                        an option with an very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very long description

group2:
  -x [ --some_long_named_option ] ARG [=some very long default parameter for this fucking thing]
                                        an option with an very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very very very very very very very
                                        very very long description
  -m [ --mopt ] ARG                     some multi opt
  --env-opt ARG                         This is an option to set cool stuff.
                                        Can be set using the environment
                                        variable 'ENV_OPT'.
  --env-opt-2 ARG                       Can be set using the environment
                                        variable 'ENV_OPT2'.
)EXPECTED";

        CHECK(expected.size() == actual.size());

        REQUIRE(expected == actual);
    }

    SECTION("Change name of default group should work")
    {
        nitro::options::parser parser("app_name", "", "test arguments");

        std::stringstream s;

        parser.toggle("tog", "some toggle").short_name("t");
        parser.usage(s);

        REQUIRE(R"EXPECTED(usage: app_name [-t]


test arguments:
  -t [ --tog ]                          some toggle
)EXPECTED" == s.str());
    }

    SECTION("Getting group with same name")
    {
        nitro::options::parser parser("app_name", "about");

        std::stringstream s;

        auto& grp1 = parser.group("group1");
        auto& grp2 = parser.group("group1");

        REQUIRE(&grp1 == &grp2);

        auto& opt1 = grp1.option("opt");
        auto& opt2 = grp2.option("opt");

        REQUIRE(&opt1 == &opt2);
    }
}

SCENARIO("The help message is useful")
{
    GIVEN("A parser with a reversable toggle")
    {
        nitro::options::parser parser;
        auto& tog = parser.toggle("tog", "some toggle").allow_reverse();

        WHEN("the default is false")
        {
            THEN("the usage should hint that the default is false")
            {
                std::stringstream str;

                parser.usage(str);

                std::string usage = str.str();

                std::string expected =
                    R"EXPECTED(usage: main --tog


arguments:
  --[no-]tog [=no]                      some toggle
)EXPECTED";

                REQUIRE(usage == expected);
            }
        }

        WHEN("the default is true")
        {
            tog.default_value(true);

            THEN("the usage should hint that the default is true")
            {
                std::stringstream str;

                parser.usage(str);

                std::string usage = str.str();

                std::string expected =
                    R"EXPECTED(usage: main --tog


arguments:
  --[no-]tog [=yes]                     some toggle
)EXPECTED";

                REQUIRE(usage == expected);
            }
        }
    }

    GIVEN("A parser with on option")
    {
        nitro::options::parser parser;

        parser.option("ab");

        WHEN("Requesting the same option again")
        {
            parser.option("ab");

            THEN("The usage should contain the option only once")
            {
                std::stringstream str;
                parser.usage(str);
                std::string usage = str.str();

                std::string expected =
                    R"EXPECTED(usage: main --ab


arguments:
  --ab ARG
)EXPECTED";

                REQUIRE(usage == expected);
            }
        }
    }

    GIVEN("A parser with several options")
    {
        nitro::options::parser parser;

        parser.toggle("zz");
        parser.option("ab");
        parser.multi_option("aa");

        WHEN("the usage is printed")
        {
            std::stringstream str;

            parser.usage(str);

            THEN("the options should be in the order of creation")
            {
                std::string usage = str.str();

                std::string expected =
                    R"EXPECTED(usage: main --zz --ab --aa


arguments:
  --zz
  --ab ARG
  --aa ARG
)EXPECTED";

                REQUIRE(usage == expected);
            }
        }
    }

    GIVEN("A parser with several groups")
    {
        nitro::options::parser parser;

        parser.group("abc").toggle("zz");
        parser.group("aaa").option("ab");
        parser.group("abc").multi_option("aa");

        WHEN("the usage is printed")
        {
            std::stringstream str;

            parser.usage(str);

            THEN("the groups should be in the order of creation")
            {
                std::string usage = str.str();

                std::string expected =
                    R"EXPECTED(usage: main --zz --ab --aa


abc:
  --zz
  --aa ARG

aaa:
  --ab ARG
)EXPECTED";

                REQUIRE(usage == expected);
            }
        }
    }
}

TEST_CASE("Check if short option names are unique", "[options]")
{
    nitro::options::parser parser;

    parser.option("opt1").short_name("o");
    parser.option("opt2").short_name("o");
    REQUIRE_THROWS_AS(parser.parse(0, nullptr), nitro::options::parser_error);
}

TEST_CASE("Simple named arguments can get parsed from command line", "[options]")
{
    SECTION("Trying to parse unknown arguments will throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt1", "12", "--opt2", "abc" };

        nitro::options::parser parser;

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Simple string values work")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt2", "abc" };

        nitro::options::parser parser;

        parser.option("opt2");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt2") == "abc");
    }

    SECTION("Parsing integer values works")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt1", "12" };

        nitro::options::parser parser;

        parser.option("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("Missing values for option with default doesn't throw")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.option("opt3").default_value("hello");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt3") == "hello");
    }

    SECTION("Missing values for option without default throws")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt3");
    }

    SECTION("Missing values for passed option without default throws")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt3" };

        nitro::options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt3");
    }

    SECTION("Missing values for passed multi option without default throws")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt3" };

        nitro::options::parser parser;

        parser.multi_option("opt3");

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt3");
    }

    SECTION("Missing values for passed option without default throws if left out in between")
    {
        int argc = 6;
        const char* argv[] = { "", "--opt1", "value1", "--opt2", "--opt3", "value3" };

        nitro::options::parser parser;

        parser.option("opt1");
        parser.option("opt2");
        parser.option("opt3");

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt2");
    }
}

TEST_CASE("Short-cut arguments can get parsed from command line", "[options]")
{
    SECTION("Simple string short named values work")
    {
        int argc = 3;
        const char* argv[] = { "", "-a", "abc" };

        nitro::options::parser parser;

        parser.option("opt2").short_name("a");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt2") == "abc");
    }

    SECTION("Parsing short named integers values works")
    {
        int argc = 3;
        const char* argv[] = { "", "-o", "12" };

        nitro::options::parser parser;

        parser.option("opt1").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("Setting default value for short named works")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.option("opt3").short_name("o").default_value("hello");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt3") == "hello");
    }

    SECTION("When given by the user, should say so")
    {
        int argc = 3;
        const char* argv[] = { "", "--a", "test" };

        nitro::options::parser parser;

        auto& a = parser.option("a");
        auto& b = parser.option("b").default_value("");

        auto options = parser.parse(argc, argv);

        REQUIRE(a.has_non_default());
        REQUIRE(!b.has_non_default());
    }
}

TEST_CASE("Multiple arguments should work", "[options]")
{
    SECTION("Giving an option unexcepted twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "13" };

        nitro::options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Giving an option mixed with short unexcepted twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "-o", "13" };

        nitro::options::parser parser;

        parser.option("opt3").short_name("o");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Giving an identical option twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Giving two different values for for a multi_option should work")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "13" };

        nitro::options::parser parser;

        parser.multi_option("opt3");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt3") == 2);

        REQUIRE(options.get("opt3", 0) == "12");
        REQUIRE(options.get("opt3", 1) == "13");
    }

    SECTION("Giving equal values twice for a multi_option should work")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::options::parser parser;

        parser.multi_option("opt3");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt3") == 2);

        REQUIRE(options.get("opt3", 0) == "12");
        REQUIRE(options.get("opt3", 1) == "12");
    }

    SECTION("Mutli arguments get_all should work as well")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::options::parser parser;

        parser.multi_option("opt3");

        auto options = parser.parse(argc, argv);

        auto values = options.get_all("opt3");

        REQUIRE(values.size() == 2);

        REQUIRE(values[0] == "12");
        REQUIRE(values[1] == "12");
    }

    SECTION("Defining an option also as a multi_option should throw")
    {
        nitro::options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_AS(parser.multi_option("opt3"), nitro::options::parser_error);
    }

    SECTION("When given by the user, should say so")
    {
        int argc = 3;
        const char* argv[] = { "", "--a", "test" };

        nitro::options::parser parser;

        auto& a = parser.multi_option("a");
        auto& b = parser.multi_option("b").default_value({ "" });

        auto options = parser.parse(argc, argv);

        REQUIRE(a.has_non_default());
        REQUIRE(!b.has_non_default());
    }
}

TEST_CASE("positional arguments should work", "[options]")
{
    SECTION("-- should trigger everything to be positional")
    {
        int argc = 6;
        const char* argv[] = { "", "--", "--opt1", "12", "--opt2", "13" };

        nitro::options::parser parser;
        parser.accept_positionals(4);

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

        nitro::options::parser parser;
        parser.accept_positionals(4);

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 4);

        REQUIRE(options.get(-4) == "--opt1");
        REQUIRE(options.get(-3) == "12");
        REQUIRE(options.get(-2) == "--opt2");
        REQUIRE(options.get(-1) == "13");
    }

    SECTION("Given accept_positionals(), unknown arguments should still throw")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::options::parser parser;
        parser.accept_positionals();

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Given positionals when non were accepted should throw")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::options::parser parser;

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Given more positionals than accepted should throw")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::options::parser parser;
        parser.accept_positionals(3);

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("positionals should be allowed between arguments")
    {
        const char* argv[] = { "",       "--opt1",      "value1", "positional0", "--opt2",
                               "value2", "positional1", "--",     "positional2" };

        nitro::options::parser parser;
        parser.option("opt1");
        parser.option("opt2");
        parser.accept_positionals(3);

        auto options = parser.parse(9, argv);

        REQUIRE(options.positionals().size() == 3);

        CHECK(options.get(0) == "positional0");
        CHECK(options.get(1) == "positional1");
        CHECK(options.get(2) == "positional2");

        CHECK(options.get("opt1") == "value1");
        CHECK(options.get("opt2") == "value2");
    }

    SECTION("when double dash is repeated, it should be a positional")
    {
        int argc = 3;
        const char* argv[] = { "", "--", "--" };

        nitro::options::parser parser;
        parser.accept_positionals(1);

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 1);

        CHECK(options.get(0) == "--");
    }
}

TEST_CASE("Giving values with an '=' should work ")
{

    SECTION("for named arguments")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt1=12" };

        nitro::options::parser parser;

        parser.option("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("for short arguments")
    {
        int argc = 2;
        const char* argv[] = { "", "-o=12" };

        nitro::options::parser parser;

        parser.option("opt1").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.as<int>("opt1") == 12);
    }

    SECTION("for toggles it should throw")
    {
        int argc = 2;
        const char* argv[] = { "", "-o=12" };

        nitro::options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }
}

TEST_CASE("Defining an option twice should return the same object twice")
{
    SECTION("for arguments")
    {
        nitro::options::parser parser;

        auto obj1 = &parser.option("opt1");
        auto obj2 = &parser.option("opt1");

        REQUIRE(obj1 == obj2);
    }

    SECTION("for multi_options")
    {
        nitro::options::parser parser;

        auto obj1 = &parser.multi_option("opt1");
        auto obj2 = &parser.multi_option("opt1");

        REQUIRE(obj1 == obj2);
    }

    SECTION("for toggles")
    {
        nitro::options::parser parser;

        auto obj1 = &parser.toggle("opt1");
        auto obj2 = &parser.toggle("opt1");

        REQUIRE(obj1 == obj2);
    }
}

TEST_CASE("Trying to redefine short_name")
{
    SECTION("for arguments should throw")
    {
        nitro::options::parser parser;

        parser.option("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.option("opt1").short_name("p"), nitro::options::parser_error);
    }

    SECTION("for multi_options should throw")
    {
        nitro::options::parser parser;

        parser.multi_option("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.multi_option("opt1").short_name("p"),
                          nitro::options::parser_error);
    }

    SECTION("for toggle should throw")
    {
        nitro::options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.toggle("opt1").short_name("p"), nitro::options::parser_error);
    }

    SECTION("for arguments with same short name_ should NOT throw")
    {
        nitro::options::parser parser;

        parser.option("opt1").short_name("o");

        REQUIRE_NOTHROW(parser.option("opt1").short_name("o"));
    }

    SECTION("for multi_options with same short name_ should NOT throw")
    {
        nitro::options::parser parser;

        parser.multi_option("opt1").short_name("o");

        REQUIRE_NOTHROW(parser.multi_option("opt1").short_name("o"));
    }

    SECTION("for toggle with same short name_ should NOT throw")
    {
        nitro::options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_NOTHROW(parser.toggle("opt1").short_name("o"));
    }
}

TEST_CASE("Toggles should work", "[options]")
{
    SECTION("for named arguments")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt1" };

        nitro::options::parser parser;

        parser.toggle("opt1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt1"));
    }

    SECTION("for short arguments")
    {
        int argc = 2;
        const char* argv[] = { "", "-o" };

        nitro::options::parser parser;

        parser.toggle("opt2").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt2"));
    }

    SECTION("given multiple times")
    {
        int argc = 4;
        const char* argv[] = { "", "-o", "-o", "-o" };

        nitro::options::parser parser;

        parser.toggle("opt2").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt2") == 3);
    }

    SECTION("when not given")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.toggle("opt2").short_name("o");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("opt2"));
    }

    SECTION("when given a value")
    {
        int argc = 3;
        const char* argv[] = { "", "-o", "value" };

        nitro::options::parser parser;
        parser.toggle("opt2").short_name("o");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("for multiple toggles in one arg")
    {
        int argc = 2;
        const char* argv[] = { "", "-ab" };

        nitro::options::parser parser;

        parser.toggle("opt_a").short_name("a");
        parser.toggle("opt_b").short_name("b");
        parser.toggle("opt_c").short_name("c");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt_a"));
        REQUIRE(options.given("opt_b"));
        REQUIRE(!options.given("opt_c"));
    }

    SECTION("When given by the user, should say so")
    {
        int argc = 2;
        const char* argv[] = { "", "--a" };

        nitro::options::parser parser;

        auto& a = parser.toggle("a");
        auto& b = parser.toggle("b");

        auto options = parser.parse(argc, argv);

        REQUIRE(a.has_non_default());
        REQUIRE(!b.has_non_default());
    }

    SECTION("When printing reversable")
    {
        nitro::options::parser parser("app_name");

        parser.toggle("tog", "some toggle").short_name("t").allow_reverse();

        std::stringstream str;

        parser.usage(str);
        auto actual = str.str();

        auto expected = R"EXPECTED(usage: app_name [-t]


arguments:
  -t [ --[no-]tog ] [=no]               some toggle
)EXPECTED";

        REQUIRE(actual == expected);
    }
}

TEST_CASE("Reading the value from the ENV variables", "[options]")
{
    SECTION("for arguments when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.option("opt1").env("OPT1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt1") == "OPT1_VALUE");
    }

    SECTION("for arguments with a default when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.option("opt1").env("OPT1").default_value("DEFAULT");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt1") == "OPT1_VALUE");
    }

    SECTION("for arguments with a default when given a value")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt1", "foo" };

        nitro::options::parser parser;

        parser.option("opt1").env("OPT1").default_value("DEFAULT");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt1") == "foo");
    }

    SECTION("for toggles when not given a value and env TRUE")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.toggle("opt2").env("OPT2");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt2"));
    }

    SECTION("for toggles when not given a value and env FALSE")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.toggle("opt3").env("OPT3");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("opt3"));
    }

    SECTION("for toggles when given a value and env FALSE")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt3" };

        nitro::options::parser parser;

        parser.toggle("opt3").env("OPT3");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt3"));
    }

    SECTION("for multi arguments when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.multi_option("opt4").env("OPT4");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt4") == 2);
        CHECK(options.get("opt4", 0) == "OPT4_VALUE0");
        CHECK(options.get("opt4", 1) == "OPT4_VALUE1");
    }

    SECTION("for multi arguments with a default when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.multi_option("opt4").env("OPT4").default_value({ "DEFAULT" });

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt4") == 2);
        CHECK(options.get("opt4", 0) == "OPT4_VALUE0");
        CHECK(options.get("opt4", 1) == "OPT4_VALUE1");
    }

    SECTION("for multi arguments with a default when given a value")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt4", "foo" };

        nitro::options::parser parser;

        parser.multi_option("opt4").env("OPT4").default_value({ "foo" });

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt4") == 1);
        CHECK(options.get("opt4", 0) == "foo");
    }
}

TEST_CASE("Usage metavar work")
{
    SECTION("metavar for arguments work")
    {
        nitro::options::parser parser("app_name", "about");

        std::stringstream s;

        parser.option("opt_nosd", "some opt without short and default").metavar("test");

        parser.usage(s);

        REQUIRE(s.str() ==
                R"EXPECTED(usage: app_name --opt_nosd

about


arguments:
  --opt_nosd test                       some opt without short and default
)EXPECTED");
    }

    SECTION("metavar for multi arguments work")
    {
        nitro::options::parser parser("app_name", "about");

        std::stringstream s;

        parser.multi_option("mopt", "some multi opt").metavar("test");

        parser.usage(s);

        REQUIRE(s.str() ==
                R"EXPECTED(usage: app_name --mopt

about


arguments:
  --mopt test                           some multi opt
)EXPECTED");
    }
}

TEST_CASE("parsing toggles with prefix and default value")
{
    SECTION("test user_input with --no- is parsed right")
    {
        nitro::options::user_input arg("--no-arg");
        REQUIRE(arg.has_prefix());
        REQUIRE(arg.name_without_prefix() == "arg");
    }

    SECTION("user_input without prefix work")
    {
        nitro::options::user_input arg("--arg");
        REQUIRE(!arg.has_prefix());
        REQUIRE_THROWS_AS(arg.name_without_prefix(), nitro::options::parser_error);
    }

    SECTION("cannot provide --arg and --no-arg at the same time")
    {
        int argc = 3;
        const char* argv[] = { "", "--arg", "--no-arg" };

        nitro::options::parser parser;

        parser.toggle("arg").allow_reverse();

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("Refuse to parse --no-arg if reverse isn't allowed")
    {
        int argc = 2;
        const char* argv[] = { "", "--no-arg" };

        nitro::options::parser parser;

        parser.toggle("arg");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("cannot provide --no-arg and --arg at the same time")
    {
        int argc = 3;
        const char* argv[] = { "", "--no-arg", "--arg" };

        nitro::options::parser parser;

        parser.toggle("arg").allow_reverse();

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::options::parsing_error);
    }

    SECTION("parsing an user_input with prefix and testing check() function")
    {
        int argc = 2;
        const char* argv[] = { "", "--no-arg" };

        nitro::options::parser parser;

        parser.toggle("arg").allow_reverse();

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("arg"));
    }

    SECTION("test default_value() function with positiv value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.toggle("arg").default_value(true);

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("arg"));
    }

    SECTION("test default_value()) function with negativ value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.toggle("arg").default_value(false);

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("arg"));
    }

    SECTION("test negativ env_var")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::options::parser parser;

        parser.toggle("arg").env("OPT3");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("arg"));
    }

    SECTION("test parse_env_value() function with positiv env_value")
    {
        using namespace nitro::options;

        REQUIRE(toggle::parse_env_value("TRUE"));
        REQUIRE(toggle::parse_env_value("True"));
        REQUIRE(toggle::parse_env_value("true"));
        REQUIRE(toggle::parse_env_value("1"));
        REQUIRE(toggle::parse_env_value("ON"));
        REQUIRE(toggle::parse_env_value("On"));
        REQUIRE(toggle::parse_env_value("on"));
        REQUIRE(toggle::parse_env_value("WITH"));
        REQUIRE(toggle::parse_env_value("with"));
        REQUIRE(toggle::parse_env_value("y"));
        REQUIRE(toggle::parse_env_value("Y"));
        REQUIRE(toggle::parse_env_value("YES"));
        REQUIRE(toggle::parse_env_value("Yes"));
        REQUIRE(toggle::parse_env_value("yes"));
    }

    SECTION("test parse_env_value() function with negativ env_value")
    {
        using namespace nitro::options;

        REQUIRE(!toggle::parse_env_value("False"));
        REQUIRE(!toggle::parse_env_value("FALSE"));
        REQUIRE(!toggle::parse_env_value("false"));
        REQUIRE(!toggle::parse_env_value("OFF"));
        REQUIRE(!toggle::parse_env_value("Off"));
        REQUIRE(!toggle::parse_env_value("off"));
        REQUIRE(!toggle::parse_env_value("0"));
        REQUIRE(!toggle::parse_env_value("without"));
        REQUIRE(!toggle::parse_env_value("WITHOUT"));
        REQUIRE(!toggle::parse_env_value("NO"));
        REQUIRE(!toggle::parse_env_value("No"));
        REQUIRE(!toggle::parse_env_value("no"));
        REQUIRE(!toggle::parse_env_value("N"));
        REQUIRE(!toggle::parse_env_value("n"));
    }

    SECTION("test parse_env_value() function with error env_value")
    {
        using namespace nitro::options;

        REQUIRE_THROWS_AS(toggle::parse_env_value("fddgh"), nitro::options::parsing_error);
        REQUIRE_THROWS_AS(toggle::parse_env_value("fdd-gdegh"), nitro::options::parsing_error);
    }

    SECTION("arguments with prefix get properly parsed")
    {
        nitro::options::user_input named_arg("--no-ab=5");
        REQUIRE(named_arg.is_argument());
        REQUIRE(!named_arg.is_short());
        REQUIRE(named_arg.is_named());
        REQUIRE(named_arg.has_value());
        REQUIRE(named_arg.has_prefix());
        REQUIRE(named_arg.value() == "5");
        REQUIRE(named_arg.name() == "--no-ab");
        REQUIRE(named_arg.name_without_prefix() == "ab");
        REQUIRE(named_arg.data() == "--no-ab=5");
    }
}

SCENARIO("options and multi_options can be optional, but without default values")
{
    GIVEN("A parser with an option and a multi_option")
    {
        nitro::options::parser parser;

        auto& opt = parser.option("opt");
        auto& mopt = parser.multi_option("mopt");

        WHEN("An optional option isn't provided")
        {
            opt.optional();

            THEN("it should not raise an exception")
            {
                int argc = 3;
                const char* argv[] = { "", "--mopt", "foo" };

                REQUIRE_NOTHROW(parser.parse(argc, argv));
            }

            THEN("it should not be provided")
            {
                int argc = 3;
                const char* argv[] = { "", "--mopt", "foo" };

                auto arguments = parser.parse(argc, argv);

                REQUIRE(!arguments.provided("opt"));
            }

            THEN("the access will raise an exception")
            {
                int argc = 3;
                const char* argv[] = { "", "--mopt", "foo" };

                auto arguments = parser.parse(argc, argv);

                REQUIRE_THROWS(arguments.get_all("opt"));
            }

            THEN("is_optional() should return true")
            {
                REQUIRE(opt.is_optional() == true);
            }
        }

        WHEN("An optional multi_option isn't provided")
        {
            mopt.optional();

            THEN("it should not raise an exception")
            {
                int argc = 3;
                const char* argv[] = { "", "--opt", "foo" };

                REQUIRE_NOTHROW(parser.parse(argc, argv));
            }

            THEN("it should not be provided")
            {
                int argc = 3;
                const char* argv[] = { "", "--opt", "foo" };

                auto arguments = parser.parse(argc, argv);

                REQUIRE(!arguments.provided("mopt"));
            }

            THEN("the access will return an empty vector")
            {
                int argc = 3;
                const char* argv[] = { "", "--opt", "foo" };

                auto arguments = parser.parse(argc, argv);

                auto v = arguments.get_all("mopt");

                REQUIRE(v.size() == 0);
            }

            THEN("is_optional() should return true")
            {
                REQUIRE(mopt.is_optional() == true);
            }
        }
    }
}

SCENARIO("a multi_option is usable")
{
    GIVEN("A parser with a multi_option with an empty list as default value")
    {
        nitro::options::parser parser;

        auto& mopt = parser.multi_option("mopt").default_value({});

        WHEN("No arguments are given")
        {
            int argc = 1;
            const char* argv[] = { "" };

            nitro::options::arguments arguments;

            THEN("the parsing is successful and the parsed argument is an empty list")
            {
                REQUIRE_NOTHROW(arguments = parser.parse(argc, argv));
                REQUIRE(arguments.count("mopt") == 0);
            }
        }

        WHEN("Arguments are given")
        {
            int argc = 3;
            const char* argv[] = { "", "--mopt", "value1" };

            nitro::options::arguments arguments;

            THEN("the parsing is successful and the parsed argument is not an empty list")
            {
                REQUIRE_NOTHROW(arguments = parser.parse(argc, argv));
                REQUIRE(arguments.count("mopt") == 1);
                REQUIRE(arguments.get("mopt", 0) == "value1");
            }
        }

        WHEN("Arguments are given as ENV")
        {
            int argc = 1;
            const char* argv[] = { "" };

            mopt.env("OPT4");

            nitro::options::arguments arguments;

            THEN("the parsing is successful and the parsed argument is not an empty list")
            {
                REQUIRE_NOTHROW(arguments = parser.parse(argc, argv));
                REQUIRE(arguments.count("mopt") == 2);
                REQUIRE(arguments.get("mopt", 0) == "OPT4_VALUE0");
                REQUIRE(arguments.get("mopt", 1) == "OPT4_VALUE1");
            }
        }
    }
}

SCENARIO("provided arguments are different from default arguments", "[options]")
{
    GIVEN("A parser with some options")
    {
        nitro::options::parser parser;

        auto& opt = parser.option("opt").default_value("value");
        auto& tog = parser.toggle("tog");
        auto& mopt = parser.multi_option("mopt").default_value({});

        WHEN("no arguments are given")
        {
            int argc = 1;
            const char* argv[] = { "" };

            auto arguments = parser.parse(argc, argv);

            THEN("all options should be marked as not provided")
            {
                REQUIRE(!opt.has_non_default());
                REQUIRE(!tog.has_non_default());
                REQUIRE(!mopt.has_non_default());

                REQUIRE(!arguments.provided("opt"));
                REQUIRE(!arguments.provided("tog"));
                REQUIRE(!arguments.provided("mopt"));
            }
        }

        WHEN("the option argument is given")
        {
            int argc = 3;
            const char* argv[] = { "", "--opt", "foo" };

            auto arguments = parser.parse(argc, argv);

            THEN("only the option should be marked as provided")
            {
                REQUIRE(opt.has_non_default());
                REQUIRE(!tog.has_non_default());
                REQUIRE(!mopt.has_non_default());

                REQUIRE(arguments.provided("opt"));
                REQUIRE(!arguments.provided("tog"));
                REQUIRE(!arguments.provided("mopt"));
            }
        }

        WHEN("the multi option argument is given")
        {
            int argc = 3;
            const char* argv[] = { "", "--mopt", "foo" };

            auto arguments = parser.parse(argc, argv);

            THEN("only the multi option should be marked as provided")
            {
                REQUIRE(!opt.has_non_default());
                REQUIRE(!tog.has_non_default());
                REQUIRE(mopt.has_non_default());

                REQUIRE(!arguments.provided("opt"));
                REQUIRE(!arguments.provided("tog"));
                REQUIRE(arguments.provided("mopt"));
            }
        }

        WHEN("the toggle argument is given")
        {
            int argc = 2;
            const char* argv[] = { "", "--tog" };

            auto arguments = parser.parse(argc, argv);

            THEN("only the toggle should be marked as provided")
            {
                REQUIRE(!opt.has_non_default());
                REQUIRE(tog.has_non_default());
                REQUIRE(!mopt.has_non_default());

                REQUIRE(!arguments.provided("opt"));
                REQUIRE(arguments.provided("tog"));
                REQUIRE(!arguments.provided("mopt"));
            }
        }
    }
}
