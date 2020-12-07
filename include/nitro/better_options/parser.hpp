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

#include <nitro/better_options/fwd.hpp>

#include <nitro/better_options/option/group.hpp>
#include <nitro/better_options/option/multi_option.hpp>
#include <nitro/better_options/option/option.hpp>
#include <nitro/better_options/option/toggle.hpp>

#include <nitro/better_options/arguments.hpp>

#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace nitro
{
namespace better_options
{

    class parser
    {
    public:
        parser(const std::string& app_name = std::string("main"),
               const std::string& about = std::string(""),
               const std::string& group = std::string("arguments"));

        auto parse(int argc, const char* const argv[]) -> arguments;
        auto parse(const std::vector<better_options::user_input>& args) -> arguments;

    public:
        nitro::better_options::group& group(const std::string& name,
                                            const std::string& description = std::string(""));

        nitro::better_options::group& group();

        auto option(const std::string& name, const std::string& description = std::string(""))
            -> better_options::option&;
        auto multi_option(const std::string& name, const std::string& description = std::string(""))
            -> better_options::multi_option&;
        auto toggle(const std::string& name, const std::string& description = std::string(""))
            -> better_options::toggle&;

        void accept_positionals(std::size_t amount = std::numeric_limits<std::size_t>::max());
        void positional_name(const std::string& name);

        void mutually_exclusive(const better_options::base& a, const better_options::base& b);

    public:
        std::ostream& usage(std::ostream& s = std::cout, bool print_default_group = true);

    private:
        void check_consistency();
        void handle_match(const std::string& name);

        std::map<std::string, nitro::better_options::option*> get_all_options() const;
        std::map<std::string, nitro::better_options::multi_option*> get_all_multi_options() const;
        std::map<std::string, nitro::better_options::toggle*> get_all_toggles() const;

        template <typename Iter>
        bool parse_options(Iter& it, Iter end);

        template <typename Iter>
        bool parse_multi_options(Iter& it, Iter end);

        template <typename Iter>
        bool parse_toggles(Iter& it);

        void prepare();
        void validate();

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

        friend class better_options::group;

    private:
        std::string app_name_;
        std::string about_;

        std::map<std::string, nitro::better_options::group> groups_;
        std::multimap<std::string, std::string> exclusions_;
        std::set<std::string> user_provided_;

        std::size_t allowed_positionals_ = 0;
        std::string positional_name_ = "args";
    };
} // namespace better_options
} // namespace nitro
