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

#include <sstream>
#include <string>
#include <vector>

namespace nitro
{
namespace lang
{
    template <typename InputIterator>
    std::string join(InputIterator begin, InputIterator end,
                     const std::string& infix = std::string(" "))
    {
        if (begin == end)
            return {};

        std::stringstream s;

        auto it = begin;

        for (; it + 1 != end; ++it)
        {
            s << *it << infix;
        }

        s << *it;

        return s.str();
    }

    inline std::string join(const std::vector<std::string>& strs,
                            const std::string& infix = std::string(" "))
    {
        return join(strs.begin(), strs.end(), infix);
    }

    inline std::vector<std::string> split(const std::string& haystack, const std::string& needle)
    {
        if (needle.empty())
        {
            raise("needle must not be empty");
        }

        std::vector<std::string> result;

        std::string::size_type start = 0;

        while (true)
        {
            auto pos = haystack.find(needle, start);
            if (pos != std::string::npos)
            {
                if (start < pos)
                {
                    result.emplace_back(haystack.substr(start, pos - start));
                }
                else
                {
                    result.emplace_back(std::string());
                }

                start = pos + needle.size();
            }
            else
            {
                result.emplace_back(haystack.substr(start));
                break;
            }
        }

        return result;
    }

    inline bool starts_with(const std::string& full, const std::string& beginning)
    {
        return full.find(beginning) == 0;
    }

    inline void replace_all(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
} // namespace lang
} // namespace nitro
