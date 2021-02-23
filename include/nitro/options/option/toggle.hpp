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

#include <nitro/options/exception.hpp>
#include <nitro/options/fwd.hpp>
#include <nitro/options/option/base.hpp>

#include <nitro/env/get.hpp>
#include <nitro/lang/optional.hpp>

#include <ios>
#include <memory>
#include <sstream>
#include <string>

namespace nitro
{
namespace options
{
    class toggle : public crtp_base<toggle>
    {
    public:
        toggle(const std::string& name, const std::string& description)
        : crtp_base(name, description), given_(0)
        {
        }

    public:
        int given() const
        {
            return given_;
        }

        toggle& default_value(bool def)
        {
            default_ = def ? 1 : 0;

            return *this;
        }

        toggle& default_value(int def)
        {
            default_ = def;

            return *this;
        }

        toggle& allow_reverse()
        {
            reversable_ = true;

            return *this;
        }

        bool is_reversible() const
        {
            return reversable_;
        }

        std::string format_name() const override
        {
            if (reversable_)
            {
                return "--[no-]" + name();
            }
            else
            {
                return "--" + name();
            }
        }

    public:
        virtual void format_value(std::ostream&) const override
        {
        }

        virtual void format_synopsis(std::ostream& s) const override
        {
            s << "[" << format_name() << "]";
        }

        virtual void format_default(std::ostream& s) const override
        {
            if (is_reversible())
            {
                s << "(default: " << (default_ ? "enabled" : "disabled") << ")";
            }
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

            if (env_value == "false" || env_value == "FALSE" || env_value == "without" ||
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
        virtual void update_value(const user_input& arg) override
        {
            if (arg.has_value())
            {
                raise<parsing_error>("a toggle cannot be given a value: ", name());
            }

            if (arg.has_prefix())
            {
                if (!reversable_)
                {
                    raise<parsing_error>("The option --no-", name(), " isn't allowed.");
                }

                if (dirty_ && given())
                {
                    raise<parsing_error>("Cannot provide --", name(), " and --no-", name(),
                                         " at the same time.");
                }

                given_ = 0;
            }
            else
            {
                if (dirty_ && !given())
                {
                    raise<parsing_error>("Cannot provide --", name(), " and --no-", name(),
                                         " at the same time.");
                }

                ++given_;
            }

            dirty_ = true;
        }

        virtual void prepare() override
        {
        }

        virtual void check() override
        {
            if (has_env() && !has_non_default())
            {
                auto env_value = nitro::env::get(env());

                if (!env_value.empty())
                {
                    given_ = parse_env_value(env_value);
                    dirty_ = true;

                    return;
                }
            }

            if (!has_non_default())
            {
                given_ = default_;
            }
        }

        bool matches(const user_input& arg) const override
        {
            if (arg.has_prefix() && arg.name_without_prefix() == name())
            {
                return true;
            }

            return base::matches(arg);
        }

        friend class parser;

    private:
        std::string name_;
        std::string description_;
        int given_;
        int default_ = 0;
        bool reversable_ = false;
    };
} // namespace options
} // namespace nitro
