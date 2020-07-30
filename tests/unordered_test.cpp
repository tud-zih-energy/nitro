#if __cplusplus > 201402L

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <nitro/lang/unordered.hpp>
#include <variant>

TEST_CASE("Test Hashing")
{

    SECTION("filling work")
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

    SECTION("filling with hashable nitro classes work")
    {
        class MyHashable : public nitro::lang::hashable
        {
        private:
            std::string str_;

        public:
            MyHashable(std::string str) : str_(str)
            {
            }

            auto hash() const
            {
                return std::hash<std::string>()(str_);
            }

            std::string str()
            {
                return str_;
            }

            bool operator==(std::string rhs) const
            {
                return str_ == rhs;
            }
            bool operator==(MyHashable rhs) const
            {
                return str_ == rhs.str();
            }
        };

        nitro::lang::unordered_set<std::variant<int, MyHashable>> test;

        std::variant<int, MyHashable> x, y, z;
        x.emplace<0>(12);
        y.emplace<1>(MyHashable("Test1"));
        z.emplace<1>(MyHashable("Test2"));

        test.insert(x);
        test.insert(y);
        test.insert(z);

        REQUIRE(test.size() == 3);
    }
}

#endif