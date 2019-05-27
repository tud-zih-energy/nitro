/*
 * Copyright (c) 2015-2019, Technische Universität Dresden, Germany
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

#include <vector>

namespace nitro
{
namespace lang
{

    namespace detail
    {
        template <typename T>
        class enumerate;

        template <typename Iterator>
        class enumerate_proxy
        {
            template <typename T>
            friend class enumerate;

            template <typename U>
            struct proxy
            {
                proxy(std::size_t index, U value) : index_(index), value_(value)
                {
                }

                std::size_t index()
                {
                    return index_;
                }

                std::size_t index() const
                {
                    return index_;
                }

                U value()
                {
                    return value_;
                }

                const U value() const
                {
                    return value_;
                }

            private:
                std::size_t index_;
                U value_;
            };

            struct iterator
            {
                using value_type = decltype(*std::declval<Iterator>());

                iterator(Iterator it, std::size_t index) : it_(it), index_(index)
                {
                }

                auto operator*()
                {
                    return proxy<value_type>(index_, *it_);
                }

                auto operator*() const
                {
                    return proxy<const value_type&>(index_, *it_);
                }

                iterator& operator++()
                {
                    ++it_;
                    ++index_;

                    return *this;
                }

                iterator operator++(int)
                {

                    iterator orig = *this;
                    ++(*this);
                    return orig;
                }

                bool operator!=(iterator other)
                {
                    return it_ != other.it_;
                }

            private:
                Iterator it_;
                std::size_t index_;
            };

        public:
            enumerate_proxy(Iterator begin, Iterator end) : begin_(begin), end_(end)
            {
            }

            iterator begin()
            {
                return { begin_, 0 };
            }

            iterator end()
            {
                return { end_, 0 };
            }

        private:
            Iterator begin_;
            Iterator end_;
        };

        template <typename T>
        class enumerate
        {
        public:
            enumerate(T container) : container_(std::move(container))
            {
            }

            auto begin() const
            {
                return typename enumerate_proxy<decltype(container_.begin())>::iterator(
                    container_.begin(), 0);
            }

            auto end() const
            {
                return typename enumerate_proxy<decltype(container_.begin())>::iterator(
                    container_.end(), 0);
            }

        private:
            T container_;
        };
    } // namespace detail

    template <typename T>
    inline auto enumerate(const T& container)
    {
        using std::begin;
        using std::end;

        return detail::enumerate_proxy<decltype(begin(container))>(begin(container),
                                                                   end(container));
    }

    template <typename T>
    inline auto enumerate(T& container)
    {
        using std::begin;
        using std::end;

        return detail::enumerate_proxy<decltype(begin(container))>(begin(container),
                                                                   end(container));
    }

    template <typename T>
    inline auto enumerate(T&& container)
    {
        return detail::enumerate<T>(std::move(container));
    }

    template <typename T>
    inline auto enumerate(std::initializer_list<T>&& container)
    {
        return enumerate(std::vector<T>(std::move(container)));
    }
} // namespace lang
} // namespace nitro
