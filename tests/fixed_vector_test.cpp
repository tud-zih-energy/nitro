#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/lang/fixed_vector.hpp>

TEST_CASE("fixed vector init", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t> v(10);

    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 10);
}

TEST_CASE("fixed vector read and write", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t> v(10);

    v.emplace_back(5);
    v.emplace_back(7);

    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 5);
    REQUIRE(v.at(1) == 7);
    REQUIRE(v.back() == 7);
}

TEST_CASE("fixed vector erase", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t> v(10);

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

    nitro::lang::fixed_vector<std::int64_t> v(10, a);

    REQUIRE(v.front() == 0);
    REQUIRE(v.back() == 9);
    REQUIRE(v.size() == 10);
}

TEST_CASE("fixed vector array assignment with less elements", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(10, a);

    REQUIRE(v.front() == 0);
    REQUIRE(v.back() == 5);
    REQUIRE(v.size() == 6);
    REQUIRE(v.capacity() == 10);
}

TEST_CASE("fixed vector std::get access", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(6, a);

    REQUIRE(std::get<2>(v) == 2);
    REQUIRE(v.get<3>() == 3);
}

TEST_CASE("fixed vector bracket access", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(6, a);

    v[0] = 7;
    v[2] = 9;
    REQUIRE(v.get<0>() == 7);
    REQUIRE(v.get<2>() == 9);
}

TEST_CASE("fixed vector at() access", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(6, a);

    v.at(0) = 7;
    v.at(2) = 9;
    REQUIRE(v.get<0>() == 7);
    REQUIRE(v.get<2>() == 9);
}

TEST_CASE("fixed vector merge two vectors #1", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };
    std::array<std::int64_t, 6> b = { 6, 7, 8, 9, 10, 11 };

    nitro::lang::fixed_vector<std::int64_t> v(12, a);
    nitro::lang::fixed_vector<std::int64_t> w(6, b);

    v.emplace(w.begin(), w.end());

    REQUIRE(v.size() == 12);
}

TEST_CASE("fixed vector merge two vectors #2", "[lang]")
{
    std::array<std::int64_t, 6> a = { 0, 1, 2, 3, 4, 5 };
    std::array<std::int64_t, 6> b = { 6, 7, 8, 9, 10, 11 };

    nitro::lang::fixed_vector<std::int64_t> v(12, a);
    nitro::lang::fixed_vector<std::int64_t> w(6, b);

    v.merge(w);

    REQUIRE(v.size() == 12);
}

// TEST_CASE("fixed vector references in storage test", "[lang]")
// {
//
//     nitro::lang::fixed_vector<std::int64_t&> v(6);
//
//     int64_t a = 1;
//     int64_t b = 2;
//     int64_t c = 3;
//
//     v.insert(a);
//     v.insert(b);
//     v.insert(c);
//
//     REQUIRE(v.get<0>() == 1);
//
//     a = 4;
//
//     REQUIRE(v.get<0>() == 4);
// }