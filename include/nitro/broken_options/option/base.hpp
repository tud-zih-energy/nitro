/*
 * Copyright (c) 2015-2018, Technische Universität Dresden, Germany
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

#include <nitro/broken_options/exception.hpp>
#include <nitro/broken_options/fwd.hpp>

#include <nitro/lang/optional.hpp>

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>

namespace nitro
{
namespace broken_options
{

    class base
    {
    public:
        base(const std::string& name, const std::string& description)
        : name_(name), description_(description)
        {
        }

        bool has_short_name() const
        {
            return static_cast<bool>(short_);
        }

        const std::string& short_name() const
        {
            return *short_;
        }

        const std::string& name() const
        {
            return name_;
        }

        bool has_env() const
        {
            return static_cast<bool>(env_);
        }

        const std::string& env() const
        {
            return *env_;
        }

        const std::string& metavar() const
        {
            return arg_name_;
        }

        virtual void format_value(std::ostream& s) const = 0;

        std::ostream& format(std::ostream& s) const
        {
            s << "  " << std::left << std::setw(38);

            std::stringstream str;

            if (has_short_name())
            {
                str << "-" << short_name() << " [ --" << name() << " ]";
            }
            else
            {
                str << "--" << name();
            }

            format_value(str);

            auto fmt = str.str();

            s << fmt;

            if (fmt.size() > 38)
            {
                s << std::endl << std::setw(40) << ' ' << " ";
            }
            else
            {
                s << " ";
            }

            auto space = 38;

            std::stringstream dstr;
            dstr << description_;

            if (has_env())
            {
                if (!description_.empty())
                {
                    dstr << ' ';
                }
                dstr << "Can be set using the environment variable '" << *env_ << "'.";
            }

            std::string word;

            while (std::getline(dstr, word, ' '))
            {
                if (word.size() + 1 > 38 || static_cast<int>(word.size() + 1) <= space)
                {
                    s << ' ' << word;
                }
                else
                {
                    s << std::endl << std::setw(40) << ' ' << "  " << word;
                    space = 38;
                }

                space -= word.size() + 1;
            }

            return s << std::endl;
        }

    private:
        virtual void update_value(const argument& data) = 0;
        virtual void prepare() = 0;
        virtual void check() = 0;

        virtual bool matches(const argument& arg)
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
                return arg.as_named() == name();
            }

            return false;
        }

        friend class parser;

    private:
        std::string name_;
        std::string description_;

    protected:
        nitro::lang::optional<std::string> short_;
        nitro::lang::optional<std::string> env_;
        std::string arg_name_ = "arg";
    };

    template <typename Option>
    class crtp_base : public base
    {
    public:
        using base::base;
        using base::env;
        using base::short_name;
        using base::metavar;

        Option& metavar(const std::string& arg_name)
        {
            if (arg_name.empty())
            {
                raise<parser_error>("Trying to assign empty string to metavar");
            }

            arg_name_ = arg_name;

            return *static_cast<Option*>(this);
        }

        Option& short_name(const std::string& short_name)
        {
            if (short_ && *short_ != short_name)
            {
                raise<parser_error>("Trying to redefine short name");
            }

            short_ = short_name;

            return *static_cast<Option*>(this);
        }

        Option& env(const std::string& env_name)
        {
            if (has_env() && env() != env_name)
            {
                raise<parser_error>("Trying to redefine env");
            }

            env_ = env_name;

            return *static_cast<Option*>(this);
        }
    };
} // namespace broken_options
} // namespace nitro
