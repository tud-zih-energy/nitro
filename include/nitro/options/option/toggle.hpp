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

#include <nitro/options/fwd.hpp>
#include <nitro/options/option/base.hpp>

#include <ios>
#include <string>

namespace nitro
{
namespace options
{
    class toggle : public crtp_base<toggle>
    {
    public:
        toggle(const std::string& name, const std::string& description);

    public:
        int given() const;

        toggle& default_value(bool def);
        toggle& default_value(int def);

        toggle& allow_reverse();
        bool is_reversible() const;

    public:
        std::string format_name() const override;
        virtual void format_value(std::ostream&) const override;
        virtual void format_synopsis(std::ostream& s) const override;
        virtual std::string format_default() const override;

        static bool parse_env_value(const std::string& env_value);

    private:
        virtual void update_value(const user_input& arg) override;
        virtual void prepare() override;
        virtual void check() override;
        bool matches(const user_input& arg) const override;

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
