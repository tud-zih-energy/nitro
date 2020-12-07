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

#pragma once

#include <nitro/better_options/exception.hpp>
#include <nitro/better_options/fwd.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/lang/optional.hpp>
#include <nitro/lang/string.hpp>

#include <regex>
#include <set>
#include <string>

namespace nitro
{
namespace better_options
{
    class user_input
    {
    public:
        user_input(const std::string& arg) : arg_(arg)
        {
            auto sep = arg_.find("=");
            if (sep != std::string::npos)
            {
                name_ = arg_.substr(0, sep);
                value_ = arg_.substr(sep + 1);
            }
            else
            {
                name_ = arg_;
            }

            if (!is_value() && !is_double_dash())
            {
                if (!std::regex_match(arg, std::regex("-{1,2}[^-=]+[^=]*=?.*")))
                {
                    raise<parsing_error>("The user input couldn't be parsed. (", arg, ")");
                }
            }
        }

    public:
        bool is_value() const noexcept
        {
            return name_[0] != '-';
        }

        bool is_double_dash() const noexcept
        {
            return arg_ == "--";
        }

        bool is_short() const noexcept
        {
            return name_.size() > 1 && name_[0] == '-' && name_[1] != '-';
        }

        bool is_named() const noexcept
        {
            return name_.size() > 2 && name_[0] == '-' && name_[1] == '-' && name_[2] != '-';
        }

        bool is_argument() const noexcept
        {
            return is_short() || is_named();
        }

        bool has_value() const noexcept
        {
            return is_value() || static_cast<bool>(value_);
        }

        bool has_prefix() const
        {
            return nitro::lang::starts_with(name_, "--no-");
        }

    public:
        const std::string& data() const noexcept
        {
            return arg_;
        }

        std::string name_without_prefix() const
        {
            if (!has_prefix())
            {
                raise<parser_error>(
                    "Trying to get the name_ without prefix but prefix is not given.");
            }
            return name().substr(5);
        }

        const std::string& name() const
        {
            if (is_value())
            {
                raise<parser_error>("Trying to get the name_ of a pure value argument.");
            }

            return name_;
        }

        const std::string& value() const
        {
            if (!has_value())
            {
                raise<parser_error>(
                    "Trying to read the value of an argument that does not have a value.");
            }

            if (is_value())
            {
                return arg_;
            }

            return *value_;
        }

        std::multiset<std::string> as_short_list() const
        {
            if (!is_short())
            {
                raise<parser_error>(
                    "Trying to interpret argument as short arguments list, but it ain't.");
            }

            std::multiset<std::string> result;

            for (std::size_t i = 1; i < name_.size(); i++)
            {
                result.emplace(1, arg_[i]);
            }

            return result;
        }

        std::string as_named() const
        {
            if (!is_named())
            {
                raise<parser_error>("Trying to interpret argument as named option, but it ain't.");
            }

            return { name_.begin() + 2, name_.end() };
        }

    private:
        std::string arg_;
        std::string name_;
        lang::optional<std::string> value_;
    };
} // namespace better_options
} // namespace nitro
