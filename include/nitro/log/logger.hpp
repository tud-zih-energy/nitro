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

#ifndef INCLUDE_NITRO_LOG_LOGGER_HPP
#define INCLUDE_NITRO_LOG_LOGGER_HPP

#include <nitro/log/severity.hpp>
#include <nitro/log/stream.hpp>

namespace nitro
{
namespace log
{

    template <typename Record, template <typename> class Formater, typename Sink,
              template <typename> class Filter>
    class logger : Sink, Formater<Record>, Filter<Record>
    {
        typedef logger self;

        logger() = default;

    public:
        static void log(Record& r)
        {
            static self instance;

            if (instance.Filter<Record>::filter(r))
            {
                instance.Sink::sink(instance.Formater<Record>::format(r));
            }
        }

        static typename actual_stream<severity_level::trace, Record, Formater, Sink, Filter>::type
        trace()
        {
            return typename actual_stream<severity_level::trace, Record, Formater, Sink,
                                          Filter>::type();
        }

        static typename actual_stream<severity_level::debug, Record, Formater, Sink, Filter>::type
        debug()
        {
            return typename actual_stream<severity_level::debug, Record, Formater, Sink,
                                          Filter>::type();
        }

        static typename actual_stream<severity_level::info, Record, Formater, Sink, Filter>::type
        info()
        {
            return typename actual_stream<severity_level::info, Record, Formater, Sink,
                                          Filter>::type();
        }

        static typename actual_stream<severity_level::warn, Record, Formater, Sink, Filter>::type
        warn()
        {
            return typename actual_stream<severity_level::warn, Record, Formater, Sink,
                                          Filter>::type();
        }

        static typename actual_stream<severity_level::error, Record, Formater, Sink, Filter>::type
        error()
        {
            return typename actual_stream<severity_level::error, Record, Formater, Sink,
                                          Filter>::type();
        }

        static typename actual_stream<severity_level::fatal, Record, Formater, Sink, Filter>::type
        fatal()
        {
            return typename actual_stream<severity_level::fatal, Record, Formater, Sink,
                                          Filter>::type();
        }
    };
}
} // namespace nitro::log

#endif // INCLUDE_NITRO_LOG_LOGGER_HPP