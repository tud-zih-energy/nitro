#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/lang/fixed_vector.hpp>

TEST_CASE("fixed vector init", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t, 10> v;

    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 10);
}

TEST_CASE("fixed vector read and write", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t, 10> v;

    v.emplace_back(5);
    v.emplace_back(7);

    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 5);
    REQUIRE(v.at(1) == 7);
    REQUIRE(v.back() == 7);
}

TEST_CASE("fixed vector erase", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t, 10> v;

    v.emplace_back(5);
    v.emplace_back(7);
    v.erase(0);

    REQUIRE(v.back() == 7);
    REQUIRE(v.front() == 7);
    REQUIRE(v.size() == 1);
}

TEST_CASE("fixed vector array assignment", "[lang]")
{
    std::array<std::int64_t, 10> a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    nitro::lang::fixed_vector<std::int64_t, 10> v(a);

    REQUIRE(v.front() == 0);
    REQUIRE(v.back() == 9);
    REQUIRE(v.size() == 10);
}

TEST_CASE("fixed vector array assignment with less elements", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t, 10> v(a);

    REQUIRE(v.front() == 0);
    REQUIRE(v.back() == 5);
    REQUIRE(v.size() == 6);
    REQUIRE(v.capacity() == 10);
}