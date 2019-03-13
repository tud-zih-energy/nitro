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

#ifndef INCLUDE_NITRO_FORMAT_FORMAT_HPP
#define INCLUDE_NITRO_FORMAT_FORMAT_HPP

#include <nitro/except/raise.hpp>

#include <cstdio>
#include <regex>
#include <sstream>
#include <string>

namespace nitro
{

namespace detail
{
    template <class Char, class Traits = std::char_traits<Char>>
    class formatter
    {
        using self = formatter;

    public:
        using string_type = std::basic_string<Char, Traits>;
        using stream_type = std::basic_stringstream<Char, Traits>;

        formatter(const string_type& format) : format_(format)
        {
        }

        template <typename T>
        self& operator%(T&& arg)
        {
            stream_type str;
            str << std::forward<T>(arg);

            args_.emplace_back(str.str());

            return *this;
        }

        template <typename Arg, typename... Args>
        self& args(Arg&& arg, Args&&... args)
        {
            (*this) % std::forward<Arg>(arg);
            this->args(std::forward<Args>(args)...);
            return *this;
        }

        self& args()
        {
            return *this;
        }

        string_type str() const
        {
            string_type result;

            std::regex r("\\{\\}");

            auto input = format_.begin();
            auto placeholders_begin = std::sregex_iterator(format_.begin(), format_.end(), r);
            auto placeholders_end = std::sregex_iterator();
            auto placeholder = placeholders_begin;

            for (auto it = args_.begin(); it != args_.end(); ++it, ++placeholder)
            {
                if (placeholder == placeholders_end)
                {
                    raise("Provided more arguments than placeholders available in format string");
                }

                result.append(input, format_.begin() + placeholder->position());
                input = format_.begin() + placeholder->position() + placeholder->length();
                result.append(it->begin(), it->end());
            }

            result.append(input, format_.end());

            if (placeholder != placeholders_end)
            {
                raise("Provided less arguments than placeholders needed in format string");
            }

            return result;
        }

        operator string_type() const
        {
            return str();
        }

    private:
        string_type format_;
        std::vector<string_type> args_;
    };

    template <class Char, class Traits = std::char_traits<Char>>
    std::ostream& operator<<(std::ostream& s, const formatter<Char, Traits>& f)
    {
        return s << f.str();
    }
} // namespace detail

template <class Char, class Traits>
inline auto format(const std::basic_string<Char, Traits>& format_str)
    -> detail::formatter<Char, Traits>
{
    return detail::formatter<Char, Traits>(format_str);
}

template <class Char>
inline auto format(const Char* format_str) -> detail::formatter<Char>
{
    return detail::formatter<Char>(format_str);
}

} // namespace nitro

inline nitro::detail::formatter<char> operator""_nf(const char* format_str, std::size_t)
{
    return nitro::detail::formatter<char>(format_str);
}

inline nitro::detail::formatter<char16_t> operator""_nf(const char16_t* format_str, std::size_t)
{
    return nitro::detail::formatter<char16_t>(format_str);
}

inline nitro::detail::formatter<char32_t> operator""_nf(const char32_t* format_str, std::size_t)
{
    return nitro::detail::formatter<char32_t>(format_str);
}

inline nitro::detail::formatter<wchar_t> operator""_nf(const wchar_t* format_str, std::size_t)
{
    return nitro::detail::formatter<wchar_t>(format_str);
}

#endif // INCLUDE_NITRO_FORMAT_FORMAT_HPP
