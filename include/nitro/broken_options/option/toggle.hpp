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

#ifndef INCLUDE_NITRO_BROKEN_OPTIONS_TOGGLE_HPP
#define INCLUDE_NITRO_BROKEN_OPTIONS_TOGGLE_HPP

#include <nitro/broken_options/exception.hpp>
#include <nitro/broken_options/fwd.hpp>
#include <nitro/broken_options/option/base.hpp>

#include <nitro/lang/optional.hpp>

#include <ios>
#include <memory>
#include <sstream>
#include <string>

namespace nitro
{
namespace broken_options
{
    class toggle : public crtp_base<toggle>
    {
    public:
        toggle(const std::string& name, const std::string& description)
        : crtp_base(name, description), ref_(nullptr), given_(0)
        {
        }

    public:
        toggle& ref(int& target)
        {
            ref_ = &target;

            return *this;
        }

    public:
        int given() const
        {
            return given_;
        }

        int default_value(const bool& def)
        {
            if(def)
            {
                return given_ = 1;
            }
            else
            {
                return given_ = 0;
            }
        }

    public:
        virtual void format_value(std::ostream&) const override
        {
        }

        static bool parse_env_value(const std::string& env_value)
        {
            if (env_value == "TRUE" || env_value == "ON" || env_value == "YES" ||
                env_value == "true" || env_value == "on" || env_value == "yes" ||
                env_value == "1" || env_value == "Y" || env_value == "with" ||
                env_value == "True" || env_value == "On" || env_value == "WITH" ||
                env_value == "With" || env_value == "y" || env_value == "Yes")
            {
                return true;
            }
            if (env_value == "false" || env_value == "FALSE" ||env_value == "without" ||
                env_value == "0" || env_value == "NO" || env_value == "no" ||
                env_value == "Without" || env_value == "n" || env_value == "off" ||
                env_value == "OFF" || env_value == "N" || env_value == "False" ||
                env_value == "Off" || env_value == "WITHOUT" || env_value == "No")
            {
                return false;
            }
            raise<parsing_error>("Can't parse env variable");
        }

    private:
        virtual void update_value(const argument& arg) override
        {
            if (arg.has_value())
            {
                raise<parsing_error>("a toggle cannot be given a value: ", name());
            }

            if (ref_ != nullptr)
            {
                *ref_ = true;
            }

            if (arg.has_prefix())
            {
                given_ = 0;
            }
            else
            {
                ++given_;
            }
            //++given_;
        }

        virtual void prepare() override
        {
        }

        virtual void check() override
        {
            if (has_env() && !given())
            {
                auto env_value = nitro::env::get(env());

                if (!env_value.empty())
                {
                    // Python code would look like this:
                    // if bool(env_value):
                    // FeelsBadMan
                    if (parse_env_value(env_value))
                    {
                        update_value({ "--" + name() });
                    }
                    else
                    {
                        update_value({ "--no-" + name()});
                    }
                    return;
                }
            }
        }
        bool matches(const argument& arg) override
        {
            if (!arg.is_argument())
            {
                return false;
            }

            if (has_short_name() && arg.is_short())
            {
                auto list = arg.as_short_list();

                if (list.size() > 1 && arg.has_value())
                {
                    return false;
                }

                return list.count(short_name());
            }
            else if (arg.is_named())
            {
                if (arg.has_prefix())
                {
                    return arg.name_without_prefix() == name();
                }
                else
                {
                    return arg.as_named() == name();
                }
            }

            return false;
        }
        
        friend class parser;

    private:
        std::string name_;
        std::string description_;
        nitro::lang::optional<std::string> short_;
        int* ref_;
        int given_;
    };
} // namespace broken_options
} // namespace nitro

#endif // INCLUDE_NITRO_BROKEN_OPTIONS_TOGGLE_HPP
