#ifndef INCLUDE_NITRO_TUPLE_OPERATORS_HPP
#define INCLUDE_NITRO_TUPLE_OPERATORS_HPP

// included on purpose, though not required in this file for convenience
#include <tuple>

namespace nitro
{
namespace lang
{
    namespace helper
    {
        template <int... Is>
        struct seq
        {
        };

        template <int N, int... Is>
        struct gen_seq : gen_seq<N - 1, N - 1, Is...>
        {
        };

        template <int... Is>
        struct gen_seq<0, Is...> : seq<Is...>
        {
        };

        template <typename T, typename F, int... Is>
        inline void for_each(T&& t, F f, seq<Is...>)
        {
            auto l = { (f(std::get<Is>(t)), 0)... };

            // l is only for meta bullshit
            (void)l;
        }

        template <typename... Ts, typename F>
        inline void for_each_in_tuple(std::tuple<Ts...> t, F f)
        {
            for_each(t, f, gen_seq<sizeof...(Ts)>());
        }

        template <class Archive>
        struct serialize_functor
        {
            serialize_functor(Archive& ar) : ar(ar)
            {
            }

            template <typename T>
            void operator()(T& t)
            {
                ar& t;
            }

            Archive& ar;
        };

        template <class Archive, typename... Args>
        inline void serialize(Archive& ar, std::tuple<Args&...> t)
        {
            for_each_in_tuple(t, serialize_functor<Archive>(ar));
        }

        template <class Archive, typename... Args, bool False = false>
        inline void serialize(Archive&, std::tuple<const Args&...>)
        {
            static_assert(False,
                          "as_tuple() must NOT be declared const in order to enable serialization");
        }

        template <class Archive, typename... Args, bool False = false>
        inline void serialize(Archive&, std::tuple<Args...>)
        {
            static_assert(
                False,
                "as_tuple() must be implemented using std::tie in order to enable serialization");
        }

        template <typename... Args>
        inline std::tuple<const Args&...> constify(std::tuple<Args&...> t)
        {
            return t;
        }
    }

    template <typename T>
    struct tuple_operators
    {
        inline friend bool operator!=(const T& x, const T& y)
        {
            return as_tuple(x) != as_tuple(y);
        }
        inline friend bool operator==(const T& x, const T& y)
        {
            return as_tuple(x) == as_tuple(y);
        }
        inline friend bool operator<(const T& x, const T& y)
        {
            return as_tuple(x) < as_tuple(y);
        }
        inline friend bool operator>(const T& x, const T& y)
        {
            return as_tuple(x) > as_tuple(y);
        }
        inline friend bool operator<=(const T& x, const T& y)
        {
            return as_tuple(x) <= as_tuple(y);
        }
        inline friend bool operator>=(const T& x, const T& y)
        {
            return as_tuple(x) >= as_tuple(y);
        }

        template <class Archive>
        inline void serialize(Archive& ar, __attribute__((unused)) const unsigned int file_version)
        {
            // Insert some random rant about boost::serialze
            // (ノಠ益ಠ)ノ彡┻━┻
            helper::serialize(ar, static_cast<T*>(this)->as_tuple());
        }
    };

    template <typename T>
    inline auto as_tuple(T& t)
    {
        return t.as_tuple();
    }

    template <typename T>
    inline auto as_tuple(const T& t)
    {
        // In order to safe our users a second overload, we do this.
        // (ノಠ益ಠ)ノ彡┻━┻
        // should be safe though, as the const is moved into the tuple
        return helper::constify(const_cast<T&>(t).as_tuple());
    }
}
}

#endif // INCLUDE_NITRO_TUPLE_OPERATORS_HPP
