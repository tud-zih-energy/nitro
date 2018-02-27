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

#ifndef INCLUDE_NITRO_BROKEN_OPTIONS_ARGUMENT_HPP
#define INCLUDE_NITRO_BROKEN_OPTIONS_ARGUMENT_HPP

#include <nitro/broken_options/exception.hpp>
#include <nitro/broken_options/fwd.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/lang/optional.hpp>

#include <string>
#include <vector>

namespace nitro
{
namespace broken_options
{
    class argument
    {
    public:
        argument(const std::string& arg) : arg_(arg)
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
        }

    public:
        bool is_value() const
        {
            return name_[0] != '-';
        }

        bool is_double_dash() const
        {
            return arg_.size() == 2 && arg_[0] == '-' && arg_[1] == '-';
        }

        bool is_short_option() const
        {
            return name_.size() > 1 && name_[0] == '-' && name_[1] != '-';
        }

        bool is_named_option() const
        {
            return name_.size() > 2 && name_[0] == '-' && name_[1] == '-' && name_[2] != '-';
        }

        bool is_option() const
        {
            return is_short_option() || is_named_option();
        }

    public:
        const std::string& data() const
        {
            return arg_;
        }

        const std::string& name() const
        {
            return name_;
        }

        bool has_value() const
        {
            return static_cast<bool>(value_);
        }

        const std::string& value() const
        {
            return *value_;
        }

        std::vector<std::string> short_options() const
        {
            if (!is_short_option() || has_value())
            {
                raise<parser_error>("Trying to interprete option as short options list, but ain't");
            }

            std::vector<std::string> result;

            for (std::size_t i = 1; i < arg_.size(); i++)
            {
                result.emplace_back(1, arg_[i]);
            }

            return result;
        }

    private:
        std::string arg_;
        std::string name_;
        lang::optional<std::string> value_;
    };
} // namespace broken_options
} // namespace nitro

#endif // INCLUDE_NITRO_BROKEN_OPTIONS_ARGUMENT_HPP
