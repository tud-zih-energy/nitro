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

#include <nitro/except/raise.hpp>

#include <array>

namespace nitro
{
namespace lang
{

    template <typename T, size_t N>
    class fixed_vector
    {
    public:
        using iterator = typename std::array<T, N>::iterator;
        using reverse_iterator = typename std::array<T, N>::reverse_iterator;

        fixed_vector() = default;

        fixed_vector(const std::array<T, N>& array) : storage_(array)
        {
            size_ = N;
        }

        template <size_t M>
        fixed_vector(const std::array<T, M>& array)
        {
            if (M > N)
                raise("Array does not fit into fixed_vector!");

            emplace(array.begin(), array.end());
        }

        fixed_vector(const fixed_vector<T, N>&) = default;
        ~fixed_vector() = default;

        bool empty()
        {
            return size_ == 0;
        }

        const std::size_t& size()
        {
            return size_;
        }

        std::size_t capacity()
        {
            return storage_.size();
        }

        T& operator[](const std::size_t& key)
        {
            if (key >= capacity())
                raise("Key is larger than capacity");

            if (key > size())
                raise("Key is larger than size, use append() instead");

            return storage_[key];
        }

        T& at(const std::size_t& key)
        {
            if (key >= capacity())
                raise("Key is larger than capacity");

            if (key > size())
                raise("Key is larger than size, use append() instead");

            return storage_[key];
        }

        const T& operator[](const std::size_t& key) const
        {
            if (key >= capacity())
                raise("Key is larger than capacity!");

            if (key > size())
                raise("Key is larger than size, use emplace_back() instead!");

            return storage_[key];
        }

        const T& at(const std::size_t& key) const
        {
            if (key >= capacity())
                raise("Key is larger than capacity!");

            if (key > size())
                raise("Key is larger than size, use emplace_back() instead!");

            return storage_[key];
        }

        T& front() noexcept
        {
            return storage_[0];
        }

        const T& front() const noexcept
        {
            return storage_[0];
        }

        T& back() noexcept
        {
            return storage_[size_ - 1];
        }

        const T& back() const noexcept
        {
            return storage_[size_ - 1];
        }

        std::size_t emplace(const T& value)
        {
            return emplace_back(value);
        }

        template <typename iter>
        void emplace(iter start, const iter& end)
        {
            emplace_back(start, end);
        }

        std::size_t emplace_back(const T& value)
        {

            if (size_ >= capacity())
                raise("No capacity left!");

            storage_[size_] = T(value);
            ++size_;

            return size_ - 1;
        }

        template <typename iter>
        void emplace_back(iter start, const iter& end)
        {
            while (start != end)
            {
                if (size_ >= capacity())
                    raise("No capacity left!");

                storage_[size_] = T(*start);
                ++size_;

                ++start;
            }
        }

        std::size_t push_back(const T& value)
        {

            if (size_ >= capacity())
                raise("No capacity left!");

            storage_[size_] = value;
            ++size_;

            return size_ - 1;
        }

        T pop_back(const std::size_t& key)
        {

            if (size_ == 0)
                raise("Container is empty!");

            --size_;
            return storage_[size_];
        }

        fixed_vector<T, N>::iterator begin() noexcept
        {
            return iterator(storage_.begin());
        }

        fixed_vector<T, N>::reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(storage_.rbegin());
        }

        fixed_vector<T, N>::iterator end() noexcept
        {
            return iterator(storage_.end());
        }

        fixed_vector<T, N>::reverse_iterator rend() noexcept
        {
            return reverse_iterator(storage_.rend());
        }

        const fixed_vector<T, N>::iterator begin() const noexcept
        {
            return iterator(storage_.begin());
        }

        const fixed_vector<T, N>::reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(storage_.rbegin());
        }

        const fixed_vector<T, N>::iterator end() const noexcept
        {
            return iterator(storage_.end());
        }

        const fixed_vector<T, N>::reverse_iterator rend() const noexcept
        {
            return reverse_iterator(storage_.rend());
        }

        const fixed_vector<T, N>::iterator cbegin() const noexcept
        {
            return iterator(storage_.begin());
        }

        const fixed_vector<T, N>::reverse_iterator crbegin() const noexcept
        {
            return reverse_iterator(storage_.rbegin());
        }

        const fixed_vector<T, N>::iterator cend() const noexcept
        {
            return iterator(storage_.end());
        }

        const fixed_vector<T, N>::reverse_iterator crend() const noexcept
        {
            return reverse_iterator(storage_.rend());
        }

        template <size_t I>
        T& get() noexcept
        {
            return std::get<I>(storage_);
        }

        template <size_t I>
        const T& get() const noexcept
        {
            return std::get<I>(storage_);
        }

        void erase(std::size_t key)
        {
            if (key >= size())
                raise("Key does not exsist!");

            while (key + 1 < size() && key + 1 < capacity())
            {
                storage_[key] = storage_[key + 1];
                ++key;
            }
            --size_;
        }

    private:
        std::size_t size_ = 0;

        std::array<T, N> storage_;
    };

} // namespace lang
} // namespace nitro

namespace std
{
template <size_t I, typename T, size_t N>
T& get(nitro::lang::fixed_vector<T, N>& c) noexcept
{
    return c.at(I);
}
} // namespace std