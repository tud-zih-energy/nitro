/*
 * Copyright (c) 2015-2016, Technische Universität Dresden, Germany
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

#ifndef INCLUDE_NITRO_LANG_REVERSE_HPP
#define INCLUDE_NITRO_LANG_REVERSE_HPP

#include <vector>

namespace nitro
{
namespace lang
{

    namespace detail
    {
        template <typename T, typename Iterator>
        class reverse_proxy
        {
        public:
            using iterator = Iterator;

            reverse_proxy(iterator begin, iterator end) : begin_(begin), end_(end)
            {
            }

            iterator begin() const
            {
                return begin_;
            }

            iterator end() const
            {
                return end_;
            }

        private:
            iterator begin_;
            iterator end_;
        };

        template <typename T>
        class reverse
        {
        public:
            reverse(T container) : container_(std::move(container))
            {
            }

            auto begin() const
            {
                using std::rbegin;
                return rbegin(container_);
            }

            auto end() const
            {
                using std::rend;
                return rend(container_);
            }

        private:
            T container_;
        };
    }

    template <typename T>
    inline auto reverse(const T& container)
    {
        using std::rbegin;
        using std::rend;

        return detail::reverse_proxy<T, decltype(rbegin(container))>(rbegin(container),
                                                                     rend(container));
    }

    template <typename T>
    inline auto reverse(T& container)
    {
        using std::rbegin;
        using std::rend;

        return detail::reverse_proxy<T, decltype(rbegin(container))>(rbegin(container),
                                                                     rend(container));
    }

    template <typename T>
    inline auto reverse(T&& container)
    {
        return detail::reverse<T>(std::move(container));
    }

    template <typename T>
    inline auto reverse(std::initializer_list<T> l)
    {
        return reverse(std::vector<T>(l));
    }
}
} // namespace nitr::lang

#endif // INCLUDE_NITRO_LANG_REVERSE_HPP
