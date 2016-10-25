#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/lang/reverse.hpp>

#include <array>
#include <vector>

using nitro::lang::reverse;

TEST_CASE("referenced containers can be reversed", "[lang]")
{
    std::vector<int> reference = { { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 } };

    SECTION("std::vector can be accessed reverse")
    {
        std::vector<int> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto i : reverse(input))
        {

            REQUIRE(i == reference[index]);

            index++;
        }
    }

    SECTION("std::array can be accessed reverse")
    {
        std::array<int, 10> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto i : reverse(input))
        {

            REQUIRE(i == reference[index]);

            index++;
        }
    }

    SECTION("array can be accessed reverse")
    {
        int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::size_t index = 0;

        for (auto i : reverse(input))
        {

            REQUIRE(i == reference[index]);

            index++;
        }
    }

    SECTION("std::vector can be modified reverse")
    {
        std::vector<int> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto& i : reverse(input))
        {
            i *= 2;
        }

        for (auto i : reverse(input))
        {
            REQUIRE(i == reference[index] * 2);

            index++;
        }
    }

    SECTION("std::array can be modified reverse")
    {
        std::array<int, 10> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto& i : reverse(input))
        {
            i *= 2;
        }

        for (auto i : reverse(input))
        {
            REQUIRE(i == reference[index] * 2);

            index++;
        }
    }

    SECTION("array can be modified reverse")
    {
        int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::size_t index = 0;

        for (auto& i : reverse(input))
        {
            i *= 2;
        }

        for (auto i : reverse(input))
        {
            REQUIRE(i == reference[index] * 2);

            index++;
        }
    }
}

TEST_CASE("r-value containers can be reversed", "[lang]")
{
    std::vector<int> reference = { { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 } };

    SECTION("std::vector can be accessed reverse")
    {
        std::size_t index = 0;

        for (auto i : reverse(std::vector<int>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } }))
        {

            REQUIRE(i == reference[index]);

            index++;
        }
    }

    SECTION("std::array can be accessed reverse")
    {
        std::size_t index = 0;

        for (auto i : reverse(std::array<int, 10>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } }))
        {

            REQUIRE(i == reference[index]);

            index++;
        }
    }

    SECTION("initializer list can be accessed reverse")
    {
        std::size_t index = 0;

        for (auto i : reverse({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }))
        {

            REQUIRE(i == reference[index]);

            index++;
        }
    }
}
