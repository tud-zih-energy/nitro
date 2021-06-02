/*
 * Copyright (c) 2015-2019, Technische Universität Dresden, Germany
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

#include <nitro/options/option/group.hpp>
#include <nitro/options/option/multi_option.hpp>
#include <nitro/options/option/option.hpp>
#include <nitro/options/option/toggle.hpp>

#include <nitro/options/arguments.hpp>

#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace nitro
{
namespace options
{

    class parser
    {
    public:
        parser(const std::string& app_name = std::string("main"),
               const std::string& about = std::string(""),
               const std::string& group = std::string("arguments"));

        auto parse(int argc, const char* const argv[]) -> arguments;
        auto parse(const std::vector<options::user_input>& args) -> arguments;

    public:
        nitro::options::group& group(const std::string& name,
                                     const std::string& description = std::string(""));

        nitro::options::group& group();

        auto option(const std::string& name, const std::string& description = std::string(""))
            -> options::option&;
        auto multi_option(const std::string& name, const std::string& description = std::string(""))
            -> options::multi_option&;
        auto toggle(const std::string& name, const std::string& description = std::string(""))
            -> options::toggle&;

        void greedy_postionals(bool enabled = true);
        void accept_positionals(std::size_t amount = std::numeric_limits<std::size_t>::max());
        void positional_metavar(const std::string& name);

    public:
        std::ostream& usage(std::ostream& s = std::cout);

    private:
        void check_parser_consistency();

        std::map<std::string, nitro::options::option*> get_all_options() const;
        std::map<std::string, nitro::options::multi_option*> get_all_multi_options() const;
        std::map<std::string, nitro::options::toggle*> get_all_toggles() const;

        template <typename Options, typename Iter>
        bool try_parse_as_option(Options&& options, Iter& it, Iter end);
        bool try_parse_as_toggle(const user_input&);

        void prepare_options();
        void validate_options();

        template <typename F>
        void for_each_option(F f)
        {
            for (auto& option : get_all_options())
            {
                f(*option.second);
            }

            for (auto& option : get_all_multi_options())
            {
                f(*option.second);
            }

            for (auto& option : get_all_toggles())
            {
                f(*option.second);
            }
        }

        bool has_option_with_name(const std::string& name) const;

        friend class options::group;

    private:
        std::string app_name_;
        std::string about_;

        std::map<std::string, nitro::options::group> groups_;
        std::vector<nitro::options::group*> group_order_;

        std::size_t allowed_positionals_ = 0;
        bool greedy_positionals_ = false;
        std::string positional_name_ = "args";
    };
} // namespace options
} // namespace nitro
