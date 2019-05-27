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

#include <functional>
#include <memory>

namespace nitro
{
namespace lang
{
    class quaint_ptr : private std::unique_ptr<void, std::function<void(void*)>>
    {
        using base = std::unique_ptr<void, std::function<void(void*)>>;

    public:
        quaint_ptr() = default;

        quaint_ptr(const quaint_ptr&) = delete;
        quaint_ptr& operator=(const quaint_ptr&) = delete;

        quaint_ptr(quaint_ptr&&) = default;
        quaint_ptr& operator=(quaint_ptr&&) = default;

    private:
        template <typename T, typename D>
        quaint_ptr(T* ptr, D d) : base(ptr, std::move(d))
        {
        }

        template <typename T, typename D>
        quaint_ptr(T* ptr, D& d) : base(ptr, d)
        {
        }

    public:
        using base::operator bool;
        using base::get;
        using base::get_deleter;

        using base::operator=;

    public:
        template <typename T>
        T& as() const
        {
            return *static_cast<T*>(get());
        }

        void reset()
        {
            base::reset(nullptr);
        }

        template <typename T, typename... Args>
        friend quaint_ptr make_quaint(Args&&... args);
    };

    template <typename T, typename... Args>
    auto make_quaint(Args&&... args) -> quaint_ptr
    {
        return { new T(std::forward<Args>(args)...),
                 [](void* ptr) { delete static_cast<T*>(ptr); } };
    }
} // namespace lang
} // namespace nitro
