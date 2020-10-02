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

#include <nitro/broken_options/option/base.hpp>
#include <nitro/broken_options/option/multi_option.hpp>
#include <nitro/broken_options/option/option.hpp>
#include <nitro/broken_options/option/toggle.hpp>

#include <functional>
#include <iostream>
#include <map>

namespace nitro
{
namespace broken_options
{
    class group
    {
    public:
        std::string name;
        std::string description;

    public:
        group(std::set<std::string>& all_argument_names, int position, const std::string& name,
              const std::string& description = std::string(""))
        : name(name), description(description), all_argument_names_(all_argument_names),
          level_(position)
        {
        }

        group& subgroup(const std::string& name, const std::string& description = std::string(""))
        {
            if (std::find(sub_groups_.begin(), sub_groups_.end(), name) == sub_groups_.end())
                sub_groups_.emplace_back(group(all_argument_names_, level_ + 1, name, description));

            return *std::find(sub_groups_.begin(), sub_groups_.end(), name);
        }

        void usage(std::ostream& s) const
        {
            s << std::endl;
            for (int i = 0; i < level_; ++i)
                s << "  ";

            s << name << ":" << std::endl;

            if (!description.empty())
            {
                s << std::endl << description << std::endl << std::endl;
            }

            std::map<std::string, const nitro::broken_options::base&> print_options;
            for (auto& it : options_)
                print_options.emplace(it.first, it.second);
            for (auto& it : multi_options_)
                print_options.emplace(it.first, it.second);
            for (auto& it : toggles_)
                print_options.emplace(it.first, it.second);

            for (auto& iter : print_options)
                iter.second.format(s, level_ * 2 + 2);
        }

        broken_options::option& option(const std::string& name,
                                       const std::string& description = std::string(""))
        {
            if (all_argument_names_.find(name) != all_argument_names_.end())
            {
                auto result = options_.find(name);
                if (result == options_.end())
                {
                    raise<parser_error>(
                        "Trying to redefine argument name in a different group. Name: ", name);
                }
                else
                {
                    return result->second;
                }
            }

            all_argument_names_.emplace(name);
            return options_
                .emplace(std::piecewise_construct, std::forward_as_tuple(name),
                         std::forward_as_tuple(name, description))
                .first->second;
        }

        broken_options::multi_option& multi_option(const std::string& name,
                                                   const std::string& description = std::string(""))
        {
            if (all_argument_names_.find(name) != all_argument_names_.end())
            {
                auto result = multi_options_.find(name);
                if (result == multi_options_.end())
                {
                    raise<parser_error>(
                        "Trying to redefine argument name in a different group. Name: ", name);
                }
                else
                {
                    return result->second;
                }
            }

            all_argument_names_.emplace(name);
            return multi_options_
                .emplace(std::piecewise_construct, std::forward_as_tuple(name),
                         std::forward_as_tuple(name, description))
                .first->second;
        }

        broken_options::toggle& toggle(const std::string& name,
                                       const std::string& description = std::string(""))
        {
            if (all_argument_names_.find(name) != all_argument_names_.end())
            {
                auto result = toggles_.find(name);
                if (result == toggles_.end())
                {
                    raise<parser_error>(
                        "Trying to redefine argument name in a different group. Name: ", name);
                }
                else
                {
                    return result->second;
                }
            }

            all_argument_names_.emplace(name);
            return toggles_
                .emplace(std::piecewise_construct, std::forward_as_tuple(name),
                         std::forward_as_tuple(name, description))
                .first->second;
        }

        std::map<std::string, broken_options::option&> get_all_options()
        {
            std::map<std::string, broken_options::option&> tmp;
            for (auto& it : options_)
                tmp.emplace(it.first, it.second);

            for (auto& sg : sub_groups_)
            {
                auto add = sg.get_all_options();
                tmp.insert(add.begin(), add.end());
            }
            return tmp;
        }

        std::map<std::string, broken_options::multi_option&> get_all_multi_options()
        {
            std::map<std::string, broken_options::multi_option&> tmp;
            for (auto& it : multi_options_)
                tmp.emplace(it.first, it.second);

            for (auto& sg : sub_groups_)
            {
                auto add = sg.get_all_multi_options();
                tmp.insert(add.begin(), add.end());
            }
            return tmp;
        }

        std::map<std::string, broken_options::toggle&> get_all_toggles()
        {
            std::map<std::string, broken_options::toggle&> tmp;
            for (auto& it : toggles_)
                tmp.emplace(it.first, it.second);

            for (auto& sg : sub_groups_)
            {
                auto add = sg.get_all_toggles();
                tmp.insert(add.begin(), add.end());
            }
            return tmp;
        }

        bool operator==(const group& rhs) const
        {
            return name == rhs.name;
        }

        bool operator==(const std::string& rhs) const
        {
            return name == rhs;
        }

    private:
        std::set<std::string>& all_argument_names_;

        std::map<std::string, broken_options::option> options_;
        std::map<std::string, broken_options::multi_option> multi_options_;
        std::map<std::string, broken_options::toggle> toggles_;

        std::vector<broken_options::group> sub_groups_;
        int level_;
    };
} // namespace broken_options
} // namespace nitro
