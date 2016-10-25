#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/lang/enumerate.hpp>

#include <array>
#include <initializer_list>
#include <vector>

using nitro::lang::enumerate;

TEST_CASE("referenced containers can be enumerated", "[lang]")
{
    std::vector<int> reference = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

    SECTION("std::vector can be accessed enumerated")
    {
        std::vector<int> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("std::array can be accessed enumerated")
    {
        std::array<int, 10> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("array can be accessed enumerated")
    {
        int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("std::vector can be modified enumerated")
    {
        std::vector<int> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto&& i : enumerate(input))
        {
            i.value() *= 2;

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index] * 2);

            index++;
        }
    }

    SECTION("std::array can be modified enumerated")
    {
        std::array<int, 10> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto&& i : enumerate(input))
        {
            i.value() *= 2;

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index] * 2);

            index++;
        }
    }

    SECTION("array can be modified enumerated")
    {
        int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {
            i.value() *= 2;

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index] * 2);

            index++;
        }
    }
}

TEST_CASE("r-value containers can be enumerated", "[lang]")
{
    std::vector<int> reference = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

    SECTION("std::vector can be accessed enumerated")
    {
        std::size_t index = 0;

        for (auto i : enumerate(std::vector<int>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } }))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("std::array can be accessed enumerated")
    {
        std::size_t index = 0;

        for (auto i : enumerate(std::array<int, 10>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } }))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("initializer list can be accessed enumerated")
    {
        std::size_t index = 0;

        for (auto i : enumerate({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }
}
