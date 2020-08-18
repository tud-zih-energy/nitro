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

#include <nitro/broken_options/argument_parser.hpp>

#include <nitro/format/format.hpp>

#include <set>
#include <string>

namespace nitro
{
namespace broken_options
{
    broken_options::option& parser::option(const std::string& name, const std::string& description)
    {
        if (multi_options_.count(name) > 0)
        {
            raise<parser_error>("Trying to redefine multi_option as option. Name: ", name);
        }

        if (toggles_.count(name) > 0)
        {
            raise<parser_error>("Trying to redefine toggle as multi_option. Name: ", name);
        }

        if (options_.count(name) == 0)
        {
            auto res = options_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                        std::forward_as_tuple(name, description));
            arguments_.add(res.first->second);
        }
        return options_.at(name);
    }

    broken_options::multi_option& parser::multi_option(const std::string& name,
                                                       const std::string& description)
    {
        if (options_.count(name) > 0)
        {
            raise<parser_error>("Trying to redefine option as multi_option. Name: ", name);
        }

        if (toggles_.count(name) > 0)
        {
            raise<parser_error>("Trying to redefine toggle as multi_option. Name: ", name);
        }

        if (multi_options_.count(name) == 0)
        {
            auto res = multi_options_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                              std::forward_as_tuple(name, description));
            arguments_.add(res.first->second);
        }
        return multi_options_.at(name);
    }

    broken_options::toggle& parser::toggle(const std::string& name, const std::string& description)
    {
        if (options_.count(name) > 0)
        {
            raise<parser_error>("Trying to redefine option as multi_option. Name: ", name);
        }

        if (multi_options_.count(name) > 0)
        {
            raise<parser_error>("Trying to redefine multi_option as option. Name: ", name);
        }

        if (toggles_.count(name) == 0)
        {
            auto res = toggles_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                        std::forward_as_tuple(name, description));
            arguments_.add(res.first->second);
        }
        return toggles_.at(name);
    }

    void parser::accept_positionals(std::size_t amount)
    {
        allowed_positionals_ = amount;
    }

    void parser::positional_name(const std::string& name)
    {
        positional_name_ = name;
    }

    options parser::parse(int argc, const char* const argv[])
    {
        check_consistency();

        prepare();

        bool only_positionals = false;
        std::vector<std::string> positionals;

        argument_parser args(argc, argv);

        for (auto it = args.begin(); it != args.end(); ++it)
        {
            if (only_positionals || it->is_value())
            {
                if (allowed_positionals_ == positionals.size())
                {
                    raise<parsing_error>("Received unexcepted positional argument: '", it->data(),
                                         "'");
                }

                positionals.push_back(it->data());

                continue;
            }

            if (it->is_double_dash())
            {
                only_positionals = true;
                continue;
            }

            if (parse_options(it, args.end()) || parse_multi_options(it, args.end()))
            {
                continue;
            }

            if (parse_toggles(it, args.end()))
            {
                continue;
            }

            if (allowed_positionals_ < positionals.size() && it->is_value())
            {
                positionals.push_back(it->name());
                continue;
            }

            raise<parsing_error>("Argument '", it->data(), "' could not be parsed.");
        }

        validate();

        return options(options_, multi_options_, toggles_, positionals);
    }

    std::ostream& parser::usage(std::ostream& s, bool print_default_group)
    {
        std::string short_list;
        std::string option_list;

        for (auto& toggle : toggles_)
        {
            if (toggle.second.has_short_name())
            {
                short_list += toggle.second.short_name();
            }
        }

        s << "usage: " << app_name_;

        if (!short_list.empty())
        {
            std::sort(short_list.begin(), short_list.end());
            s << " [-" << short_list << "]";
        }

        for (auto& opt : options_)
        {
            if (opt.second.has_default())
            {
                option_list += " [--" + opt.second.name() + "]";
            }
            else
            {
                option_list += " --" + opt.second.name();
            }
        }

        for (auto& mopt : multi_options_)
        {
            if (mopt.second.has_default())
            {
                option_list += " [--" + mopt.second.name() + "]";
            }
            else
            {
                option_list += " --" + mopt.second.name();
            }
        }

        if (allowed_positionals_)
        {
            option_list += " [" + positional_name_ + " ...]";
        }

        s << option_list << std::endl << std::endl;

        if (!about_.empty())
        {
            s << about_ << std::endl << std::endl;
        }

        if (print_default_group)
        {
            arguments_.usage(s);
        }

        return s;
    }

    template <typename Iter>
    bool parser::parse_options(Iter& it, Iter end)
    {
        for (auto& option : options_)
        {

            if (it->has_value())
            {
                if (option.second.matches(it->name()))
                {

                    option.second.update_value(*it);

                    return true;
                }
            }
            else if (option.second.matches(it->data()))
            {
                auto next = it + 1;

                if (next != end && next->is_value())
                {
                    option.second.update_value(*next);
                }
                else
                {
                    raise<parsing_error>("missing value for required option: ",
                                         option.second.name());
                }

                ++it;
                return true;
            }
        }

        return false;
    }

    template <typename Iter>
    bool parser::parse_multi_options(Iter& it, Iter end)
    {
        for (auto& option : multi_options_)
        {
            if (it->has_value())
            {
                if (option.second.matches(it->name()))
                {
                    option.second.update_value(*it);

                    return true;
                }
            }
            else if (option.second.matches(it->name()))
            {
                auto next = it + 1;

                if (next != end && next->is_value())
                {
                    option.second.update_value(*next);
                }
                else
                {
                    raise<parsing_error>("missing value for required option: ",
                                         option.second.name());
                }

                ++it;
                return true;
            }
        }

        return false;
    }

    template <typename Iter>
    bool parser::parse_toggles(Iter& it, Iter end)
    {
        (void)end;

        bool match_found = false;

        for (auto& option : toggles_)
        {
            if (option.second.matches(*it))
            {
                option.second.update_value(*it);
                match_found = true;
            }
        }

        return match_found;
    }

    void parser::prepare()
    {
        for_each_option([](auto& arg) { arg.prepare(); });
    }

    void parser::validate()
    {
        for_each_option([](auto& arg) { arg.check(); });
    }

    void parser::check_consistency()
    {
        std::set<std::string> short_names;

        for_each_option([&short_names](auto& arg) {
            if (arg.has_short_name())
            {
                auto res = short_names.emplace(arg.short_name());

                if (!res.second)
                {
                    raise<parser_error>(
                        nitro::format("redefinition of short name '{}' from option '{}'") %
                        arg.short_name() % arg.name());
                }
            }
        });
    }

    template <typename F>
    void parser::for_each_option(F f)
    {
        for (auto& option : options_)
        {
            f(option.second);
        }

        for (auto& option : multi_options_)
        {
            f(option.second);
        }

        for (auto& option : toggles_)
        {
            f(option.second);
        }
    }

} // namespace broken_options
} // namespace nitro
