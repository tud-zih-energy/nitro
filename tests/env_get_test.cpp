#include <catch2/catch.hpp>

#include <nitro/env/get.hpp>

TEST_CASE("environment variabls read", "[env]")
{
    REQUIRE(nitro::env::get("TEST_1") == "THIS_WAS_SET");

    REQUIRE(nitro::env::get("TEST_2", "THIS_WAS_NOT_SET") == "THIS_WAS_NOT_SET");
}

TEST_CASE("environment variables read without default", "[env]")
{
    REQUIRE(nitro::env::get("TEST_1", nitro::env::no_default) == "THIS_WAS_SET");

    REQUIRE_THROWS(nitro::env::get("TEST_2", nitro::env::no_default));
}
