#include <nitro/lang/string.hpp>

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

TEST_CASE("String starts_with works", "[lang]")
{
    SECTION("Egg and Spam starts with Egg")
    {
        REQUIRE(nitro::lang::starts_with("Egg and Spam", "Egg") == true);
    }

    SECTION("Egg and Spam doesn't sart with Spam")
    {
        REQUIRE(nitro::lang::starts_with("Egg and Spam", "Spam") == false);
    }

    SECTION("Egg doesn't start with Egg and Spam")
    {
        REQUIRE(nitro::lang::starts_with("Egg", "Egg and Spam") == false);
    }
    SECTION("All strings start with the empty string")
    {
        REQUIRE(nitro::lang::starts_with("Egg", "") == true);
    }
    SECTION("The empty string starts with the empty string")
    {
        REQUIRE(nitro::lang::starts_with("", "") == true);
    }

    SECTION("the empty string starts with no string")
    {
        REQUIRE(nitro::lang::starts_with("", "Egg") == false);
    }
}

TEST_CASE("String replace_all works", "[lang]")
{
    SECTION("Egg, Bacon and Bacon becomes Egg, Spam and Spam")
    {
        std::string str("Egg, Bacon and Bacon");
        nitro::lang::replace_all(str, "Bacon", "Spam");
        REQUIRE(str == "Egg, Spam and Spam");
    }
    SECTION("replace_all handles replaces that are longer than the original")
    {
        std::string str("Egg and Spam");
        nitro::lang::replace_all(str, " and Spam", ", Sausage and Spam");
        REQUIRE(str == "Egg, Sausage and Spam");
    }
    SECTION("Replacing with empty string")
    {
        std::string str("Egg, Spam, Spam, Spam");
        nitro::lang::replace_all(str, ", Spam", "");
        REQUIRE(str == "Egg");
    }
    SECTION("Replaces in an empty string")
    {
        std::string str("");
        nitro::lang::replace_all(str, "Egg", "Spam");
        REQUIRE(str == "");
    }
}
