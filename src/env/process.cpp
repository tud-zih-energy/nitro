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

#include <nitro/env/process.hpp>

extern "C"
{
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#include <windows.h>

#include <tlhelp32.h>
#else
#include <unistd.h>
#endif

#ifdef __linux__
#include <sys/syscall.h>
#include <sys/types.h>
#endif
}

namespace nitro
{
namespace env
{
    int get_pid()
    {
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
        return GetCurrentProcessId();
#else
        return getpid();
#endif
    }

    int get_parent_pid()
    {
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
        HANDLE hSnapshot;
        PROCESSENTRY32 pe32;
        DWORD ppid = 0, pid = nitro::env::get_pid();

        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        __try
        {
            if (hSnapshot == INVALID_HANDLE_VALUE)
                __leave;

            ZeroMemory(&pe32, sizeof(pe32));
            pe32.dwSize = sizeof(pe32);
            if (!Process32First(hSnapshot, &pe32))
                __leave;

            do
            {
                if (pe32.th32ProcessID == pid)
                {
                    ppid = pe32.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        __finally
        {
            if (hSnapshot != INVALID_HANDLE_VALUE)
                CloseHandle(hSnapshot);
        }
        return ppid;
#else
        return getppid();
#endif
    }

    int get_tid()
    {
#if __linux__
        return syscall(SYS_gettid);
#else
        return nitro::env::get_pid();
#endif
    }

} // namespace env
} // namespace nitro
