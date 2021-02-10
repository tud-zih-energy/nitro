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

#ifndef INCLUDE_NITRO_LOG_LOGGER_HPP
#define INCLUDE_NITRO_LOG_LOGGER_HPP

#include <nitro/log/severity.hpp>
#include <nitro/log/stream.hpp>

#include <nitro/lang/string_ref.hpp>

namespace nitro
{
namespace log
{

    template <typename Clock>
    class timestamp_clock_attribute;

    template <typename Record, template <typename> class Formater, typename Sink,
              template <typename> class Filter>
    class logger : Sink, Formater<Record>, Filter<Record>
    {
        using self = logger;

        logger() = default;

        template <severity_level Severity>
        using actual_stream_t =
            typename actual_stream<Severity, Record, Formater, Sink, Filter>::type;

        static_assert(
            detail::has_attribute_specialization<timestamp_clock_attribute, Record>::value,
            "Record requires a timestamp attribute");

    public:
        static self& instance()
        {
            static self instance_;

            return instance_;
        }

        static bool will_log(Record& r)
        {
            return instance().Filter<Record>::filter(r);
        }

        static void log(severity_level s, Record& r)
        {
            instance().Sink::sink(s, instance().Formater<Record>::format(r));
        }

        static actual_stream_t<severity_level::trace> trace(lang::string_ref tag = nullptr)
        {
            return actual_stream_t<severity_level::trace>(tag);
        }

        static actual_stream_t<severity_level::debug> debug(lang::string_ref tag = nullptr)
        {
            return actual_stream_t<severity_level::debug>(tag);
        }

        static actual_stream_t<severity_level::info> info(lang::string_ref tag = nullptr)
        {
            return actual_stream_t<severity_level::info>(tag);
        }

        static actual_stream_t<severity_level::warn> warn(lang::string_ref tag = nullptr)
        {
            return actual_stream_t<severity_level::warn>(tag);
        }

        static actual_stream_t<severity_level::error> error(lang::string_ref tag = nullptr)
        {
            return actual_stream_t<severity_level::error>(tag);
        }

        static actual_stream_t<severity_level::fatal> fatal(lang::string_ref tag = nullptr)
        {
            return actual_stream_t<severity_level::fatal>(tag);
        }
    };
} // namespace log
} // namespace nitro

#endif // INCLUDE_NITRO_LOG_LOGGER_HPP
