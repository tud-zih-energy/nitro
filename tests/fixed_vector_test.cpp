#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/lang/fixed_vector.hpp>

TEST_CASE("fixed vector init", "[lang]")
{
    constexpr int SIZE = 10;

    nitro::lang::fixed_vector<std::int64_t> v(SIZE);

    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 10);
}

TEST_CASE("fixed vector read and write", "[lang]")
{
    constexpr int SIZE = 10;

    nitro::lang::fixed_vector<std::int64_t> v(SIZE);

    v.emplace_back(5);
    v.emplace_back(7);

    REQUIRE(v.size() == 2);
    REQUIRE(v[0] == 5);
    REQUIRE(v.front() == 5);
    REQUIRE(v.at(1) == 7);
    REQUIRE(v.back() == 7);
}

TEST_CASE("fixed vector erase", "[lang]")
{
    constexpr int SIZE = 10;

    nitro::lang::fixed_vector<std::int64_t> v(SIZE);

    v.emplace_back(5);
    v.emplace_back(7);
    v.erase(v.begin());

    REQUIRE(v.back() == 7);
    REQUIRE(v.front() == 7);
    REQUIRE(v.size() == 1);
}

TEST_CASE("fixed vector array assignment", "[lang]")
{
    
    constexpr int SIZE = 10;
    constexpr std::array<std::int64_t, 10> INIT_VALUES = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    nitro::lang::fixed_vector<std::int64_t> v(SIZE, INIT_VALUES);

    REQUIRE(v.front() == 0);
    REQUIRE(v.back() == 9);
    REQUIRE(v.size() == 10);
}

TEST_CASE("fixed vector array assignment with less elements", "[lang]")
{
    constexpr int SIZE = 10;
    constexpr std::array<std::int64_t, 6> INIT_VALUES = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(SIZE, INIT_VALUES);

    REQUIRE(v.front() == 0);
    REQUIRE(v.back() == 5);
    REQUIRE(v.size() == 6);
    REQUIRE(v.capacity() == 10);
}

TEST_CASE("fixed vector std::get access", "[lang]")
{
    constexpr int SIZE = 6;
    constexpr std::array<std::int64_t, 6> INIT_VALUES = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(SIZE, INIT_VALUES);

    REQUIRE(std::get<2>(v) == 2);
    REQUIRE(std::get<3>(v) == 3);
}

TEST_CASE("fixed vector bracket access", "[lang]")
{
    constexpr int SIZE = 10;
    constexpr std::array<std::int64_t, 6> INIT_VALUES = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(SIZE, INIT_VALUES);

    v[0] = 7;
    v[2] = 9;
    REQUIRE(v[0] == 7);
    REQUIRE(v[2] == 9);
}

TEST_CASE("fixed vector at() access", "[lang]")
{
    constexpr int SIZE = 10;
    constexpr std::array<std::int64_t, 6> INIT_VALUES = { 0, 1, 2, 3, 4, 5 };

    nitro::lang::fixed_vector<std::int64_t> v(SIZE, INIT_VALUES);

    v.at(0) = 7;
    v.at(2) = 9;
    REQUIRE(v.at(0) == 7);
    REQUIRE(v.at(2) == 9);
}

TEST_CASE("fixed vector merge two vectors", "[lang]")
{
    constexpr int SIZE_DOUBLE = 12;
    constexpr int SIZE_SINGLE = 6;

    constexpr std::array<std::int64_t, 6> INIT_VALUES_A = { 0, 1, 2, 3, 4, 5 };
    constexpr std::array<std::int64_t, 6> INIT_VALUES_B = { 6, 7, 8, 9, 10, 11 };

    nitro::lang::fixed_vector<std::int64_t> v(SIZE_DOUBLE, INIT_VALUES_A);
    nitro::lang::fixed_vector<std::int64_t> w(SIZE_SINGLE, INIT_VALUES_B);

    v.push_back(w.begin(), w.end());

    REQUIRE(v.size() == 12);
}

TEST_CASE("fixed vector init with init list", "[lang]")
{
    nitro::lang::fixed_vector<std::int64_t> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    REQUIRE(v.size() == 12);
}

TEST_CASE("fixed vector with unique ptr", "[lang]")
{
    constexpr int SIZE = 2;

    nitro::lang::fixed_vector<std::unique_ptr<std::int64_t>> v(SIZE);
    v.insert(std::move(std::make_unique<std::int64_t>(5)));
    v.insert(std::move(std::make_unique<std::int64_t>(6)));

    REQUIRE_NOTHROW(v.erase(v.begin()));
}

// TEST_CASE("fixed vector references in storage test", "[lang]")
// {
//     nitro::lang::fixed_vector<std::reference_wrapper<std::int64_t>> v(6);
//     int64_t a = 1;
//     int64_t b = 2;
//     int64_t c = 3;
//     v.emplace_back(std::ref(a));
//     v.emplace_back(std::ref(b));
//     v.emplace_back(std::ref(c));
//     REQUIRE(v[0] == 1);
//     a = 4;
//     REQUIRE(v[0] == 4);
// }