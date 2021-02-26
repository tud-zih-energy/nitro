/*
 * Copyright (c) 2015-2021, Technische Universität Dresden, Germany
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

#include <nitro/options/option/option.hpp>

#include <nitro/env/get.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <type_traits>

namespace nitro
{
namespace options
{

    option::option(const std::string& name, const std::string& description)
    : crtp_base(name, description)
    {
    }

    option& option::default_value(const std::string& new_default)
    {
        default_ = new_default;

        return *this;
    }

    bool option::has_default() const
    {
        return static_cast<bool>(default_);
    }

    const std::string& option::get_default() const
    {
        return *default_;
    }

    option& option::optional()
    {
        is_optional_ = true;

        return *this;
    }

    bool option::is_optional() const
    {
        return is_optional_;
    }

    void option::format_value(std::ostream& s) const
    {
        s << " " << metavar();
    }

    void option::format_synopsis(std::ostream& s) const
    {
        s << "[";

        if (has_short_name())
        {
            s << "-" << short_name() << "\t<" << metavar() << "> | ";
        }

        s << format_name() << "\t<" << metavar() << ">]";
    }

    std::string option::format_default() const
    {
        if (has_default())
        {
            return nitro::format("(default: {})") % get_default();
        }

        return {};
    }

    const std::string& option::get() const
    {
        return *value_;
    }

    void option::update_value(const user_input& arg)
    {
        if (value_)
        {
            raise<parsing_error>("option was already given: ", name());
        }

        dirty_ = true;
        value_ = arg.value();
    }

    void option::prepare()
    {
    }

    void option::check()
    {
        if (!value_)
        {
            if (has_env())
            {
                auto env_value = nitro::env::get(env());

                if (!env_value.empty())
                {
                    update_value(env_value);

                    return;
                }
            }

            if (default_)
            {
                value_ = *default_;
            }
            else if (!is_optional_)
            {
                raise<parsing_error>("missing value for required option: ", name());
            }
        }
    }

} // namespace options
} // namespace nitro
