#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/format/format.hpp>

#include <sstream>

TEST_CASE("Simple format strings", "[format]")
{
    SECTION("can be printed to an ostream")
    {
        std::stringstream s;
        s << nitro::format("Hello {}") % "World";

        REQUIRE(s.str() == "Hello World");
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

    SECTION("can format using a user-literal")
    {
        const char* ref_str = "Hello world";

        std::string out = "Hello {}"_nf % "world";

        REQUIRE(out == std::string(ref_str));
    }
}

TEST_CASE("Args works")
{
    SECTION("Can be called with zero arguments")
    {
        auto fmt = "Test"_nf;
        fmt.args();
        std::string out = fmt;

        REQUIRE(out == "Test");
    }

    SECTION("Can be called with multiple arguments")
    {
        auto fmt = "Test {} {} {}"_nf;
        fmt.args(1, "foo", 5.25004005);
        std::string out = fmt;

        REQUIRE(out == "Test 1 foo 5.25004");
    }

    SECTION("Can be called mixed with arguments")
    {
        auto fmt = "Test {} {}"_nf;
        fmt % 1;
        fmt.args("foo");
        std::string out = fmt;

        REQUIRE(out == "Test 1 foo");
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

TEST_CASE("Format can be used in exptions and raise")
{
    SECTION("Passing a format works")
    {
        REQUIRE_THROWS_WITH(nitro::raise("This is an exception formatted with nitro::format"_nf),
                            "This is an exception formatted with nitro::format");
    }

    SECTION("Formatting the exception message works")
    {
        REQUIRE_THROWS_WITH(
            nitro::raise("This is an exception formatted with {}"_nf % "nitro::format"),
            "This is an exception formatted with nitro::format");
    }
}
