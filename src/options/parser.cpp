/*
 * Copyright (c) 2015-2019, Technische Universität Dresden, Germany
 * All rights reserved.options
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

#include <nitro/options/parser.hpp>

#include <nitro/options/arguments.hpp>
#include <nitro/options/exception.hpp>
#include <nitro/options/option/group.hpp>
#include <nitro/options/option/multi_option.hpp>
#include <nitro/options/option/option.hpp>
#include <nitro/options/option/toggle.hpp>
#include <nitro/options/user_input.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/format/format.hpp>

#include <set>
#include <string>

namespace nitro
{
namespace options
{
    parser::parser(const std::string& app_name, const std::string& about, const std::string& group)
    : app_name_(app_name), about_(about)
    {
        groups_.emplace(std::piecewise_construct, std::forward_as_tuple("__default"),
                        std::forward_as_tuple(*this, group));
    }

    nitro::options::group& parser::group()
    {
        return groups_.at("__default");
    }

    bool parser::has_option_with_name(const std::string& name) const
    {
        return get_all_multi_options().count(name) + get_all_options().count(name) +
               get_all_toggles().count(name);
    }

    std::map<std::string, options::option*> parser::get_all_options() const
    {
        std::map<std::string, options::option*> tmp;
        for (auto& sg : groups_)
        {
            auto& options = sg.second.get_options();
            for (auto& option : options)
            {
                tmp.emplace(option.first, const_cast<options::option*>(&option.second));
            }
        }
        return tmp;
    }

    std::map<std::string, options::multi_option*> parser::get_all_multi_options() const
    {
        std::map<std::string, options::multi_option*> tmp;
        for (auto& sg : groups_)
        {
            auto& multi_options = sg.second.get_multi_options();
            for (auto& multi_option : multi_options)
            {
                tmp.emplace(multi_option.first,
                            const_cast<options::multi_option*>(&multi_option.second));
            }
        }
        return tmp;
    }

    std::map<std::string, options::toggle*> parser::get_all_toggles() const
    {
        std::map<std::string, options::toggle*> tmp;
        for (auto& group : groups_)
        {
            auto& toggles = group.second.get_toggles();
            for (auto& toggle : toggles)
            {
                tmp.emplace(toggle.first, const_cast<options::toggle*>(&toggle.second));
            }
        }
        return tmp;
    }

    nitro::options::group& parser::group(const std::string& name, const std::string& description)
    {
        auto res = groups_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                   std::forward_as_tuple(*this, name, description));

        if (res.second)
        {
            group_order_.push_back(&(res.first->second));
        }

        return res.first->second;
    }

    options::option& parser::option(const std::string& name, const std::string& description)
    {
        return group().option(name, description);
    }

    options::multi_option& parser::multi_option(const std::string& name,
                                                const std::string& description)
    {
        return group().multi_option(name, description);
    }

    options::toggle& parser::toggle(const std::string& name, const std::string& description)
    {
        return group().toggle(name, description);
    }

    void parser::greedy_postionals(bool enabled)
    {
        greedy_positionals_ = enabled;
    }

    void parser::accept_positionals(std::size_t amount)
    {
        allowed_positionals_ = amount;
    }

    void parser::positional_metavar(const std::string& name)
    {
        positional_name_ = name;
    }

    arguments parser::parse(int argc, const char* const argv[])
    {
        std::vector<options::user_input> args;

        for (int i = 1; i < argc; i++)
        {
            args.emplace_back(argv[i]);
        }

        return parse(args);
    }

    arguments parser::parse(const std::vector<options::user_input>& args)
    {
        check_parser_consistency();

        prepare_options();

        bool only_positionals_mode = false;
        std::vector<std::string> positionals;

        for (auto it = args.begin(); it != args.end(); ++it)
        {
            if (only_positionals_mode || it->is_value())
            {
                if (allowed_positionals_ == positionals.size())
                {
                    raise<parsing_error>("Received unexcepted positional argument: '", it->data(),
                                         "'");
                }

                if (greedy_positionals_)
                {
                    only_positionals_mode = true;
                }

                positionals.push_back(it->data());

                continue;
            }

            if (it->is_double_dash())
            {
                only_positionals_mode = true;
                continue;
            }

            if (try_parse_as_option(get_all_options(), it, args.end()) ||
                try_parse_as_option(get_all_multi_options(), it, args.end()) ||
                try_parse_as_toggle(*it))
            {
                continue;
            }

            if (allowed_positionals_ < positionals.size() && it->is_value())
            {
                positionals.push_back(it->data());
                continue;
            }

            raise<parsing_error>("Argument '", it->data(), "' could not be parsed.");
        }

        validate_options();

        std::set<std::string> provided;

        for_each_option([&provided](auto& option) {
            if (option.has_non_default())
            {
                provided.insert(option.name());
            }
        });

        return arguments(get_all_options(), get_all_multi_options(), get_all_toggles(), positionals,
                         provided);
    }

    std::ostream& parser::usage(std::ostream& s)
    {
        std::string short_list;

        std::set<options::toggle*> long_toggles;

        for (auto& toggle : get_all_toggles())
        {
            if (toggle.second->has_short_name())
            {
                short_list += toggle.second->short_name();

                if (toggle.second->is_reversible())
                {
                    long_toggles.insert(toggle.second);
                }
            }
            else
            {
                long_toggles.insert(toggle.second);
            }
        }

        s << "usage: " << app_name_;

        std::stringstream usage;

        if (!short_list.empty())
        {
            std::sort(short_list.begin(), short_list.end());
            usage << " [-" << short_list << "]";
        }

        for (auto toggle : long_toggles)
        {
            usage << " ";
            toggle->format_synopsis(usage);
        }

        for (auto& opt : get_all_options())
        {
            usage << " ";

            opt.second->format_synopsis(usage);
        }

        for (auto& mopt : get_all_multi_options())
        {
            usage << " ";
            mopt.second->format_synopsis(usage);
        }

        if (allowed_positionals_)
        {
            usage << " [" << positional_name_ << " ...]";
        }

        auto out = usage.str();

        if (!out.empty())
        {
            out = out.substr(1);

            nitro::io::terminal::format_padded(s, out, 8 + app_name_.size(), 80);
        }

        s << std::endl << std::endl;

        if (!about_.empty())
        {
            s << about_ << std::endl << std::endl;
        }

        group().usage(s);

        for (const auto& grp : group_order_)
        {
            grp->usage(s);
        }

        return s;
    }

    template <typename Options, typename Iter>
    bool parser::try_parse_as_option(Options&& options, Iter& it, Iter end)
    {
        for (auto& option : options)
        {
            if (option.second->matches(*it))
            {
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

    bool parser::try_parse_as_toggle(const user_input& in)
    {
        // a given user_input might match more than one toggle, e.g., -ab matches a and b.
        // Therefore, we need to keep checking all toggles, even after one match.
        auto match_found = false;

        for (auto& option : get_all_toggles())
        {
            if (option.second->matches(in))
            {
                option.second->update_value(in);
                match_found = true;
            }
        }

        return match_found;
    }

    void parser::prepare_options()
    {
        for_each_option([](auto& arg) { arg.prepare(); });
    }

    void parser::validate_options()
    {
        for_each_option([](auto& arg) { arg.check(); });
    }

    void parser::check_parser_consistency()
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

} // namespace options
} // namespace nitro
