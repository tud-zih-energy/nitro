/*
 * Copyright (c) 2015-2017, Technische Universität Dresden, Germany
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

#include <nitro/lang/tuple_operators.hpp>

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

namespace nitro
{
namespace jiffy
{
    class Jiffy : public nitro::lang::tuple_operators<Jiffy>
    {
    public:
        Jiffy();

        explicit Jiffy(std::chrono::system_clock::time_point tp);

        explicit Jiffy(const std::string& isoformat);

        Jiffy(const std::string& date, const std::string& format);

    public:
        std::string format(std::string fmt) const;

        std::string isoformat() const
        {
            return format("%Y-%m-%dT%H:%M:%S.%f%z");
        }

        operator std::string() const
        {
            return isoformat();
        }

    public:
        int year() const;

        int month() const;

        int day() const;

        int hour() const;

        int minute() const;

        int second() const;

        int microsecond() const;

    public:
        auto as_tuple()
        {
            return std::tie(tp_);
        }

    public:
        explicit operator std::tm() const;
        explicit operator std::chrono::system_clock::time_point() const;

        std::chrono::system_clock::time_point time_point() const;

    private:
        void clear() const;
        void update_cached_tm() const;

        mutable bool update_needed_ = true;
        mutable std::tm tm_data_;
        std::chrono::system_clock::time_point tp_;
    };

    inline std::ostream& operator<<(std::ostream& s, const Jiffy& j)
    {
        return s << j.isoformat();
    }
} // namespace jiffy
} // namespace nitro
