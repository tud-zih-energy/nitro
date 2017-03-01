#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/lang/quaint_ptr.hpp>

#include <iostream>
#include <vector>

class A
{
public:
    A()
    {
        std::cout << "A()" << std::endl;
    }

    ~A()
    {
        std::cout << "~A()" << std::endl;
    }
};

class B
{
public:
    B()
    {
        std::cout << "B()" << std::endl;
    }

    ~B()
    {
        std::cout << "~B()" << std::endl;
    }
};

TEST_CASE("quaint_ptr can be used", "[lang]")
{
    SECTION("can be made and destruct")
    {
        auto p = nitro::lang::make_quaint<A>();
    }

    SECTION("can be put into vectors")
    {
        std::vector<nitro::lang::quaint_ptr> ptrs;

        ptrs.push_back(nitro::lang::make_quaint<A>());
    }

    SECTION("can be mixed in vectors")
    {
        std::vector<nitro::lang::quaint_ptr> ptrs;

        ptrs.push_back(nitro::lang::make_quaint<A>());
        ptrs.push_back(nitro::lang::make_quaint<B>());
    }
}
