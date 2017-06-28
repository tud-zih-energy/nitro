# Copyright (c) 2016-2017, Technische Universität Dresden, Germany
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions
#    and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
#    and the following disclaimer in the documentation and/or other materials provided with the
#    distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
#    or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

project(Nitro)

cmake_minimum_required(VERSION 3.2)

# Intialize git submodules if not done already
file(GLOB SUBMODULE_FILES "common/*")
list(LENGTH SUBMODULE_FILES COUNT_COMMON)
file(GLOB SUBMODULE_FILES "tests/Catch/*")
list(LENGTH SUBMODULE_FILES COUNT_CATCH)
if(${COUNT_COMMON} EQUAL 0 OR ${COUNT_CATCH} EQUAL 0)
    message(STATUS "Initializing git submodule")
    execute_process(COMMAND "git" "submodule" "init" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    execute_process(COMMAND "git" "submodule" "update" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

include(common/DefaultBuildType.cmake)

# Intialize git submodules if not done already
file(GLOB SUBMODULE_FILES "${CMAKE_CURRENT_LIST_DIR}/common/*")
list(LENGTH SUBMODULE_FILES COUNT_COMMON)

if(${COUNT_COMMON} EQUAL 0)
    message(STATUS "Initializing git submodule")
    execute_process(COMMAND "git" "submodule" "init" WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
endif()
message(STATUS "Updating git submodule")
execute_process(COMMAND "git" "submodule" "update" WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

include(${CMAKE_CURRENT_LIST_DIR}/common/UnsetIfUpdated.cmake)
UnsetIfUpdated(MIN_LOG_LEVEL CMAKE_BUILD_TYPE)

if (NOT MIN_LOG_LEVEL)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(MIN_LOG_LEVEL "trace" CACHE STRING
                "The minimum required severity level of log messages to be compiled into the binary.")
    else()
        set(MIN_LOG_LEVEL "debug" CACHE STRING
                "The minimum required severity level of log messages to be compiled into the binary.")
    endif()

    set_property(CACHE MIN_LOG_LEVEL PROPERTY "STRINGS" "fatal" "error" "warn" "info" "debug"
            "trace")
endif()

set(Nitro_VERSION 1.0)

add_library(NitroCore INTERFACE)
target_include_directories(NitroCore INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(NitroCore
    INTERFACE
        cxx_auto_type
        cxx_constexpr
        cxx_decltype
        cxx_decltype_auto
        cxx_defaulted_functions
        cxx_defaulted_move_initializers
        cxx_delegating_constructors
        cxx_deleted_functions
        cxx_explicit_conversions
        cxx_generalized_initializers
        cxx_generic_lambdas
        cxx_inheriting_constructors
        cxx_lambdas
        cxx_lambda_init_captures
        cxx_noexcept
        cxx_nullptr
        cxx_override
        cxx_range_for
        cxx_return_type_deduction
        cxx_rvalue_references
        cxx_thread_local
        cxx_trailing_return_types
        cxx_user_literals
        cxx_variable_templates
        cxx_variadic_templates
        cxx_template_template_parameters
)

add_library(NitroDl INTERFACE)
target_link_libraries(NitroDl
    INTERFACE
        Nitro::Core
        ${CMAKE_DL_LIBS}
)

add_library(NitroLog INTERFACE)
target_link_libraries(NitroLog
    INTERFACE
        Nitro::Core
)
target_compile_definitions(NitroLog INTERFACE NITRO_LOG_MIN_SEVERITY=${MIN_LOG_LEVEL})

add_library(NitroAll INTERFACE)
target_link_libraries(NitroAll
    INTERFACE
        Nitro::Core
        Nitro::Log
        Nitro::Dl
)

add_library(Nitro::Nitro ALIAS NitroAll)
add_library(Nitro::Core ALIAS NitroCore)
add_library(Nitro::Dl ALIAS NitroDl)
add_library(Nitro::Log ALIAS NitroLog)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nitro
    DESTINATION include
)
install(TARGETS NitroCore NitroAll NitroDl NitroLog EXPORT NitroTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(EXPORT NitroTargets
    FILE NitroTargets.cmake
    NAMESPACE Nitro::
    DESTINATION lib/cmake/Nitro
)

include(CMakePackageConfigHelpers)
write_basic_package_version_File("NitroConfigVersion.cmake"
    VERSION ${Nitro_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES "NitroConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/NitroConfigVersion.cmake"
    DESTINATION lib/cmake/Nitro
)

include(CTest)

add_subdirectory(tests)
