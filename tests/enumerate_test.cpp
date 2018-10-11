#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nitro/lang/enumerate.hpp>

#include <array>
#include <initializer_list>
#include <vector>

using nitro::lang::enumerate;

template <typename T>
struct NonDefaultIterator
{
    NonDefaultIterator() = delete;
    NonDefaultIterator(T* it) : it(it)
    {
    }

    NonDefaultIterator& operator++()
    {
        it++;
        return *this;
    }

    NonDefaultIterator operator++(int)
    {
        NonDefaultIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    T& operator*()
    {
        return *it;
    }

    bool operator!=(const NonDefaultIterator& other)
    {
        return it != other.it;
    }

    T* it;
};

template <typename Container>
struct Wrapper
{
    using value_type = typename std::remove_reference<decltype(std::declval<Container>()[0])>::type;

    Wrapper(Container& c) : c(c)
    {
    }

    NonDefaultIterator<value_type> begin()
    {
        return { &c[0] };
    }

    NonDefaultIterator<value_type> end()
    {
        return { (&c[std::distance(std::begin(c), std::end(c)) - 1]) + 1 };
    }

    Container& c;
};

TEST_CASE("referenced containers can be enumerated", "[lang]")
{
    std::vector<int> reference = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

    SECTION("std::vector can be accessed enumerated")
    {
        std::vector<int> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("std::array can be accessed enumerated")
    {
        std::array<int, 10> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("array can be accessed enumerated")
    {
        int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("std::vector can be modified enumerated")
    {
        std::vector<int> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto&& i : enumerate(input))
        {
            i.value() *= 2;

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index] * 2);

            index++;
        }
    }

    SECTION("std::array can be modified enumerated")
    {
        std::array<int, 10> input = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        std::size_t index = 0;

        for (auto&& i : enumerate(input))
        {
            i.value() *= 2;

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index] * 2);

            index++;
        }
    }

    SECTION("array can be modified enumerated")
    {
        int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::size_t index = 0;

        for (auto i : enumerate(input))
        {
            i.value() *= 2;

            CHECK(i.index() == index);
            CHECK(i.value() == reference[index] * 2);

            index++;
        }
    }
}

TEST_CASE("r-value containers can be enumerated", "[lang]")
{
    std::vector<int> reference = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

    SECTION("std::vector can be accessed enumerated")
    {
        std::size_t index = 0;

        for (auto i : enumerate(std::vector<int>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } }))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("std::array can be accessed enumerated")
    {
        std::size_t index = 0;

        for (auto i : enumerate(std::array<int, 10>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } }))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }

    SECTION("initializer list can be accessed enumerated")
    {
        std::size_t index = 0;

        for (auto i : enumerate({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }
}

TEST_CASE("iterators without a default constructoir can be used", "[lang]")
{
    std::vector<int> reference = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

    SECTION("iteration works")
    {
        std::size_t index = 0;

        auto vec = std::vector<int>{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        Wrapper<std::vector<int>> c(vec);

        for (auto i : enumerate(c))
        {
            CHECK(i.index() == index);
            CHECK(i.value() == reference[index]);

            index++;
        }
    }
}
