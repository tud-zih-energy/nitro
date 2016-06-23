#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/env/get.hpp>

TEST_CASE("environment variabls read", "[env]")
{
    REQUIRE(nitro::env::get("TEST_1") == "THIS_WAS_SET");

    REQUIRE(nitro::env::get("TEST_2", "THIS_WAS_NOT_SET") == "THIS_WAS_NOT_SET");
}
