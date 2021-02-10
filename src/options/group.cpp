/*
 * Copyright (c) 2015-2020, Technische Universität Dresden, Germany
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

#include <nitro/options/option/group.hpp>

#include <nitro/options/option/multi_option.hpp>
#include <nitro/options/option/option.hpp>
#include <nitro/options/option/toggle.hpp>
#include <nitro/options/parser.hpp>

#include <functional>
#include <iostream>
#include <map>

namespace nitro
{
namespace options
{
    group::group(const options::parser& parser, const std::string& name,
                 const std::string& description)
    : parser_(parser), name_(name), description_(description)
    {
    }

    const std::string& group::name() const
    {
        return name_;
    }

    const std::string& group::description() const
    {
        return description_;
    }

    bool group::empty() const
    {
        return options_.empty() && multi_options_.empty() && toggles_.empty();
    }

    void group::usage(std::ostream& s) const
    {
        if (empty())
            return;

        s << std::endl;

        s << name_ << ":" << std::endl;

        if (!description_.empty())
        {
            s << std::endl << description_ << std::endl << std::endl;
        }

        std::map<std::string, const nitro::options::base&> print_options;
        for (auto& it : options_)
            print_options.emplace(it.first, it.second);
        for (auto& it : multi_options_)
            print_options.emplace(it.first, it.second);
        for (auto& it : toggles_)
            print_options.emplace(it.first, it.second);

        for (auto& iter : print_options)
            iter.second.format(s);
    }

    options::option& group::option(const std::string& name, const std::string& description)
    {
        if (parser_.has_option_with_name(name) && options_.count(name) == 0)
        {
            raise<parser_error>("Trying to redefine option. Name: ", name);
        }

        return options_
            .emplace(std::piecewise_construct, std::forward_as_tuple(name),
                     std::forward_as_tuple(name, description))
            .first->second;
    }

    options::multi_option& group::multi_option(const std::string& name,
                                               const std::string& description)
    {
        if (parser_.has_option_with_name(name) && multi_options_.count(name) == 0)
        {
            raise<parser_error>("Trying to redefine option. Name: ", name);
        }

        return multi_options_
            .emplace(std::piecewise_construct, std::forward_as_tuple(name),
                     std::forward_as_tuple(name, description))
            .first->second;
    }

    options::toggle& group::toggle(const std::string& name, const std::string& description)
    {
        if (parser_.has_option_with_name(name) && toggles_.count(name) == 0)
        {
            raise<parser_error>("Trying to redefine option. Name: ", name);
        }

        return toggles_
            .emplace(std::piecewise_construct, std::forward_as_tuple(name),
                     std::forward_as_tuple(name, description))
            .first->second;
    }

    const std::map<std::string, options::option>& group::get_options() const
    {
        return options_;
    }

    const std::map<std::string, options::multi_option>& group::get_multi_options() const
    {
        return multi_options_;
    }

    const std::map<std::string, options::toggle>& group::get_toggles() const
    {
        return toggles_;
    }

} // namespace options
} // namespace nitro
