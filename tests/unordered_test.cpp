#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/lang/unordered.hpp>
#include <variant>

TEST_CASE("Test Hashing")
{
    nitro::lang::unordered_set<std::variant<int, int, long>> test;

    std::variant<int, int, long> x, y, z;
    x.emplace<0>(12);
    y.emplace<1>(125);
    z.emplace<2>(12598593775L);

    test.insert(x);
    test.insert(y);
    test.insert(z);

    REQUIRE(test.size() == 3);
}
