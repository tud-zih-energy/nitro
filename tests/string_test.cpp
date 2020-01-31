#include <nitro/lang/string.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("String join works", "[lang]")
{
    std::vector<std::string> str;
    str.emplace_back("Hello");
    str.emplace_back("World");

    SECTION("using the vector overload")
    {
        REQUIRE(nitro::lang::join(str) == "Hello World");
    }

    SECTION("using the iterator overload")
    {
        REQUIRE(nitro::lang::join(str.begin(), str.end()) == "Hello World");
    }

    SECTION("joining an empty vector gives an empty string")
    {
        REQUIRE(nitro::lang::join({}) == std::string());
    }

    SECTION("joining an empty range gives an empty string")
    {
        REQUIRE(nitro::lang::join(str.begin(), str.begin()) == std::string());
    }

    SECTION("joining with a different infix works")
    {
        REQUIRE(nitro::lang::join(str, " cruel ") == "Hello cruel World");
    }

    SECTION("joining with an empty infix works")
    {
        REQUIRE(nitro::lang::join(str, std::string()) == "HelloWorld");
    }

    SECTION("joining a non string vector works too")
    {
        std::vector<int> ints;
        ints.emplace_back(1);
        ints.emplace_back(2);
        ints.emplace_back(3);
        ints.emplace_back(4);
        ints.emplace_back(5);
        ints.emplace_back(6);
        ints.emplace_back(7);
        ints.emplace_back(8);
        ints.emplace_back(9);
        ints.emplace_back(10);

        REQUIRE(nitro::lang::join(ints.begin(), ints.end()) == "1 2 3 4 5 6 7 8 9 10");
    }
}

TEST_CASE("String split works", "[lang]")
{
    std::string str = "1234##5678##9101112";

    SECTION("splitting with char works")
    {
        auto split = nitro::lang::split(str, "#");

        REQUIRE(split.size() == 5);
        REQUIRE(split[0] == "1234");
        REQUIRE(split[1] == "");
        REQUIRE(split[2] == "5678");
        REQUIRE(split[3] == "");
        REQUIRE(split[4] == "9101112");
    }

    SECTION("splitting with string works")
    {
        auto split = nitro::lang::split(str, "##");

        REQUIRE(split.size() == 3);
        REQUIRE(split[0] == "1234");
        REQUIRE(split[1] == "5678");
        REQUIRE(split[2] == "9101112");
    }

    SECTION("splitting with string works if at the beginning")
    {
        auto split = nitro::lang::split(str, "1234");

        REQUIRE(split.size() == 2);
        REQUIRE(split[0] == "");
        REQUIRE(split[1] == "##5678##9101112");
    }

    SECTION("splitting with string works if at the end")
    {
        auto split = nitro::lang::split(str, "112");

        REQUIRE(split.size() == 2);
        REQUIRE(split[0] == "1234##5678##9101");
        REQUIRE(split[1] == "");
    }

    SECTION("splitting an empty string works")
    {
        auto split = nitro::lang::split({}, "112");

        REQUIRE(split.size() == 1);
        REQUIRE(split[0] == "");
    }

    SECTION("splitting with an empty string throws")
    {
        REQUIRE_THROWS(nitro::lang::split(str, {}));
    }

    SECTION("splitting without match works")
    {
        auto split = nitro::lang::split(str, "@@@");

        REQUIRE(split.size() == 1);
        REQUIRE(split[0] == "1234##5678##9101112");
    }
}
