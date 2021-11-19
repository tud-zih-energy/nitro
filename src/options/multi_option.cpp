/*
 * Copyright (c) 2015-2016, Technische Universit�t Dresden, Germany
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

#include <nitro/options/option/multi_option.hpp>

#include <nitro/options/exception.hpp>

#include <nitro/env/get.hpp>
#include <nitro/except/raise.hpp>
#include <nitro/format.hpp>
#include <nitro/lang/string.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace nitro
{
namespace options
{

    multi_option::multi_option(const std::string& name, const std::string& description)
    : crtp_base(name, description)
    {
    }

    multi_option& multi_option::default_value(const std::vector<std::string>& new_default)
    {
        default_ = std::make_unique<std::vector<std::string>>(new_default);

        return *this;
    }

    bool multi_option::has_default() const
    {
        return static_cast<bool>(default_);
    }

    const std::vector<std::string>& multi_option::get_default() const
    {
        return *default_;
    }

    multi_option& multi_option::optional()
    {
        is_optional_ = true;

        return *this;
    }

    bool multi_option::is_optional() const
    {
        return is_optional_;
    }

    void multi_option::format_value(std::ostream& s) const
    {
        s << " " << metavar();
    }

    void multi_option::format_synopsis(std::ostream& s) const
    {
        s << "[";

        if (has_short_name())
        {
            s << "-" << short_name() << "\t<" << metavar() << "> | ";
        }

        s << format_name() << "\t<" << metavar() << ">]";
    }

    std::string multi_option::format_default() const
    {
        if (has_default())
        {
            return nitro::format("(default: {})") %
                   nitro::lang::join(get_default().begin(), get_default().end(), ", ");
        }

        return {};
    }

    const std::string& multi_option::get(std::size_t i) const
    {
        return value_[i];
    }

    const std::vector<std::string>& multi_option::get_all() const
    {
        return value_;
    }

    std::size_t multi_option::count() const
    {
        return value_.size();
    }

    void multi_option::update_value(const user_input& arg)
    {
        dirty_ = true;

        value_.push_back(arg.value());
    }

    void multi_option::prepare()
    {
    }

    void multi_option::check()
    {
        if (value_.empty())
        {
            if (has_env())
            {
                auto env_value = nitro::env::get(env());

                if (!env_value.empty())
                {
                    std::string element;
                    std::stringstream str;
                    str << env_value;

                    while (std::getline(str, element, ';'))
                    {
                        update_value(element);
                    }

                    return;
                }
            }

            if (default_)
            {
                value_ = *default_;

                return;
            }

            if (!is_optional_)
            {
                raise<parsing_error>("missing value for required option: ", name());
            }
        }
    }

} // namespace options
} // namespace nitro
