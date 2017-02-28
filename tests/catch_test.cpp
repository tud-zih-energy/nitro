#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/lang/catch.hpp>

#include <stdexcept>
#include <string>

template <typename E, typename... Args>
void throw_it(const std::string& what)
{
    throw E(what);
}

TEST_CASE("exceptions will be catched", "[lang]")
{
    SECTION("std::exception should be catched by default")
    {
        REQUIRE_NOTHROW(nitro::lang::make_catch(throw_it<std::logic_error>, "Something"));
    }

    SECTION("specific exceptions should be catched")
    {
        REQUIRE_NOTHROW(
            nitro::lang::make_catch<std::logic_error>(throw_it<std::logic_error>, "Something"));
    }

    SECTION("only specified exceptions should be catched")
    {
        REQUIRE_THROWS(
            nitro::lang::make_catch<std::runtime_error>(throw_it<std::logic_error>, "Something"));
    }

    SECTION("throwing something else than std::exception, should not be catched")
    {
        REQUIRE_THROWS(nitro::lang::make_catch(throw_it<std::string>, "Something"));
    }
}
