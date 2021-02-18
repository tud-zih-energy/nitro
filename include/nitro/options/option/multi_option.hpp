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
#include <nitro/except/raise.hpp>
#include <nitro/lang/optional.hpp>
#include <nitro/lang/string.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace nitro
{
namespace options
{
    class multi_option : public crtp_base<multi_option>
    {
    public:
        multi_option(const std::string& name, const std::string& description)
        : crtp_base(name, description)
        {
        }

    public:
        multi_option& default_value(const std::vector<std::string>& new_default)
        {
            default_ = std::make_unique<std::vector<std::string>>(new_default);

            return *this;
        }

        bool has_default() const
        {
            return static_cast<bool>(default_);
        }

        const std::vector<std::string>& get_default() const
        {
            return *default_;
        }

        multi_option& optional()
        {
            is_optional_ = true;

            return *this;
        }

        bool is_optional() const
        {
            return is_optional_;
        }

    public:
        virtual void format_value(std::ostream& s) const override
        {
            if (has_default())
            {
                s << " [=" << nitro::lang::join(get_default().begin(), get_default().end(), ", ")
                  << "]";
            }
            else
            {
                s << " " << metavar();
            }
        }

        const std::string& get(std::size_t i) const
        {
            return value_[i];
        }

        const std::vector<std::string>& get_all() const
        {
            return value_;
        }

        template <typename T>
        T as(std::size_t i) const
        {
            std::stringstream str;
            str << value_[i];

            T result{};

            str >> result;

            // TODO error handling

            return result;
        }

        std::size_t count() const
        {
            return value_.size();
        }

    private:
        void update_value(const user_input& arg) override
        {
            dirty_ = true;

            value_.push_back(arg.value());
        }

        void prepare() override
        {
        }

        void check() override
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

        friend class parser;

    private:
        std::unique_ptr<std::vector<std::string>> default_;
        std::vector<std::string> value_;

        bool is_optional_ = false;
    };
} // namespace options
} // namespace nitro
