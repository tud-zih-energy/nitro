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

#include <nitro/better_options/parser.hpp>

#include <nitro/better_options/arguments.hpp>
#include <nitro/better_options/exception.hpp>
#include <nitro/better_options/option/group.hpp>
#include <nitro/better_options/option/multi_option.hpp>
#include <nitro/better_options/option/option.hpp>
#include <nitro/better_options/option/toggle.hpp>
#include <nitro/better_options/user_input.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/format/format.hpp>

#include <set>
#include <string>

namespace nitro
{
namespace better_options
{
    parser::parser(const std::string& app_name, const std::string& about, const std::string& group)
    : app_name_(app_name), about_(about)
    {
        groups_.emplace(std::piecewise_construct, std::forward_as_tuple("__default"),
                        std::forward_as_tuple(*this, group));
    }

    nitro::better_options::group& parser::group()
    {
        return groups_.at("__default");
    }

    bool parser::has_option_with_name(const std::string& name) const
    {
        return get_all_multi_options().count(name) + get_all_options().count(name) +
               get_all_toggles().count(name);
    }

    std::map<std::string, better_options::option*> parser::get_all_options() const
    {
        std::map<std::string, better_options::option*> tmp;
        for (auto& sg : groups_)
        {
            auto& options = sg.second.get_options();
            for (auto& option : options)
            {
                tmp.emplace(option.first, const_cast<better_options::option*>(&option.second));
            }
        }
        return tmp;
    }

    std::map<std::string, better_options::multi_option*> parser::get_all_multi_options() const
    {
        std::map<std::string, better_options::multi_option*> tmp;
        for (auto& sg : groups_)
        {
            auto& multi_options = sg.second.get_multi_options();
            for (auto& multi_option : multi_options)
            {
                tmp.emplace(multi_option.first,
                            const_cast<better_options::multi_option*>(&multi_option.second));
            }
        }
        return tmp;
    }

    std::map<std::string, better_options::toggle*> parser::get_all_toggles() const
    {
        std::map<std::string, better_options::toggle*> tmp;
        for (auto& group : groups_)
        {
            auto& toggles = group.second.get_toggles();
            for (auto& toggle : toggles)
            {
                tmp.emplace(toggle.first, const_cast<better_options::toggle*>(&toggle.second));
            }
        }
        return tmp;
    }

    nitro::better_options::group& parser::group(const std::string& name,
                                                const std::string& description)
    {
        auto res = groups_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                   std::forward_as_tuple(*this, name, description));

        return res.first->second;
    }

    better_options::option& parser::option(const std::string& name, const std::string& description)
    {
        return group().option(name, description);
    }

    better_options::multi_option& parser::multi_option(const std::string& name,
                                                       const std::string& description)
    {
        return group().multi_option(name, description);
    }

    better_options::toggle& parser::toggle(const std::string& name, const std::string& description)
    {
        return group().toggle(name, description);
    }

    void parser::accept_positionals(std::size_t amount)
    {
        allowed_positionals_ = amount;
    }

    void parser::positional_name(const std::string& name)
    {
        positional_name_ = name;
    }

    void parser::mutually_exclusive(const better_options::base& a, const better_options::base& b)
    {
        if (a.name() == b.name())
        {
            raise<parser_error>("An option cannot conflict with itself.");
        }

        exclusions_.insert(std::make_pair(a.name(), b.name()));
        exclusions_.insert(std::make_pair(b.name(), a.name()));
    }

    arguments parser::parse(int argc, const char* const argv[])
    {
        std::vector<better_options::user_input> args;

        for (int i = 1; i < argc; i++)
        {
            args.emplace_back(argv[i]);
        }

        return parse(args);
    }

    arguments parser::parse(const std::vector<better_options::user_input>& args)
    {
        check_consistency();

        prepare();

        bool only_positionals = false;
        std::vector<std::string> positionals;

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

            if (parse_options(it, args.end()) || parse_multi_options(it, args.end()) ||
                parse_toggles(it))
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

        return arguments(get_all_options(), get_all_multi_options(), get_all_toggles(),
                         positionals);
    }

    std::ostream& parser::usage(std::ostream& s, bool print_default_group)
    {
        std::string short_list;
        std::string option_list;

        for (auto& toggle : get_all_toggles())
        {
            if (toggle.second->has_short_name())
            {
                short_list += toggle.second->short_name();
            }
        }

        s << "usage: " << app_name_;

        if (!short_list.empty())
        {
            std::sort(short_list.begin(), short_list.end());
            s << " [-" << short_list << "]";
        }

        for (auto& opt : get_all_options())
        {
            if (opt.second->has_default())
            {
                option_list += " [--" + opt.second->name() + "]";
            }
            else
            {
                option_list += " --" + opt.second->name();
            }
        }

        for (auto& mopt : get_all_multi_options())
        {
            if (mopt.second->has_default())
            {
                option_list += " [--" + mopt.second->name() + "]";
            }
            else
            {
                option_list += " --" + mopt.second->name();
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

        for (const auto& grp : groups_)
        {
            grp.second.usage(s);
        }

        return s;
    }

    void parser::handle_match(const std::string& name)
    {
        auto range = exclusions_.equal_range(name);

        for (auto it = range.first; it != range.second; ++it)
        {
            if (user_provided_.count(it->second) > 0)
            {
                raise<parsing_error>("provided argument for both mutually exclusive options: '",
                                     name, "' and '", it->second, "'");
            }
        }

        user_provided_.emplace(name);
    }

    template <typename Iter>
    bool parser::parse_options(Iter& it, Iter end)
    {
        for (auto& option : get_all_options())
        {
            if (option.second->matches(*it))
            {
                handle_match(option.first);

                if (it->has_value())
                {
                    option.second->update_value(*it);
                }
                else
                {
                    auto next = it + 1;

                    if (next != end && next->is_value())
                    {
                        option.second->update_value(*next);
                    }
                    else
                    {
                        raise<parsing_error>("missing value for required option: ",
                                             option.second->name());
                    }

                    ++it;
                }

                return true;
            }
        }

        return false;
    }

    template <typename Iter>
    bool parser::parse_multi_options(Iter& it, Iter end)
    {
        for (auto& option : get_all_multi_options())
        {
            if (option.second->matches(*it))
            {
                handle_match(option.first);

                if (it->has_value())
                {
                    option.second->update_value(*it);
                }
                else
                {
                    auto next = it + 1;

                    if (next != end && next->is_value())
                    {
                        option.second->update_value(*next);
                    }
                    else
                    {
                        raise<parsing_error>("missing value for required option: ",
                                             option.second->name());
                    }

                    ++it;
                }

                return true;
            }
        }

        return false;
    }

    template <typename Iter>
    bool parser::parse_toggles(Iter& it)
    {
        auto match_found = false;

        for (auto& option : get_all_toggles())
        {
            if (option.second->matches(*it))
            {
                handle_match(option.first);

                option.second->update_value(*it);
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
                        nitro::format("redefinition of short name_ '{}' from option '{}'") %
                        arg.short_name() % arg.name());
                }
            }
        });
    }

} // namespace better_options
} // namespace nitro
