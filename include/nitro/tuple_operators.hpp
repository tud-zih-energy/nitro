#ifndef INCLUDE_NITRO_TUPLE_OPERATORS_HPP
#define INCLUDE_NITRO_TUPLE_OPERATORS_HPP

// included on purpose, though not required in this file for convenience
#include <tuple>

#include <type_traits>

namespace nitro
{
template <typename T>
struct tuple_operators
{
    inline friend bool operator!=(const T& x, const T& y)
    {
        return x.as_tuple() != y.as_tuple();
    }
    inline friend bool operator==(const T& x, const T& y)
    {
        return x.as_tuple() == y.as_tuple();
    }
    inline friend bool operator<(const T& x, const T& y)
    {
        return x.as_tuple() < y.as_tuple();
    }
    inline friend bool operator>(const T& x, const T& y)
    {
        return x.as_tuple() > y.as_tuple();
    }
    inline friend bool operator<=(const T& x, const T& y)
    {
        return x.as_tuple() <= y.as_tuple();
    }
    inline friend bool operator>=(const T& x, const T& y)
    {
        return x.as_tuple() >= y.as_tuple();
    }
};
}

#endif // INCLUDE_NITRO_TUPLE_OPERATORS_HPP
