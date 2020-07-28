#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/lang/unordered.hpp>
#include <variant>

TEST_CASE("Test Hashing")
{
    nitro::lang::unordered_set<std::variant<int, long>> test;

    std::variant<int, long> x, y;
    x = 12;
    y = 25;

    test.insert(x);
    test.insert(y);

    REQUIRE(test.size() == 2);
}
