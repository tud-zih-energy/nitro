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
        using iterator = typename std::vector<T>::iterator;

        using reverse_iterator = typename std::vector<T>::reverse_iterator;

        fixed_vector(std::size_t capacity)
        : capacity_(capacity), data_(std::make_unique<T[]>(capacity))
        {
        }

        template <typename Iterabel>
        fixed_vector(std::size_t capacity, const Iterabel& array)
        : capacity_(capacity), data_(std::make_unique<T[]>(capacity))
        {
            insert(array.begin(), array.end());
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
        void emplace(iterator pos, Args&&... args)
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
        void insert(Iter pos, Iter start, Iter end)
        {
            auto key = std::distance(begin(), pos);
            if (key > size_)
                raise("Key larger than size!");

            while (start != end)
            {
                if (key >= capacity_)
                    raise("No capacity left!");

                data_[key] = *start;

                if (key == size_)
                    ++size_;

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

        void pop_back()
        {
            if (size_ == 0)
                raise("Container is empty!");

            --size_;
        }

        typename fixed_vector<T>::iterator begin() noexcept
        {
            return iterator(&data_[0]);
        }

        typename fixed_vector<T>::reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(&data_[size_ - 1]);
        }

        typename fixed_vector<T>::iterator end() noexcept
        {
            return iterator(&data_[size_]);
        }

        typename fixed_vector<T>::reverse_iterator rend() noexcept
        {
            return reverse_iterator(&data_[-1]);
        }

        const typename fixed_vector<T>::iterator begin() const noexcept
        {
            return iterator(&data_[0]);
        }

        const typename fixed_vector<T>::reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(&data_[size_ - 1]);
        }

        const typename fixed_vector<T>::iterator end() const noexcept
        {
            return iterator(&data_[size_]);
        }

        const typename fixed_vector<T>::reverse_iterator rend() const noexcept
        {
            return reverse_iterator(&data_[-1]);
        }

        const typename fixed_vector<T>::iterator cbegin() const noexcept
        {
            return iterator(&data_[0]);
        }

        const typename fixed_vector<T>::reverse_iterator crbegin() const noexcept
        {
            return reverse_iterator(&data_[size_ - 1]);
        }

        const typename fixed_vector<T>::iterator cend() const noexcept
        {
            return iterator(&data_[size_]);
        }

        const typename fixed_vector<T>::reverse_iterator crend() const noexcept
        {
            return reverse_iterator(&data_[-1]);
        }

        void erase(std::size_t key)
        {
            if (key >= size_)
                raise("Key does not exsist!");

            while (key + 1 < size_ && key + 1 < capacity_)
            {
                replace(data_[key], std::forward<T>(data_[key + 1]));
                ++key;
            }
            --size_;
        }

        void erase(iterator pos)
        {
            auto key = std::distance(begin(), pos);

            if (key >= size_)
                raise("Key does not exsist!");

            while (key + 1 < size_ && key + 1 < capacity_)
            {
                replace(data_[key], data_[key + 1]);
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
