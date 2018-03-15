#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/meta/callable.hpp>

TEST_CASE("is_callable trait works", "[meta]")
{
    SECTION("for free functions")
    {
        using foo = double (*)(double);

        CHECK((nitro::meta::is_callable<foo, double(double)>::value == true));
        CHECK((nitro::meta::is_callable<foo, double(void)>::value == false));
        CHECK((nitro::meta::is_callable<foo, void(double)>::value == false));
        CHECK((nitro::meta::is_callable<foo, void(void)>::value == false));
    }

    SECTION("for std::function's")
    {
        using foo = std::function<double(double)>;

        CHECK((nitro::meta::is_callable<foo, double(double)>::value == true));
        CHECK((nitro::meta::is_callable<foo, double(void)>::value == false));
        CHECK((nitro::meta::is_callable<foo, void(double)>::value == false));
        CHECK((nitro::meta::is_callable<foo, void(void)>::value == false));
    }

    SECTION("for lambdas")
    {
        auto fool = [](double x) { return x; };
        using foo = decltype(fool);

        CHECK((nitro::meta::is_callable<foo, double(double)>::value == true));
        CHECK((nitro::meta::is_callable<foo, double(void)>::value == false));
        CHECK((nitro::meta::is_callable<foo, void(double)>::value == false));
        CHECK((nitro::meta::is_callable<foo, void(void)>::value == false));
    }
    SECTION("with conversions")
    {
        int x;
        auto fool = [&x](double) { return "hello"; };
        using foo = decltype(fool);

        CHECK((nitro::meta::is_callable<foo, const char*(double)>::value == true));
        CHECK((nitro::meta::is_callable<foo, std::string(double)>::value == true));
    }
}
