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

#ifndef INCLUDE_NITRO_BROKEN_OPTIONS_OPTIONS_HPP
#define INCLUDE_NITRO_BROKEN_OPTIONS_OPTIONS_HPP

#include <nitro/broken_options/multi_option.hpp>
#include <nitro/broken_options/option.hpp>
#include <nitro/broken_options/toggle.hpp>

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace nitro
{
namespace broken_options
{
    class options
    {
    public:
        options(const std::map<std::string, option>& option_map,
                const std::map<std::string, multi_option>& multi_option_map,
                const std::map<std::string, toggle>& toggle_map,
                const std::vector<std::string>& positionals)
        : options_(option_map), multi_options_(multi_option_map), toggles_(toggle_map),
          positionals_(positionals)
        {
        }

    public:
        const std::string& get(const std::string& name) const
        {
            return options_.at(name).get();
        }

        template <typename T>
        T as(const std::string& name) const
        {
            return options_.at(name).as<T>();
        }

    public:
        bool given(const std::string& name) const
        {
            return toggles_.at(name).given();
        }

        std::size_t count(const std::string& name) const
        {
            return multi_options_.at(name).count();
        }

        const std::string& get(const std::string& name, std::size_t i) const
        {
            return multi_options_.at(name).get(i);
        }

        template <typename T>
        T as(const std::string& name, std::size_t i) const
        {
            return multi_options_.at(name).as<T>(i);
        }

    public:
        const std::string& get(int i) const
        {
            if (i < 0)
            {
                // if you ever manage to have more than 2^31 positionals, I owe you a beer.
                i += static_cast<int>(positionals_.size());
            }
            return positionals_.at(i);
        }

        const std::string& operator[](int i) const
        {
            return get(i);
        }

        const std::vector<std::string>& positionals() const
        {
            return positionals_;
        }

    private:
        std::map<std::string, option> options_;
        std::map<std::string, multi_option> multi_options_;
        std::map<std::string, toggle> toggles_;
        std::vector<std::string> positionals_;
    };
}
} // namespace nitr::broken_options

#endif // INCLUDE_NITRO_BROKEN_OPTIONS_OPTIONS_HPP
