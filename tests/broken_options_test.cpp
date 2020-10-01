#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/broken_options/parser.hpp>

TEST_CASE("argument class works as intended")
{
    SECTION("double dash gets parsed correctly")
    {
        nitro::broken_options::argument arg("--");

        REQUIRE(arg.is_double_dash());
    }

    SECTION("double dash does not get confised with a named argument")
    {
        nitro::broken_options::argument arg("--not-a-double-dash");

        REQUIRE(!arg.is_double_dash());
    }

    SECTION("double dash does not get confised with a short")
    {
        nitro::broken_options::argument arg("-v");

        REQUIRE(!arg.is_double_dash());
    }

    SECTION("Values get parsed as such")
    {
        nitro::broken_options::argument arg("some_value");
        REQUIRE(arg.is_value());
        REQUIRE(arg.value() == "some_value");
        REQUIRE_THROWS_AS(arg.name(), nitro::broken_options::parser_error);
    }

    SECTION("Values get not confused with named arguments")
    {
        nitro::broken_options::argument arg("--some-arg");
        REQUIRE(!arg.is_value());
    }

    SECTION("Values get not confused with short list")
    {
        nitro::broken_options::argument arg("-abcd");
        REQUIRE(!arg.is_value());
    }

    SECTION("short arguments get recognized properly")
    {
        REQUIRE(nitro::broken_options::argument("-a").is_short());
        REQUIRE(nitro::broken_options::argument("-ab").is_short());
        REQUIRE(!nitro::broken_options::argument("--ab").is_short());
        REQUIRE(!nitro::broken_options::argument("ab").is_short());
        REQUIRE(!nitro::broken_options::argument("--").is_short());
    }

    SECTION("named arguments get recognized properly")
    {
        REQUIRE(!nitro::broken_options::argument("-a").is_named());
        REQUIRE(!nitro::broken_options::argument("-ab").is_named());
        REQUIRE(nitro::broken_options::argument("--ab").is_named());
        REQUIRE(!nitro::broken_options::argument("ab").is_named());
        REQUIRE(!nitro::broken_options::argument("--").is_named());
    }

    SECTION("arguments get recognized properly")
    {
        REQUIRE(nitro::broken_options::argument("-a").is_argument());
        REQUIRE(nitro::broken_options::argument("-ab").is_argument());
        REQUIRE(nitro::broken_options::argument("--ab").is_argument());
        REQUIRE(!nitro::broken_options::argument("ab").is_argument());
        REQUIRE(!nitro::broken_options::argument("--").is_argument());
    }

    SECTION("arguments get properly parsed")
    {
        nitro::broken_options::argument short_arg("-a");
        REQUIRE(short_arg.is_argument());
        REQUIRE(short_arg.is_short());
        REQUIRE(!short_arg.is_named());
        REQUIRE(!short_arg.has_value());
        REQUIRE(short_arg.name() == "-a");
        REQUIRE_THROWS_AS(short_arg.as_named(), nitro::broken_options::parser_error);
        REQUIRE(short_arg.as_short_list().size() == 1);
        REQUIRE(short_arg.as_short_list().count("a") == 1);

        nitro::broken_options::argument named_arg("--ab");
        REQUIRE(named_arg.is_argument());
        REQUIRE(!named_arg.is_short());
        REQUIRE(named_arg.is_named());
        REQUIRE(!named_arg.has_value());
        REQUIRE(named_arg.name() == "--ab");
        REQUIRE(named_arg.as_named() == "ab");
        REQUIRE_THROWS_AS(named_arg.as_short_list(), nitro::broken_options::parser_error);

        nitro::broken_options::argument short_list("-ab");
        REQUIRE(short_list.is_argument());
        REQUIRE(short_list.is_short());
        REQUIRE(!short_list.is_named());
        REQUIRE(!short_list.has_value());
        REQUIRE(short_list.name() == "-ab");
        REQUIRE_THROWS_AS(short_list.as_named(), nitro::broken_options::parser_error);
        REQUIRE(short_list.as_short_list().size() == 2);
        REQUIRE(short_list.as_short_list().count("a") == 1);
        REQUIRE(short_list.as_short_list().count("b") == 1);
    }

    SECTION("arguments with values get properly parsed")
    {
        nitro::broken_options::argument short_arg("-a=5");
        REQUIRE(short_arg.is_argument());
        REQUIRE(short_arg.is_short());
        REQUIRE(!short_arg.is_named());
        REQUIRE(short_arg.has_value());
        REQUIRE(short_arg.value() == "5");
        REQUIRE(short_arg.name() == "-a");
        REQUIRE(short_arg.data() == "-a=5");

        nitro::broken_options::argument named_arg("--ab=5");
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
        nitro::broken_options::argument without_value("--ab=");

        REQUIRE(without_value.has_value());
        REQUIRE(without_value.value() == "");
        REQUIRE(without_value.as_named() == "ab");

        REQUIRE_THROWS_AS(nitro::broken_options::argument("-="),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("--="),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("---="),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("---ab="),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("---a=b"),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("---=ab"),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("--=ab"),
                          nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(nitro::broken_options::argument("-=ab"),
                          nitro::broken_options::parsing_error);
    }
}

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

        parser.accept_positionals(3);
        parser.positional_name("command line");

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

        parser.usage(s);

        auto actual = s.str();

        std::string expected =
            R"EXPECTED(usage: app_name [-tu] --env-opt --env-opt-2 --opt --opt_long [--opt_nos] --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt [command line ...]

about


arguments:
  --env-opt arg                           This is an option to set cool stuff.
                                          Can be set using the environment
                                          variable 'ENV_OPT'.
  --env-opt-2 arg                         Can be set using the environment
                                          variable 'ENV_OPT2'.
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
)EXPECTED";

        CHECK(actual.size() == expected.size());

        std::size_t tmp = 0;
        for (unsigned long i = 0; i < actual.size(); ++i)
        {
            if (actual[i] != expected[i])
            {
                std::cout << i << " " << actual[i] << " " << expected[i] << std::endl;
                ++tmp;
                if (tmp > 100)
                    break;
            }
        }

        REQUIRE(actual == expected);
    }
}

TEST_CASE("Groups work")
{
    SECTION("Groups work")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        parser.accept_positionals(3);
        parser.positional_name("command line");

        parser.group("group1").toggle("tog", "some toggle").short_name("t");
        parser.group("group1").toggle("togg", "some other toggle").short_name("u");

        parser.group("group1").option("opt", "some opt").short_name("o");
        parser.group("group1")
            .option("opt_with_d", "some opt with a default")
            .short_name("d")
            .default_value("default value");

        parser.group("group1")
            .option("opt_nos", "some opt without a short, but a default")
            .default_value("default value");
        parser.group("group1").option("opt_nosd", "some opt without short and default");

        parser.group("group1").option(
            "opt_long", "an option with an very very very very very very very very very "
                        "very very very very very very very very very very very very very "
                        "very very very very very very very very long description");

        parser.group("group2")
            .option("some_long_named_option",
                    "an option with an very very very very very very very very very "
                    "very very very very very very very very very very very very very "
                    "very very very very very very very very long description")
            .short_name("x")
            .default_value("some very long default parameter for this fucking thing");

        parser.group("group2").multi_option("mopt", "some multi opt").short_name("m");

        parser.group("group2")
            .option("env-opt", "This is an option to set cool stuff.")
            .env("ENV_OPT");
        parser.group("group2").option("env-opt-2").env("ENV_OPT2");

        parser.usage(s, false);

        auto actual = s.str();
        std::string expected =
            R"EXPECTED(usage: app_name [-tu] --env-opt --env-opt-2 --opt --opt_long [--opt_nos] --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt [command line ...]

about


group1:
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
  -t [ --tog ]                            some toggle
  -u [ --togg ]                           some other toggle

group2:
  --env-opt arg                           This is an option to set cool stuff.
                                          Can be set using the environment
                                          variable 'ENV_OPT'.
  --env-opt-2 arg                         Can be set using the environment
                                          variable 'ENV_OPT2'.
  -m [ --mopt ] arg                       some multi opt
  -x [ --some_long_named_option ] [=some very long default parameter for this fucking thing]
                                          an option with an very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very long
                                          description
)EXPECTED";

        CHECK(actual.size() == expected.size());

        std::size_t tmp = 0;
        for (unsigned long i = 0; i < actual.size(); ++i)
        {
            if (actual[i] != expected[i])
            {
                std::cout << i << " " << actual[i] << " " << expected[i] << std::endl;
                ++tmp;
                if (tmp > 100)
                    break;
            }
        }

        REQUIRE(actual == expected);
    }

    SECTION("Groups work 2")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        parser.accept_positionals(3);
        parser.positional_name("command line");

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
        grp1.option("opt_nosd", "some opt without short and default");

        grp1.option("opt_long", "an option with an very very very very very very very very very "
                                "very very very very very very very very very very very very very "
                                "very very very very very very very very long description");

        grp2.option("some_long_named_option",
                    "an option with an very very very very very very very very very "
                    "very very very very very very very very very very very very very "
                    "very very very very very very very very long description")
            .short_name("x")
            .default_value("some very long default parameter for this fucking thing");

        grp2.multi_option("mopt", "some multi opt").short_name("m");

        grp2.option("env-opt", "This is an option to set cool stuff.").env("ENV_OPT");
        grp2.option("env-opt-2").env("ENV_OPT2");

        parser.usage(s, false);

        auto actual = s.str();
        std::string expected =
            R"EXPECTED(usage: app_name [-tu] --env-opt --env-opt-2 --opt --opt_long [--opt_nos] --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt [command line ...]

about


group1:

some text

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
  -t [ --tog ]                            some toggle
  -u [ --togg ]                           some other toggle

group2:
  --env-opt arg                           This is an option to set cool stuff.
                                          Can be set using the environment
                                          variable 'ENV_OPT'.
  --env-opt-2 arg                         Can be set using the environment
                                          variable 'ENV_OPT2'.
  -m [ --mopt ] arg                       some multi opt
  -x [ --some_long_named_option ] [=some very long default parameter for this fucking thing]
                                          an option with an very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very very very
                                          very very very very very long
                                          description
)EXPECTED";

        CHECK(actual.size() == expected.size());

        std::size_t tmp = 0;
        for (unsigned long i = 0; i < actual.size(); ++i)
        {
            if (actual[i] != expected[i])
            {
                std::cout << i << " " << actual[i] << " " << expected[i] << std::endl;
                ++tmp;
                if (tmp > 100)
                    break;
            }
        }

        REQUIRE(actual == expected);
    }

    SECTION("Change Default Group")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        parser.default_group("test arguments");
        parser.accept_positionals(3);
        parser.positional_name("command line");

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

        parser.usage(s);

        REQUIRE(
            s.str() ==
            R"EXPECTED(usage: app_name [-tu] --env-opt --env-opt-2 --opt --opt_long [--opt_nos] --opt_nosd [--opt_with_d] [--some_long_named_option] --mopt [command line ...]

about


test arguments:
  --env-opt arg                           This is an option to set cool stuff.
                                          Can be set using the environment
                                          variable 'ENV_OPT'.
  --env-opt-2 arg                         Can be set using the environment
                                          variable 'ENV_OPT2'.
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

    SECTION("Multiple groups with same name should be one group")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        auto grp1 = parser.group("group1");
        auto grp2 = parser.group("group2");

        grp1.toggle("tog", "should work");
        REQUIRE_THROWS_AS(grp2.toggle("tog", "should fail"), nitro::broken_options::parser_error);
    }

    SECTION("Groups work 2")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        auto grp1 = parser.group("group1");
        auto grp2 = parser.group("group1");

        REQUIRE(grp1 == grp2);
    }
}

TEST_CASE("Check if short option names are unique")
{
    nitro::broken_options::parser parser;

    parser.option("opt1").short_name("o");
    parser.option("opt2").short_name("o");
    REQUIRE_THROWS_AS(parser.parse(0, nullptr), nitro::broken_options::parser_error);
}

TEST_CASE("Simple named arguments can get parsed from command line", "[broken_options]")
{
    SECTION("Trying to parse unknown arguments will throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt1", "12", "--opt2", "abc" };

        nitro::broken_options::parser parser;

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
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

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt3");
    }

    SECTION("Missing values for passed option without default throws")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt3" };

        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt3");
    }

    SECTION("Missing values for passed option without default throws if left out in between")
    {
        int argc = 6;
        const char* argv[] = { "", "--opt1", "value1", "--opt2", "--opt3", "value3" };

        nitro::broken_options::parser parser;

        parser.option("opt1");
        parser.option("opt2");
        parser.option("opt3");

        REQUIRE_THROWS_WITH(parser.parse(argc, argv), "missing value for required option: opt2");
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

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
    }

    SECTION("Giving an option mixed with short unexcepted twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "-o", "13" };

        nitro::broken_options::parser parser;

        parser.option("opt3").short_name("o");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
    }

    SECTION("Giving an identical option twice should throw")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
    }

    SECTION("Giving two different values for for a multi_option should work")
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

    SECTION("Giving equal values twice for a multi_option should work")
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

    SECTION("Mutli options get_all should work as well")
    {
        int argc = 5;
        const char* argv[] = { "", "--opt3", "12", "--opt3", "12" };

        nitro::broken_options::parser parser;

        parser.multi_option("opt3");

        auto options = parser.parse(argc, argv);

        auto values = options.get_all("opt3");

        REQUIRE(values.size() == 2);

        REQUIRE(values[0] == "12");
        REQUIRE(values[1] == "12");
    }

    SECTION("Defining an option also as a multi_option should throw")
    {
        nitro::broken_options::parser parser;

        parser.option("opt3");

        REQUIRE_THROWS_AS(parser.multi_option("opt3"), nitro::broken_options::parser_error);
    }
}

TEST_CASE("positional arguments should work", "[broken_options]")
{
    SECTION("-- should trigger everything to be positional")
    {
        int argc = 6;
        const char* argv[] = { "", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;
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

        nitro::broken_options::parser parser;
        parser.accept_positionals(4);

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 4);

        REQUIRE(options.get(-4) == "--opt1");
        REQUIRE(options.get(-3) == "12");
        REQUIRE(options.get(-2) == "--opt2");
        REQUIRE(options.get(-1) == "13");
    }

    SECTION("Given accept_positionals(), unknown options should still throw")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;
        parser.accept_positionals();

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
    }

    SECTION("Given positionals when non were accepted should throw")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
    }

    SECTION("Given more positionals than accepted should throw")
    {
        int argc = 8;
        const char* argv[] = { "", "--unknown", "value", "--", "--opt1", "12", "--opt2", "13" };

        nitro::broken_options::parser parser;
        parser.accept_positionals(3);

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
    }

    SECTION("positionals should be allowed between arguments")
    {
        const char* argv[] = { "",       "--opt1",      "value1", "positional0", "--opt2",
                               "value2", "positional1", "--",     "positional2" };

        nitro::broken_options::parser parser;
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

        nitro::broken_options::parser parser;
        parser.accept_positionals(1);

        auto options = parser.parse(argc, argv);

        REQUIRE(options.positionals().size() == 1);

        CHECK(options.get(0) == "--");
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

    SECTION("for toggles it should throw")
    {
        int argc = 2;
        const char* argv[] = { "", "-o=12" };

        nitro::broken_options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
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

        REQUIRE_THROWS_AS(parser.option("opt1").short_name("p"),
                          nitro::broken_options::parser_error);
    }

    SECTION("for multi_options should throw")
    {
        nitro::broken_options::parser parser;

        parser.multi_option("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.multi_option("opt1").short_name("p"),
                          nitro::broken_options::parser_error);
    }

    SECTION("for toggle should throw")
    {
        nitro::broken_options::parser parser;

        parser.toggle("opt1").short_name("o");

        REQUIRE_THROWS_AS(parser.toggle("opt1").short_name("p"),
                          nitro::broken_options::parser_error);
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

    SECTION("when given a value")
    {
        int argc = 3;
        const char* argv[] = { "", "-o", "value" };

        nitro::broken_options::parser parser;
        parser.toggle("opt2").short_name("o");

        REQUIRE_THROWS_AS(parser.parse(argc, argv), nitro::broken_options::parsing_error);
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

TEST_CASE("Reading the value from the ENV variables", "[broken_options]")
{
    SECTION("for options when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt1").env("OPT1");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt1") == "OPT1_VALUE");
    }

    SECTION("for options with a default when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.option("opt1").env("OPT1").default_value("DEFAULT");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt1") == "OPT1_VALUE");
    }

    SECTION("for options with a default when given a value")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt1", "foo" };

        nitro::broken_options::parser parser;

        parser.option("opt1").env("OPT1").default_value("DEFAULT");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.get("opt1") == "foo");
    }

    SECTION("for toggles when not given a value and env TRUE")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.toggle("opt2").env("OPT2");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt2"));
    }

    SECTION("for toggles when not given a value and env FALSE")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.toggle("opt3").env("OPT3");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("opt3"));
    }

    SECTION("for toggles when given a value and env FALSE")
    {
        int argc = 2;
        const char* argv[] = { "", "--opt3" };

        nitro::broken_options::parser parser;

        parser.toggle("opt3").env("OPT3");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("opt3"));
    }

    SECTION("for multi options when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.multi_option("opt4").env("OPT4");

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt4") == 2);
        CHECK(options.get("opt4", 0) == "OPT4_VALUE0");
        CHECK(options.get("opt4", 1) == "OPT4_VALUE1");
    }

    SECTION("for multi options with a default when not given a value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.multi_option("opt4").env("OPT4").default_value({ "DEFAULT" });

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt4") == 2);
        CHECK(options.get("opt4", 0) == "OPT4_VALUE0");
        CHECK(options.get("opt4", 1) == "OPT4_VALUE1");
    }

    SECTION("for multi options with a default when given a value")
    {
        int argc = 3;
        const char* argv[] = { "", "--opt4", "foo" };

        nitro::broken_options::parser parser;

        parser.multi_option("opt4").env("OPT4").default_value({ "foo" });

        auto options = parser.parse(argc, argv);

        REQUIRE(options.count("opt4") == 1);
        CHECK(options.get("opt4", 0) == "foo");
    }
}

TEST_CASE("Usage metavar work")
{
    SECTION("metavar for options work")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        parser.option("opt_nosd", "some opt without short and default").metavar("test");

        parser.usage(s);

        REQUIRE(s.str() ==
                R"EXPECTED(usage: app_name --opt_nosd

about


arguments:
  --opt_nosd test                         some opt without short and default
)EXPECTED");
    }

    SECTION("metavar for multi options work")
    {
        nitro::broken_options::parser parser("app_name", "about");

        std::stringstream s;

        parser.multi_option("mopt", "some multi opt").metavar("test");

        parser.usage(s);

        REQUIRE(s.str() ==
                R"EXPECTED(usage: app_name --mopt

about


arguments:
  --mopt test                             some multi opt
)EXPECTED");
    }
}

TEST_CASE("parsing toggles with prefix and default value")
{
    SECTION("test argument with --no- is parsed right")
    {
        nitro::broken_options::argument arg("--no-arg");
        REQUIRE(arg.has_prefix());
        REQUIRE(arg.name_without_prefix() == "arg");
    }

    SECTION("argument without prefix work")
    {
        nitro::broken_options::argument arg("--arg");
        REQUIRE(!arg.has_prefix());
        REQUIRE_THROWS_AS(arg.name_without_prefix(), nitro::broken_options::parser_error);
    }

    SECTION("parsing an argument with prefix and testing check() function")
    {
        int argc = 2;
        const char* argv[] = { "", "--no-arg" };

        nitro::broken_options::parser parser;

        parser.toggle("arg");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("arg"));
    }

    SECTION("test default_value() function with positiv value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.toggle("arg").default_value(true);

        auto options = parser.parse(argc, argv);

        REQUIRE(options.given("arg"));
    }

    SECTION("test default_value()) function with negativ value")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.toggle("arg").default_value(false);

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("arg"));
    }

    SECTION("test negativ env_var")
    {
        int argc = 1;
        const char* argv[] = { "" };

        nitro::broken_options::parser parser;

        parser.toggle("arg").env("OPT3");

        auto options = parser.parse(argc, argv);

        REQUIRE(!options.given("arg"));
    }

    SECTION("test parse_env_value() function with positiv env_value")
    {
        using namespace nitro::broken_options;

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
        using namespace nitro::broken_options;

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
        using namespace nitro::broken_options;

        REQUIRE_THROWS_AS(toggle::parse_env_value("fddgh"), nitro::broken_options::parsing_error);
        REQUIRE_THROWS_AS(toggle::parse_env_value("fdd-gdegh"),
                          nitro::broken_options::parsing_error);
    }

    SECTION("arguments with prefix get properly parsed")
    {
        nitro::broken_options::argument named_arg("--no-ab=5");
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