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

#include <nitro/log/severity.hpp>

#include <string>

extern "C" {
#include <syslog.h>
}

namespace nitro
{
namespace log
{
    namespace sink
    {
        class Syslog
        {
        public:
            Syslog()
            {
                openlog(nullptr, LOG_PID, LOG_USER);
            }

            void sink(severity_level sev, const std::string& formatted_record)
            {
                syslog(get_syslog_priority(sev), "%s", formatted_record.c_str());
            }

            ~Syslog()
            {
                closelog();
            }

        private:
            int get_syslog_priority(severity_level sev)
            {
                switch (sev)
                {
                case severity_level::fatal:
                    return LOG_CRIT;
                case severity_level::error:
                    return LOG_ERR;
                case severity_level::warn:
                    return LOG_WARNING;
                case severity_level::info:
                    return LOG_INFO;
                case severity_level::debug:
                    return LOG_DEBUG;
                case severity_level::trace:
                    return LOG_DEBUG;
                default:
                    return LOG_NOTICE;
                }
            }
        };
    }
}
}
