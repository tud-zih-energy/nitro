#include <catch2/catch_test_macros.hpp>

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

    A(const A&)
    {
        std::cout << "A(const A&)" << std::endl;
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

class C
{
public:
    C(int i) : i(i)
    {
        std::cout << "C(int)" << std::endl;
    }

    C(const C&) = delete;
    C(C&&)
    {
        std::cout << "C(C&&)" << std::endl;
    }

    ~C()
    {
        std::cout << "~C()" << std::endl;
    }

    int i;
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

    SECTION("can be used with move-only types")
    {
        C c(42);

        auto p = nitro::lang::make_quaint<C>(std::move(c));
    }

    SECTION("can be accessed with as<>()")
    {
        auto p = nitro::lang::make_quaint<A>();

        auto a = p.as<A>();
    }

    SECTION("can be resetted")
    {
        auto p = nitro::lang::make_quaint<A>();

        p.reset();

        CHECK(p.get() == nullptr);
    }
}
