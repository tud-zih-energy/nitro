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

#ifndef INCLUDE_NITRO_LANG_OPTIONAL_HPP
#define INCLUDE_NITRO_LANG_OPTIONAL_HPP

#include <nitro/except/raise.hpp>

#include <memory>
#include <utility>

namespace nitro
{
namespace lang
{

    template <typename T>
    class optional
    {
    public:
        optional() = default;

        optional(const optional& other)
        {
            if (other)
            {
                data_ = std::make_unique<T>(*other);
            }
        }

        optional(const T& data) : data_(std::make_unique<T>(data))
        {
        }

        optional(T&& data) : data_(std::make_unique<T>(std::move(data)))
        {
        }

        optional& operator=(const optional& other)
        {
            if (other)
            {
                data_ = std::make_unique<T>(*other);
            }

            return *this;
        }

        optional& operator=(const T& data)
        {
            data_ = std::make_unique<T>(data);

            return *this;
        }

        optional& operator=(T&& data)
        {
            data_ = std::make_unique<T>(std::move(data));

            return *this;
        }

    public:
        explicit operator bool() const
        {
            return static_cast<bool>(data_);
        }

        const T& operator*() const
        {
            if (data_)
            {
                return *data_;
            }

            raise("No value set");
        }

    private:
        std::unique_ptr<T> data_;
    };
} // namespace lang
} // namespace nitro

#endif // INCLUDE_NITRO_LANG_OPTIONAL_HPP
