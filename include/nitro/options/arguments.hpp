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

#include <nitro/options/exception.hpp>
#include <nitro/options/option/multi_option.hpp>
#include <nitro/options/option/option.hpp>
#include <nitro/options/option/toggle.hpp>

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace nitro
{
namespace options
{
    class arguments
    {
    public:
        arguments() = default;

        arguments(const std::map<std::string, option*>& option_map,
                  const std::map<std::string, multi_option*>& multi_option_map,
                  const std::map<std::string, toggle*>& toggle_map,
                  const std::vector<std::string>& positionals,
                  const std::set<std::string>& provided)
        : options_(option_map), multi_options_(multi_option_map), toggles_(toggle_map),
          positionals_(positionals), provided_(provided)
        {
        }

    public:
        const std::string& get(const std::string& name) const
        {
            try
            {
                return options_.at(name)->get();
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get value of option --", name,
                                             ": No such option exists!");
            }
        }

        template <typename T>
        T as(const std::string& name) const
        {
            try
            {
                return options_.at(name)->as<T>();
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get value of option --", name,
                                             ": No such option exists!");
            }
        }

    public:
        int given(const std::string& name) const
        {
            try
            {
                return toggles_.at(name)->given();
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get number of times toggle --", name,
                                             " was given: No such toggle exists!");
            }
        }

        std::size_t count(const std::string& name) const
        {
            try
            {
                return multi_options_.at(name)->count();
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get number of times multi-option --", name,
                                             " was given: No such multi-option exists!");
            }
        }

        const std::string& get(const std::string& name, std::size_t i) const
        {
            try
            {
                return multi_options_.at(name)->get(i);
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get value for multi-option --", name,
                                             ": No such multi-option exists!");
            }
        }

        const std::vector<std::string>& get_all(const std::string& name) const
        {
            try
            {
                return multi_options_.at(name)->get_all();
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get all value for multi-option --", name,
                                             ": No such multi-option exists!");
            }
        }

        template <typename T>
        T as(const std::string& name, std::size_t i) const
        {
            try
            {
                return multi_options_.at(name)->as<T>(i);
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get value of multi-option --", name,
                                             ": No such multi-option exists!");
            }
        }

    public:
        const std::string& get(int i) const
        {
            if (i < 0)
            {
                // if you ever manage to have more than 2^31 positionals, I owe you a beer.
                i += static_cast<int>(positionals_.size());
            }
            try
            {
                return positionals_.at(i);
            }
            catch (std::out_of_range& e)
            {
                throw no_such_argument_error("Can not get ", std::to_string(i),
                                             "th positional, there are only ",
                                             std::to_string(positionals_.size()), " positionals");
            }
        }

        const std::string& operator[](int i) const
        {
            return get(i);
        }

        const std::vector<std::string>& positionals() const
        {
            return positionals_;
        }

        bool provided(const std::string& name) const
        {
            return provided_.count(name);
        }

    private:
        std::map<std::string, option*> options_;
        std::map<std::string, multi_option*> multi_options_;
        std::map<std::string, toggle*> toggles_;
        std::vector<std::string> positionals_;
        std::set<std::string> provided_;
    };
} // namespace options
} // namespace nitro
