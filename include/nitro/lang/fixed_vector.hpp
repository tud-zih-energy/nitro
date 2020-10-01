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
        fixed_vector(std::size_t capacity)
        : capacity_(capacity), data_(std::make_unique<T[]>(capacity))
        {
        }

        template <typename Iterabel>
        fixed_vector(std::size_t capacity, const Iterabel& array)
        : capacity_(capacity), data_(std::make_unique<T[]>(capacity))
        {
            insert(this->begin(), array.begin(), array.end());
        }

        fixed_vector(const fixed_vector<T>& v) : fixed_vector(v.capacity_, v)
        {
        }

        fixed_vector(fixed_vector<T>&& v) : capacity_(v.capacity_), data_(std::move(v.data_))
        {
        }

        ~fixed_vector() = default;

        bool empty() const
        {
            return size_ == 0;
        }

        std::size_t size() const
        {
            return size_;
        }

        std::size_t capacity() const
        {
            return capacity_;
        }

        T& operator[](const std::size_t& key)
        {
            return data_[key];
        }

        T& at(const std::size_t& key)
        {
            if (key >= capacity_)
                raise("Key is larger than capacity");

            if (key > size_)
                raise("Key is larger than size, use append() instead");

            return data_[key];
        }

        const T& operator[](const std::size_t& key) const
        {
            return data_[key];
        }

        const T& at(const std::size_t& key) const
        {
            if (key >= capacity_)
                raise("Key is larger than capacity!");

            if (key > size_)
                raise("Key is larger than size, use emplace_back() instead!");

            return data_[key];
        }

        T& front() noexcept
        {
            return data_[0];
        }

        const T& front() const noexcept
        {
            return data_[0];
        }

        T& back() noexcept
        {
            return data_[size_ - 1];
        }

        const T& back() const noexcept
        {
            return data_[size_ - 1];
        }

        template <class... Args>
        void emplace(T* const pos, Args&&... args)
        {
            auto key = std::distance(begin(), pos);

            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[key], T(args...));
            ++size_;
        }

        template <class... Args>
        std::size_t emplace_back(Args&&... args)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[size_], T(std::forward<Args>(args)...));
            ++size_;

            return size_ - 1;
        }

        std::size_t insert(const T& value)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[size_], value);
            ++size_;

            return size_ - 1;
        }

        std::size_t insert(T&& value)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            replace(data_[size_], std::move(value));
            ++size_;

            return size_ - 1;
        }

        template <typename Iter>
        void insert(T* const pos, Iter start, Iter end)
        {
            std::size_t key = std::distance(begin(), pos);
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

        std::size_t push_back(const T& value)
        {
            if (size_ >= capacity_)
                raise("No capacity left!");

            data_[size_] = value;
            ++size_;

            return size_ - 1;
        }

        template <typename Iter>
        void push_back(Iter start, Iter end)
        {
            insert(this->end(), start, end);
        }

        void pop_back()
        {
            if (size_ == 0)
                raise("Container is empty!");

            --size_;
        }

        T* begin() noexcept
        {
            return &data_[0];
        }

        T* rbegin() noexcept
        {
            return &data_[size_ - 1];
        }

        T* end() noexcept
        {
            return &data_[size_];
        }

        T* rend() noexcept
        {
            return &data_[-1];
        }

        const T* begin() const noexcept
        {
            return &data_[0];
        }

        const T* rbegin() const noexcept
        {
            return &data_[size_ - 1];
        }

        const T* end() const noexcept
        {
            return &data_[size_];
        }

        const T* rend() const noexcept
        {
            return &data_[-1];
        }

        const T* cbegin() const noexcept
        {
            return &data_[0];
        }

        const T* crbegin() const noexcept
        {
            return &data_[size_ - 1];
        }

        const T* cend() const noexcept
        {
            return &data_[size_];
        }

        const T* crend() const noexcept
        {
            return &data_[-1];
        }

        void erase(T* pos)
        {
            std::size_t key = std::distance(begin(), pos);

            if (key >= size_)
                raise("Key does not exsist!");

            while (key + 1 < size_ && key + 1 < capacity_)
            {
                replace(data_[key], std::forward<T>(data_[key + 1]));
                ++key;
            }
            --size_;
        }

        T* data() noexcept
        {
            return data_.get();
        }

        const T* data() const noexcept
        {
            return data_.get();
        }

    private:
        std::size_t size_ = 0;
        std::size_t capacity_ = 0;

        std::unique_ptr<T[]> data_;

        template <typename A>
        std::enable_if_t<std::is_move_assignable<A>::value> replace(A& a, A&& b)
        {
            a = std::move(b);
        }

        template <typename A>
        std::enable_if_t<!std::is_move_assignable<A>::value> replace(A& a, const A& b)
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
