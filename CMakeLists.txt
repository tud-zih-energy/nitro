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

if(TARGET Nitro::nitro)
    if(Git_FOUND)
        _is_git(${CMAKE_CURRENT_SOURCE_DIR} IN_GIT)
    endif()
    if(IN_GIT)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --tags --dirty
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE THIS_NITRO_VERSION_STRING
        )
        string(STRIP ${THIS_NITRO_VERSION_STRING} THIS_NITRO_VERSION_STRING)
    else()
        message(AUTHOR_WARNING "Cannot detect Nitro version.")
        set(THIS_NITRO_VERSION_STRING "unknown")
    endif()

    if(THIS_NITRO_VERSION_STRING STREQUAL NITRO_VERSION_STRING)
        message(STATUS "Compatible Nitro targets were already defined. Skipping.")
    else()
        message(AUTHOR_WARNING "A different version of Nitro already defined the Nitro targets")
    endif()
else()

cmake_minimum_required(VERSION 3.3)

project(Nitro VERSION 2.0.0)

if(MSVC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:W0 ")
        set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
    endif()
endif()

include(cmake/GitSubmoduleUpdate.cmake)
find_package(Git)
if(Git_FOUND)
    _is_git(${CMAKE_CURRENT_SOURCE_DIR} IN_GIT)
endif()
if(IN_GIT)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --tags --dirty
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE _NITRO_VERSION_STRING
    )
    string(STRIP ${_NITRO_VERSION_STRING} _NITRO_VERSION_STRING)
else()
    message(AUTHOR_WARNING "Cannot detect Nitro version.")
    set(_NITRO_VERSION_STRING "unknown")
endif()

set(NITRO_VERSION_STRING ${_NITRO_VERSION_STRING} CACHE INTERNAL "")
unset(_NITRO_VERSION_STRING)

include(cmake/DefaultBuildType.cmake)
include(cmake/UnsetIfUpdated.cmake)

UnsetIfUpdated(NITRO_LOG_LEVEL CMAKE_BUILD_TYPE)

if (NOT NITRO_LOG_LEVEL)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(NITRO_LOG_LEVEL "trace" CACHE STRING
                "The minimum required severity level of log messages available during runtime.")
    else()
        set(NITRO_LOG_LEVEL "debug" CACHE STRING
                "The minimum required severity level of log messages available during runtime.")
    endif()

    set_property(CACHE NITRO_LOG_LEVEL PROPERTY "STRINGS" "fatal" "error" "warn" "info" "debug" "trace")
endif()

option(NITRO_POSITION_INDEPENDENT_CODE "Whether to build Nitro libraries with position independent code" OFF)
option(NITRO_BUILD_TESTING  "Whether to build Nitro tests" ON)

add_library(nitro-core INTERFACE)
target_compile_features(nitro-core INTERFACE cxx_std_17)


if(MSVC)
    target_compile_options(nitro-core INTERFACE /Zc:__cplusplus)
endif()

add_library(nitro-dl INTERFACE)
target_link_libraries(nitro-dl
    INTERFACE
        Nitro::core
        ${CMAKE_DL_LIBS}
)

add_library(nitro-log INTERFACE)
target_link_libraries(nitro-log
    INTERFACE
        Nitro::core
)
target_compile_definitions(nitro-log INTERFACE NITRO_LOG_MIN_SEVERITY=${NITRO_LOG_LEVEL})

set(NITRO_ENV_SRCS
    src/env/get.cpp
    src/env/hostname.cpp
    src/env/process.cpp
)

if(NOT WIN32)
    add_library(nitro-env SHARED ${NITRO_ENV_SRCS})
    target_link_libraries(nitro-env
        PUBLIC
            Nitro::core
            $<$<CXX_COMPILER_ID:MSVC>:ws2_32>
    )
endif()

add_library(nitro-env-static STATIC ${NITRO_ENV_SRCS})
target_link_libraries(nitro-env-static
    PUBLIC
        Nitro::core
        $<$<CXX_COMPILER_ID:MSVC>:ws2_32>
)
set_target_properties(nitro-env-static PROPERTIES
    POSITION_INDEPENDENT_CODE ${NITRO_POSITION_INDEPENDENT_CODE}
    OUTPUT_NAME nitro-env
)

set(NITRO_OPTIONS_SRCS
    src/options/parser.cpp
    src/options/group.cpp
    src/options/option.cpp
    src/options/toggle.cpp
    src/options/multi_option.cpp
 )

if(NOT WIN32)
    add_library(nitro-options SHARED ${NITRO_OPTIONS_SRCS})
    target_link_libraries(nitro-options
        PUBLIC
            Nitro::core
            nitro-env
            $<$<CXX_COMPILER_ID:MSVC>:ws2_32>
    )
endif()

add_library(nitro-options-static STATIC ${NITRO_OPTIONS_SRCS})
target_link_libraries(nitro-options-static
    PUBLIC
        Nitro::core
        Nitro::env
        $<$<CXX_COMPILER_ID:MSVC>:ws2_32>
)
set_target_properties(nitro-options-static PROPERTIES
    POSITION_INDEPENDENT_CODE ${NITRO_POSITION_INDEPENDENT_CODE}
    OUTPUT_NAME nitro-options
)


add_library(nitro INTERFACE)
target_link_libraries(nitro
    INTERFACE
        Nitro::core
        Nitro::log
        Nitro::dl
        Nitro::env
)

set_target_properties(nitro-core PROPERTIES EXPORT_NAME core)
set_target_properties(nitro-dl PROPERTIES EXPORT_NAME dl)
set_target_properties(nitro-log PROPERTIES EXPORT_NAME log)
if(WIN32)
    set_target_properties(nitro-env-static PROPERTIES EXPORT_NAME env)
    set_target_properties(nitro-options-static PROPERTIES EXPORT_NAME options)
else()
    set_target_properties(nitro-env PROPERTIES EXPORT_NAME env)
    set_target_properties(nitro-options PROPERTIES EXPORT_NAME options)
endif()

add_library(Nitro::nitro ALIAS nitro)
add_library(Nitro::core ALIAS nitro-core)
add_library(Nitro::dl ALIAS nitro-dl)
add_library(Nitro::log ALIAS nitro-log)
add_library(Nitro::env ALIAS nitro-env-static)
add_library(Nitro::options ALIAS nitro-options-static)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # build Debug with all warnings
    if(MSVC)
        target_compile_options(nitro INTERFACE $<$<CONFIG:Debug>:/W4>)
    else()
        target_compile_options(nitro INTERFACE $<$<CONFIG:Debug>:-Wall -pedantic -Wextra>)
    endif()

    target_include_directories(nitro-core INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/nitro
        DESTINATION include
    )

    if(WIN32)
        install(TARGETS
                nitro
                nitro-core
                nitro-dl
                nitro-log
                nitro-env-static
                nitro-options-static
            EXPORT NitroTargets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
        )
    else()
        install(TARGETS
                nitro
                nitro-core
                nitro-dl
                nitro-log
                nitro-env
                nitro-env-static
                nitro-options
                nitro-options-static
            EXPORT NitroTargets
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
        )
    endif()
    install(EXPORT NitroTargets
        DESTINATION lib/cmake/Nitro
        NAMESPACE Nitro::
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_File("NitroConfigVersion.cmake"
        VERSION ${Nitro_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(FILES "NitroConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/NitroConfigVersion.cmake"
        DESTINATION lib/cmake/Nitro
    )

    if (NITRO_BUILD_TESTING)
        include(CTest)
        add_subdirectory(tests)
    endif()
else()
    target_include_directories(nitro-core SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    if(WIN32)
        set_target_properties(
                nitro-env-static
                nitro-options-static
            PROPERTIES
                EXCLUDE_FROM_ALL TRUE
        )
    else()
        set_target_properties(
                nitro-env
                nitro-env-static
                nitro-options
                nitro-options-static
            PROPERTIES
                EXCLUDE_FROM_ALL TRUE
        )
    endif()
endif()

endif()
