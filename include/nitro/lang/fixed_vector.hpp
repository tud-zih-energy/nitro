/*
 * Copyright (c) 2015-2020, Technische Universität Dresden, Germany
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

#include <nitro/except/raise.hpp>

#include <array>
#include <type_traits>

namespace nitro
{
namespace lang
{
    template <typename T>
    class fixed_vector
    {
    public:
        using value_type = T;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr fixed_vector(size_type capacity)
        : capacity_(capacity), data_(std::make_unique<value_type[]>(capacity))
        {
        }

        template <typename Iterabel>
        constexpr fixed_vector(size_type capacity, const Iterabel& array)
        : capacity_(capacity), data_(std::make_unique<value_type[]>(capacity))
        {
            insert(this->begin(), array.begin(), array.end());
        }

        constexpr fixed_vector(const std::initializer_list<value_type>& list)
        : capacity_(list.size()), data_(std::make_unique<value_type[]>(list.size()))
        {
            insert(this->begin(), list.begin(), list.end());
        }

        constexpr fixed_vector(const fixed_vector<value_type>& v) : fixed_vector(v.capacity_, v)
        {
        }

        constexpr fixed_vector(fixed_vector<value_type>&& v)
        : capacity_(v.capacity_), data_(std::move(v.data_))
        {
        }

        constexpr fixed_vector operator=(const fixed_vector& v)
        {
            return fixed_vector(v);
        }

        constexpr fixed_vector operator=(fixed_vector&& v)
        {
            return fixed_vector(std::move(v));
        }

        constexpr fixed_vector operator=(const std::initializer_list<value_type>& l)
        {
            return fixed_vector(l.size(), l);
        }

        ~fixed_vector() = default;

        constexpr bool empty() const
        {
            return size_ == 0;
        }

        constexpr size_type size() const
        {
            return size_;
        }

        constexpr size_type capacity() const
        {
            return capacity_;
        }

        constexpr reference operator[](const size_type& key)
        {
            return data_[key];
        }

        constexpr reference at(const size_type& key)
        {
            if (key >= capacity_)
                raise("Key is larger than capacity");

            if (key > size_)
                raise("Key is larger than size, use append() instead");

            return data_[key];
        }

        constexpr const_reference operator[](const size_type& key) const
        {
            return data_[key];
        }

        constexpr const_reference at(const size_type& key) const
        {
            if (key >= capacity_)
                raise("Key is larger than capacity!");

            if (key > size_)
                raise("Key is larger than size, use emplace_back() instead!");

            return data_[key];
        }

        constexpr reference front() noexcept
        {
            return data_[0];
        }

        constexpr const_reference front() const noexcept
        {
            return data_[0];
        }

        constexpr reference back() noexcept
        {
            return data_[size_ - 1];
        }

        constexpr const_reference back() const noexcept
        {
            return data_[size_ - 1];
        }

        template <class... Args>
        constexpr void emplace(pointer const pos, Args&&... args)
        {
            auto key = std::distance(begin(), pos);

            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[key], value_type(args...));
            ++size_;
        }

        template <class... Args>
        constexpr size_type emplace_back(Args&&... args)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[size_], value_type(std::forward<Args>(args)...));
            ++size_;

            return size_ - 1;
        }

        constexpr size_type insert(const_reference value)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[size_], value);
            ++size_;

            return size_ - 1;
        }

        constexpr size_type insert(reference& value)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[size_], std::move(value));
            ++size_;

            return size_ - 1;
        }

        template <typename Iter>
        constexpr void insert(iterator const pos, Iter start, Iter end)
        {
            size_type key = std::distance(begin(), pos);
            if (key > size_)
                raise("Key larger than size!");

            while (start != end)
            {
                if (key >= capacity_)
                    raise("No capacity left!");

                data_[key] = *start;

                if (key == size_)
                    ++size_;

                ++key;
                ++start;
            }
        }

        constexpr void insert(iterator const pos, std::initializer_list<value_type>& list)
        {
            insert(pos, list.begin(), list.end());
        }

        constexpr size_type push_back(const_reference value)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            data_[size_] = value;
            ++size_;

            return size_ - 1;
        }

        template <typename Iter>
        constexpr void push_back(Iter start, Iter end)
        {
            insert(this->end(), start, end);
        }

        constexpr void pop_back()
        {
            if (size_ == 0)
                raise("Container is empty!");

            --size_;
        }

        constexpr iterator begin() noexcept
        {
            return &data_[0];
        }

        constexpr iterator rbegin() noexcept
        {
            return &data_[size_ - 1];
        }

        constexpr iterator end() noexcept
        {
            return &data_[size_];
        }

        constexpr iterator rend() noexcept
        {
            return &data_[-1];
        }

        constexpr const iterator begin() const noexcept
        {
            return &data_[0];
        }

        constexpr const iterator rbegin() const noexcept
        {
            return &data_[size_ - 1];
        }

        constexpr const iterator end() const noexcept
        {
            return &data_[size_];
        }

        constexpr const iterator rend() const noexcept
        {
            return &data_[-1];
        }

        constexpr const iterator cbegin() const noexcept
        {
            return &data_[0];
        }

        constexpr const iterator crbegin() const noexcept
        {
            return &data_[size_ - 1];
        }

        constexpr const iterator cend() const noexcept
        {
            return &data_[size_];
        }

        constexpr const iterator crend() const noexcept
        {
            return &data_[-1];
        }

        constexpr void erase(iterator pos)
        {
            size_type key = std::distance(begin(), pos);

            if (key >= size_)
                raise("Key does not exsist!");

            while (key + 1 < size_ && key + 1 < capacity_)
            {
                replace(data_[key], std::forward<value_type>(data_[key + 1]));
                ++key;
            }
            --size_;
        }

        constexpr pointer data() noexcept
        {
            return data_.get();
        }

        constexpr const_pointer data() const noexcept
        {
            return data_.get();
        }

    private:
        size_type size_ = 0;
        size_type capacity_ = 0;

        std::unique_ptr<value_type[]> data_;

        template <typename A>
        constexpr std::enable_if_t<std::is_move_assignable<A>::value> replace(A& a, A&& b)
        {
            a = std::move(b);
        }

        template <typename A>
        constexpr std::enable_if_t<!std::is_move_assignable<A>::value> replace(A& a, const A& b)
        {
            a = b;
        }
    };

} // namespace lang
} // namespace nitro

namespace std
{
template <size_t I, typename T>
T& get(nitro::lang::fixed_vector<T>& c) noexcept
{
    return c.at(I);
}
} // namespace std
