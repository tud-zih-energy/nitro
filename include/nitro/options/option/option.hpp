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

#include <nitro/options/fwd.hpp>

#include <nitro/options/option/base.hpp>

#include <sstream>
#include <string>
#include <type_traits>

namespace nitro
{
namespace options
{
    class option : public crtp_base<option>
    {
    public:
        option(const std::string& name, const std::string& description);

    public:
        option& default_value(const std::string& new_default);
        bool has_default() const;
        const std::string& get_default() const;

        option& optional();
        bool is_optional() const override;

        virtual void format_value(std::ostream& s) const override;
        virtual void format_synopsis(std::ostream& s) const override;
        virtual std::string format_default() const override;

    public:
        const std::string& get() const;

        template <typename T>
        std::enable_if_t<!std::is_constructible<T, std::string>::value, T> as() const
        {
            std::stringstream str;
            str << *value_;

            T result;

            str >> result;

            // TODO error handling

            return result;
        }

        template <typename T>
        std::enable_if_t<std::is_constructible<T, std::string>::value, T> as() const
        {
            return T(*value_);
        }

    private:
        void update_value(const user_input& arg) override;

        void prepare() override;
        void check() override;

        friend class parser;

    private:
        std::optional<std::string> default_;
        std::optional<std::string> value_;

        bool is_optional_ = false;
    };
} // namespace options
} // namespace nitro
