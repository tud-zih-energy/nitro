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

#ifndef INCLUDE_NITRO_LOG_STREAM_HPP
#define INCLUDE_NITRO_LOG_STREAM_HPP

#include <nitro/log/attribute/tag.hpp>
#include <nitro/log/detail/has_attribute.hpp>
#include <nitro/log/detail/set_attribute.hpp>
#include <nitro/log/severity.hpp>

#include <nitro/meta/callable.hpp>

#include <chrono>
#include <memory>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace nitro
{
namespace log
{
    class tag_attribute;

    template <typename Record, template <typename> class Formatter, typename Sink,
              template <typename> class Filter>
    class logger;

    namespace detail
    {
        template <typename Record>
        void set_timestamp(Record& r)
        {
            r.timestamp() = r.timestamp_clock_get_time();
        }

        template <typename Record, bool has_tag>
        class set_tag_attribute
        {
        public:
            void operator()(Record&, std::string_view)
            {
            }
        };

        template <typename Record>
        class set_tag_attribute<Record, true>
        {
        public:
            void operator()(Record& r, std::string_view tag)
            {
                if (!tag.empty())
                {
                    r.tag() = tag;
                }
            }
        };

        template <typename Record>
        void set_tag(Record& r, std::string_view tag)
        {
            set_tag_attribute<Record, detail::has_attribute<tag_attribute, Record>::value>()(r,
                                                                                             tag);
        }

        template <typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, severity_level Severity>
        class smart_stream
        {
            typedef nitro::log::logger<Record, Formatter, Sink, Filter> logger;

        public:
            smart_stream(std::string_view tag) : r(new Record), s()
            {
                detail::set_tag(*r, tag);
                detail::set_severity<Record>()(*r, Severity);

                if (logger::will_log(*r))
                {
                    s.reset(new std::stringstream());
                }
                else
                {
                    r.reset();
                }
            }

            smart_stream(smart_stream&& ss) : r(std::move(ss.r)), s(std::move(ss.s))
            {
            }

            ~smart_stream()
            {
                if (r)
                {
                    detail::set_timestamp(*r);
                    r->message() = s->str();
                    logger::log(Severity, *r);
                }
            }

            Record& record()
            {
                return *r;
            }

            std::stringstream& sstr()
            {
                return *s;
            }

            operator bool() const
            {
                return static_cast<bool>(s);
            }

        private:
            std::unique_ptr<Record> r;
            std::unique_ptr<std::stringstream> s;
        };

        template <typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, typename T, severity_level Severity,
                  typename std::enable_if<nitro::meta::is_callable<T, std::string()>::value,
                                          int>::type = 0>
        smart_stream<Record, Formatter, Sink, Filter, Severity>&
        operator<<(smart_stream<Record, Formatter, Sink, Filter, Severity>& s, T t)
        {
            if (s)
            {
                s.sstr() << t();
            }

            return s;
        }

        template <typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, typename T, severity_level Severity,
                  typename std::enable_if<nitro::meta::is_callable<T, std::string()>::value,
                                          int>::type = 0>
        smart_stream<Record, Formatter, Sink, Filter, Severity>
        operator<<(smart_stream<Record, Formatter, Sink, Filter, Severity>&& s, T t)
        {
            if (s)
            {
                s.sstr() << t();
            }

            return std::move(s);
        }

        template <typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, typename T, severity_level Severity,
                  typename std::enable_if<!nitro::meta::is_callable<T, std::string()>::value,
                                          int>::type = 0>
        smart_stream<Record, Formatter, Sink, Filter, Severity>
        operator<<(smart_stream<Record, Formatter, Sink, Filter, Severity>&& s, const T& t)
        {
            if (s)
            {
                s.sstr() << t;
            }

            return std::move(s);
        }

        template <typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, typename T, severity_level Severity,
                  typename std::enable_if<!nitro::meta::is_callable<T, std::string()>::value,
                                          int>::type = 0>
        smart_stream<Record, Formatter, Sink, Filter, Severity>&
        operator<<(smart_stream<Record, Formatter, Sink, Filter, Severity>& s, const T& t)
        {
            if (s)
            {
                s.sstr() << t;
            }

            return s;
        }

        class null_stream
        {
        public:
            null_stream(std::string_view)
            {
            }
        };

        template <typename T>
        null_stream operator<<(null_stream&& s, const T&)
        {
            return s;
        }

        template <typename T>
        null_stream& operator<<(null_stream& s, const T&)
        {
            return s;
        }

        template <bool, typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, severity_level Severity>
        struct actual_stream
        {
            typedef smart_stream<Record, Formatter, Sink, Filter, Severity> type;
        };

        template <typename Record, template <typename> class Formatter, typename Sink,
                  template <typename> class Filter, severity_level Severity>
        struct actual_stream<false, Record, Formatter, Sink, Filter, Severity>
        {
            typedef null_stream type;
        };

    } // namespace detail

    template <severity_level Severity, typename Record, template <typename> class Formatter,
              typename Sink, template <typename> class Filter>
    struct actual_stream
    {
        using type =
            typename detail::actual_stream<Severity >= severity_level::NITRO_LOG_MIN_SEVERITY,
                                           Record, Formatter, Sink, Filter, Severity>::type;
    };
} // namespace log
} // namespace nitro

#endif // INCLUDE_NITRO_LOG_STREAM_HPP
