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

#include <exception>
#include <type_traits>

namespace nitro
{
namespace lang
{
    namespace detail
    {
        template <typename Er = std::exception, typename... E>
        class catch_wrapper
        {
        public:
            template <typename F, typename... Args>
            auto operator()(F& f, Args... args)
            {
                try
                {
                    return catch_wrapper<E...>()(f, args...);
                }
                catch (Er& e)
                {
                    using T = decltype(f(args...));
                    return T();
                }
            }
        };

        template <typename E>
        class catch_wrapper<E>
        {
        public:
            template <typename F, typename... Args>
            auto operator()(F& f, Args... args) -> decltype(f(args...))
            {

                try
                {
                    return f(args...);
                }
                catch (E&)
                {
                    using T = decltype(f(args...));

                    return T();
                }
            }
        };
    } // namespace detail

    template <typename... E, typename F, typename... Args>
    auto make_catch(F& f, Args... args)
    {
        using T = decltype(f(args...));

        static_assert(!std::is_reference<T>::value, "the callable must not return a reference.");

        static_assert((std::is_nothrow_default_constructible<T>::value &&
                       std::is_nothrow_copy_constructible<T>::value) ||
                          std::is_same<T, void>::value,
                      "The return value type must not throw on returning a default constructed "
                      "value, or be void.");
        return detail::catch_wrapper<E...>()(f, args...);
    }
} // namespace lang
} // namespace nitro
