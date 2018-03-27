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

#ifndef INCLUDE_NITRO_BROKEN_OPTIONS_MULTI_OPTION_HPP
#define INCLUDE_NITRO_BROKEN_OPTIONS_MULTI_OPTION_HPP

#include <nitro/broken_options/exception.hpp>
#include <nitro/broken_options/fwd.hpp>
#include <nitro/broken_options/option/base.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/lang/optional.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace nitro
{
namespace broken_options
{
    class multi_option : public crtp_base<multi_option>
    {
    public:
        multi_option(const std::string& name, const std::string& description)
        : crtp_base(name, description), ref_(nullptr)
        {
        }

    public:
        multi_option& default_value(const std::vector<std::string>& new_default)
        {
            default_ = new_default;

            return *this;
        }

        bool has_default() const
        {
            return !default_.empty();
        }

        const std::vector<std::string>& default_value() const
        {
            return default_;
        }

        multi_option& ref(std::vector<std::string>& target)
        {
            ref_ = &target;

            return *this;
        }

    public:
        virtual void format_value(std::ostream& s) const override
        {
            if (has_default())
            {
                s << " [=";

                bool first = true;

                for (auto& value : default_value())
                {
                    if (!first)
                    {
                        s << ", ";
                    }
                    s << value;
                    first = false;
                }

                s << "]";
            }
            else
            {
                s << " arg";
            }
        }

        const std::string& get(std::size_t i) const
        {
            return value_[i];
        }

        template <typename T>
        T as(std::size_t i) const
        {
            std::stringstream str;
            str << value_[i];

            T result;

            str >> result;

            // TODO error handling

            return result;
        }

        std::size_t count() const
        {
            return value_.size();
        }

    private:
        void update_value(const argument& arg) override
        {
            if (ref_)
            {
                ref_->push_back(arg.value());
            }

            value_.push_back(arg.value());
        }

        void update() override
        {
            if (default_.size())
            {
                for (auto& item : default_)
                {
                    update_value(item);
                }
            }
        }

        void check() override
        {
            if (value_.empty())
            {
                raise<parsing_error>("missing value for required option");
            }
        }

        friend class parser;

    private:
        std::vector<std::string> default_;
        std::vector<std::string> value_;
        std::vector<std::string>* ref_;
    };
} // namespace broken_options
} // namespace nitro

#endif // INCLUDE_NITRO_BROKEN_OPTIONS_MULTI_OPTION_HPP
