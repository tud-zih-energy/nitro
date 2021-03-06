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

#ifndef INCLUDE_NITRO_LANG_STRING_REF_HPP
#define INCLUDE_NITRO_LANG_STRING_REF_HPP

#include <nitro/except/raise.hpp>

#include <string>

#include <cstring>

namespace nitro
{
namespace lang
{

    class string_ref
    {

        using pointer_type = const char*;

    public:
        using iterator = const char*;

        string_ref(const std::string& str) : ptr_(str.c_str())
        {
        }

        string_ref(const char* const pstr) : ptr_(pstr)
        {
        }

    public:
        char at(std::size_t i) const
        {
            if (i >= size())
            {
                raise("Index to large for given string");
            }

            return (*this)[i];
        }

        char operator[](std::size_t i) const noexcept
        {
            return ptr_[i];
        }

        std::size_t size() const noexcept
        {
            return std::strlen(const_cast<pointer_type>(ptr_));
        }

        bool empty() const
        {
            return ptr_ == nullptr ? true : ptr_[0] == '\0';
        }

        iterator begin() const noexcept
        {
            return ptr_;
        }

        iterator end() const noexcept
        {
            return ptr_ + size();
        }

        std::string str() const
        {
            return { ptr_ };
        }

        pointer_type get() const
        {
            return ptr_;
        }

    public:
        operator std::string() const
        {
            return str();
        }

        explicit operator bool() const
        {
            return !empty();
        }

        friend inline std::ostream& operator<<(std::ostream& s, const string_ref& str);
        friend inline bool operator==(const string_ref& a, const string_ref& b);
        friend inline bool operator!=(const string_ref& a, const string_ref& b);

    private:
        pointer_type ptr_;
    };

    inline bool operator==(const string_ref& a, const string_ref& b)
    {
        if (a.ptr_ == nullptr || b.ptr_ == nullptr)
        {
            return false;
        }
        return std::strcmp(a.ptr_, b.ptr_) == 0;
    }

    inline bool operator!=(const string_ref& a, const string_ref& b)
    {
        return !(a == b);
    }

    inline std::ostream& operator<<(std::ostream& s, const string_ref& str)
    {
        return s << str.ptr_;
    }
} // namespace lang
} // namespace nitro

#endif // INCLUDE_NITRO_LANG_STRING_REF_HPP
