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

#ifndef INCLUDE_NITRO_BROKEN_OPTIONS_PARSER_HPP
#define INCLUDE_NITRO_BROKEN_OPTIONS_PARSER_HPP

#include <nitro/broken_options/argument.hpp>
#include <nitro/broken_options/multi_option.hpp>
#include <nitro/broken_options/option.hpp>
#include <nitro/broken_options/options.hpp>

#include <nitro/except/raise.hpp>

#include <map>
#include <string>
#include <vector>

namespace nitro
{
namespace broken_options
{

    class parser
    {
    public:
        broken_options::option& option(const std::string& name,
                                       const std::string& description = std::string(""))
        {
            if (multi_options_.count(name) > 0)
            {
                raise("Trying to redefine multi_option as option. Name:", name);
            }

            if (toggles_.count(name) > 0)
            {
                raise("Trying to redefine toggle as multi_option. Name:", name);
            }

            if (options_.count(name) == 0)
            {
                options_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                 std::forward_as_tuple(name, description));
            }
            return options_.at(name);
        }

        broken_options::multi_option& multi_option(const std::string& name,
                                                   const std::string& description = std::string(""))
        {
            if (options_.count(name) > 0)
            {
                raise("Trying to redefine option as multi_option. Name:", name);
            }

            if (toggles_.count(name) > 0)
            {
                raise("Trying to redefine toggle as multi_option. Name:", name);
            }

            if (multi_options_.count(name) == 0)
            {
                multi_options_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                       std::forward_as_tuple(name, description));
            }
            return multi_options_.at(name);
        }

        broken_options::toggle& toggle(const std::string& name,
                                       const std::string& description = std::string(""))
        {
            if (options_.count(name) > 0)
            {
                raise("Trying to redefine option as multi_option. Name:", name);
            }

            if (multi_options_.count(name) > 0)
            {
                raise("Trying to redefine multi_option as option. Name:", name);
            }

            if (toggles_.count(name) == 0)
            {
                toggles_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                                 std::forward_as_tuple(name, description));
            }
            return toggles_.at(name);
        }

        void ignore_unknown(bool ignore = true)
        {
            force_positional_ = ignore;
        }

        options parse(int argc, const char* const argv[])
        {
            for (auto& option : options_)
            {
                option.second.update();
            }

            for (auto& option : multi_options_)
            {
                option.second.update();
            }

            for (auto& option : toggles_)
            {
                option.second.update();
            }

            bool only_positionals = false;
            std::vector<std::string> positionals;

            for (int i = 1; i < argc; i++)
            {
                argument current_arg(argv[i]);

                if (current_arg.is_double_dash())
                {
                    only_positionals = true;

                    continue;
                }

                if (only_positionals || current_arg.is_value())
                {
                    positionals.push_back(current_arg.data());

                    continue;
                }

                bool match_found = false;
                for (auto& option : options_)
                {

                    if (current_arg.has_value())
                    {
                        if (option.second.matches(current_arg.name()))
                        {
                            match_found = true;

                            option.second.update_value(current_arg.value());

                            break;
                        }
                    }
                    else
                    {
                        if (option.second.matches(current_arg.data()))
                        {
                            match_found = true;

                            option.second.update_value(std::string(argv[++i]));

                            break;
                        }
                    }
                }

                for (auto& option : multi_options_)
                {
                    if (match_found)
                    {
                        // could already be set in previous loop
                        break;
                    }

                    if (current_arg.has_value())
                    {
                        if (option.second.matches(current_arg.name()))
                        {
                            match_found = true;

                            option.second.update_value(current_arg.value());

                            break;
                        }
                    }
                    else
                    {
                        if (option.second.matches(current_arg.name()))
                        {
                            match_found = true;

                            option.second.update_value(std::string(argv[++i]));

                            break;
                        }
                    }
                }

                for (auto& option : toggles_)
                {
                    if (match_found)
                    {
                        // could already be set in previous loop
                        break;
                    }

                    if (option.second.matches(current_arg.name()))
                    {
                        match_found = true;

                        option.second.update_value(current_arg.name());

                        break;
                    }
                }

                if (force_positional_)
                {
                    positionals.push_back(current_arg.name());

                    argument next_argument(argv[++i]);
                    if (next_argument.is_value())
                    {
                        positionals.emplace_back(next_argument.data());
                    }

                    continue;
                }

                if (!match_found)
                {
                    raise("Argument '", current_arg.data(), "' could not be parsed.");
                }
            }

            for (auto& option : options_)
            {
                option.second.check();
            }

            for (auto& option : multi_options_)
            {
                option.second.check();
            }

            for (auto& option : toggles_)
            {
                option.second.check();
            }

            return options(options_, multi_options_, toggles_, positionals);
        }

    private:
        std::map<std::string, broken_options::option> options_;
        std::map<std::string, broken_options::multi_option> multi_options_;
        std::map<std::string, broken_options::toggle> toggles_;

        bool force_positional_ = false;
    };
}
} // namespace nitr::broken_options

#endif // INCLUDE_NITRO_BROKEN_OPTIONS_PARSER_HPP
