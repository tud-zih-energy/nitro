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
    class argument_group
    {
    public:
        argument_group(const std::string& name,
                       std::map<std::string, broken_options::option>& all_options,
                       std::map<std::string, broken_options::multi_option>& all_multi_options,
                       std::map<std::string, broken_options::toggle>& all_toggles,
                       const std::string& description = std::string(""))
        : name_(name), description_(description), all_options_(all_options),
          all_multi_options_(all_multi_options), all_toggles_(all_toggles)
        {
        }

        void add(base& option)
        {
            options_.emplace(option.name(), std::ref(option));
        }

        const std::string& name() const
        {
            return name_;
        }

        std::size_t size() const
        {
            return options_.size();
        }

        void usage(std::ostream& s) const
        {
            s << std::endl << name_ << ":" << std::endl;
            if (description_.size())
            {
                s << std::endl << description_ << std::endl << std::endl;
            }

            for (auto& iter : options_)
            {
                iter.second.get().format(s);
            }
        }

        broken_options::option& option(const std::string& name,
                                       const std::string& description = std::string())
        {
            if (all_multi_options_.count(name) > 0)
            {
                raise<parser_error>("Trying to redefine multi_option as option. Name: ", name);
            }

            if (all_toggles_.count(name) > 0)
            {
                raise<parser_error>("Trying to redefine toggle as option. Name: ", name);
            }

            if (all_options_.count(name) == 0)
            {
                auto res =
                    all_options_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                         std::forward_as_tuple(name, description));

                add(res.first->second);
            }
            else if (options_.find(name) == options_.end())
            {
                raise<parser_error>("Trying to redefine option in another group. Name: ", name);
            }
            return all_options_.at(name);
        }

        broken_options::multi_option& multi_option(const std::string& name,
                                                   const std::string& description = std::string())
        {
            if (all_options_.count(name) > 0)
            {
                raise<parser_error>("Trying to redefine option as multi_option. Name: ", name);
            }

            if (all_toggles_.count(name) > 0)
            {
                raise<parser_error>("Trying to redefine toggle as multi_option. Name: ", name);
            }

            if (all_multi_options_.count(name) == 0)
            {
                auto res = all_multi_options_.emplace(std::piecewise_construct,
                                                      std::forward_as_tuple(name),
                                                      std::forward_as_tuple(name, description));

                add(res.first->second);
            }
            else if (options_.find(name) == options_.end())
            {
                raise<parser_error>("Trying to redefine multi_option in another group. Name: ",
                                    name);
            }
            return all_multi_options_.at(name);
        }

        broken_options::toggle& toggle(const std::string& name,
                                       const std::string& description = std::string())
        {
            if (all_options_.count(name) > 0)
            {
                raise<parser_error>("Trying to redefine option as multi_option. Name: ", name);
            }

            if (all_multi_options_.count(name) > 0)
            {
                raise<parser_error>("Trying to redefine multi_option as option. Name: ", name);
            }

            if (all_toggles_.count(name) == 0)
            {
                auto res =
                    all_toggles_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                         std::forward_as_tuple(name, description));

                add(res.first->second);
            }
            else if (options_.find(name) == options_.end())
            {
                raise<parser_error>("Trying to redefine toggle in another group. Name: ", name);
            }
            return all_toggles_.at(name);
        }

    private:
        std::string name_;
        std::string description_;
        std::map<std::string, std::reference_wrapper<base>> options_;

        std::map<std::string, broken_options::option>& all_options_;
        std::map<std::string, broken_options::multi_option>& all_multi_options_;
        std::map<std::string, broken_options::toggle>& all_toggles_;
    };

    class proxy_argument_group
    {
    private:
        argument_group& argument_group_;

    public:
        proxy_argument_group(argument_group& argument_group) : argument_group_(argument_group)
        {
        }

        broken_options::option& option(const std::string& name,
                                       const std::string& description = std::string(""))
        {
            return argument_group_.option(name, description);
        }

        broken_options::multi_option& multi_option(const std::string& name,
                                                   const std::string& description = std::string(""))
        {
            return argument_group_.multi_option(name, description);
        }

        broken_options::toggle& toggle(const std::string& name,
                                       const std::string& description = std::string(""))
        {
            return argument_group_.toggle(name, description);
        }

        bool operator==(const proxy_argument_group& rhs) const
        {
            return &argument_group_ == &rhs.argument_group_;
        }
    };
} // namespace broken_options
} // namespace nitro
