/*
 * Copyright (c) 2015-2017, Technische Universität Dresden, Germany
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <nitro/meta/std_hashable.hpp>

#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace nitro
{
namespace lang
{
    // this class is only a tag
    // in order to use this, implement a hash() member function
    class hashable
    {
    };

#if __cplusplus > 201402L
    template <typename... T>
    inline auto hash(const std::variant<T...>& t);
#endif

    template <typename... T>
    inline auto hash(const std::tuple<T...>& t);

    template <typename T, typename U>
    inline auto hash(const std::pair<T, U>& t);

    template <typename T>
    inline typename std::enable_if<meta::std_hashable<T>::value, std::size_t>::type
    hash(const T& t);

    template <typename T>
    inline typename std::enable_if<std::is_base_of<hashable, T>::value, std::size_t>::type
    hash(const T& t);

    template <typename T>
    inline auto hash(const std::unique_ptr<T>& p);

    template <typename T>
    inline auto hash(const std::shared_ptr<T>& p);

    namespace detail
    {
        template <typename HashT>
        inline void hash_combine_impl(HashT& seed, HashT value)
        {
            seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        template <std::size_t I, typename T>
        inline typename std::enable_if<(I == std::tuple_size<T>::value), void>::type
        hash_combine_tuple(std::size_t&, T const&)
        {
        }

        template <std::size_t I, typename T>
        inline typename std::enable_if<(I < std::tuple_size<T>::value), void>::type
        hash_combine_tuple(std::size_t& seed, const T& v)
        {
            hash_combine_impl(seed, hash(std::get<I>(v)));
            hash_combine_tuple<I + 1>(seed, v);
        }

#if __cplusplus > 201402L
        template <std::size_t I, typename T>
        inline typename std::enable_if<(I == std::variant_size<T>::value), void>::type
        hash_combine_variant(std::size_t&, const T&)
        {
        }

        template <std::size_t I, typename T>
        inline typename std::enable_if<(I < std::variant_size<T>::value), void>::type
        hash_combine_variant(std::size_t& seed, const T& v)
        {
            if (auto x = std::get_if<I>(&v))
            {
                hash_combine_impl(seed, hash(*x));
            }
            else
            {
                hash_combine_variant<I + 1>(seed, v);
            }
        }
        template <typename T, typename... U>
        inline void hash_combine_variant(std::size_t& seed, const std::variant<U...>& v)
        {
            if (auto x = std::get_if<T>(&v))
            {
                hash_combine_impl(seed, hash(*x));
            }
        }
#endif
    } // namespace detail

    template <typename... T>
    inline auto hash(const std::tuple<T...>& t)
    {
        std::size_t seed = 0;
        detail::hash_combine_tuple<0>(seed, t);
        return seed;
    }

    template <typename T, typename U>
    inline auto hash(const std::pair<T, U>& t)
    {
        std::size_t seed = hash(t.first);
        detail::hash_combine_impl(seed, hash(t.second));
        return seed;
    }

#if __cplusplus > 201402L
    template <typename... T>
    inline auto hash(const std::variant<T...>& t)
    {
        std::size_t seed = 0;

        detail::hash_combine_variant<0>(seed, t);

        return seed;
    }
#endif

    template <typename T>
    inline auto hash(const std::unique_ptr<T>& p)
    {
        return hash(*p);
    }

    template <typename T>
    inline auto hash(const std::shared_ptr<T>& p)
    {
        return hash(*p);
    }

    template <typename T>
    inline typename std::enable_if<meta::std_hashable<T>::value, std::size_t>::type hash(const T& t)
    {
        return std::hash<T>()(t);
    }

    template <typename T>
    inline typename std::enable_if<std::is_base_of<hashable, T>::value, std::size_t>::type
    hash(const T& t)
    {
        return t.hash();
    }

    template <typename T>
    struct hash_wrapper
    {
        auto operator()(const T& t) const
        {
            return hash(t);
        }
    };
} // namespace lang
} // namespace nitro
