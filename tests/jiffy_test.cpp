#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/jiffy/jiffy.hpp>

#include <chrono>

TEST_CASE("Print 100 jiffies in iso format", "[jiffy]")
{
    for (int i = 0; i < 100; i++)
    {
        nitro::jiffy::Jiffy j;

        std::cout << j << std::endl;
    }
}

TEST_CASE("Jiffy can be taken", "[jiffy]")
{
    SECTION("Default constructed")
    {
        nitro::jiffy::Jiffy now;
    }

    SECTION("Chrono constructed")
    {
        nitro::jiffy::Jiffy now(std::chrono::system_clock::now());
    }
}

TEST_CASE("Jiffy can be printed with a fraction", "[jiffy]")
{
    std::chrono::system_clock::time_point ctp(std::chrono::microseconds(1542297437305463));

    nitro::jiffy::Jiffy tp(ctp);

    REQUIRE(tp.isoformat() == "2018-11-15T16:57:17.305463+0100");

    REQUIRE_THROWS(nitro::jiffy::Jiffy("2018-11-15T16:57:17.305463+0100"));
}

TEST_CASE("Jiffy are comparable", "[jiffy]")
{
    nitro::jiffy::Jiffy now;

    REQUIRE(now == now);
}

TEST_CASE("Jiffy can be parsed without a fraction", "[jiffy]")
{
    std::chrono::system_clock::time_point ctp(std::chrono::microseconds(1542297437000000));

    nitro::jiffy::Jiffy tp(ctp);

    REQUIRE(tp.isoformat() == "2018-11-15T16:57:17.000000+0100");

    REQUIRE(nitro::jiffy::Jiffy("2018-11-15T16:57:17+0100") == tp);
}
