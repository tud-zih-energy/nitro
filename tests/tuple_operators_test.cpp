#include <catch2/catch_test_macros.hpp>

#include <nitro/lang/tuple_operators.hpp>

struct my_int : nitro::lang::tuple_operators<my_int>
{
    my_int(int i) : value_(i)
    {
    }

    auto as_tuple()
    {
        return std::tie(value_);
    }

private:
    int value_;
};

TEST_CASE("simple class comparison works", "[tuple_operators]")
{
    my_int one(1);
    my_int zero(0);

    REQUIRE(one == one);
    REQUIRE(one != zero);
    REQUIRE(one > zero);
    REQUIRE(zero < one);
}
