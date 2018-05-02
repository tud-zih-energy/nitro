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
    message(AUTHOR_WARNING "Nitro targets were already defined. Skipping.")
else()

project(Nitro)

cmake_minimum_required(VERSION 3.2)

include(cmake/GitSubmoduleUpdate.cmake)
git_submodule_update()

include(common/DefaultBuildType.cmake)
include(common/UnsetIfUpdated.cmake)

UnsetIfUpdated(NITRO_LOG_LEVEL CMAKE_BUILD_TYPE)

if (NOT NITRO_LOG_LEVEL)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(NITRO_LOG_LEVEL "trace" CACHE STRING
                "The minimum required severity level of log messages available during runtime.")
    else()
        set(NITRO_LOG_LEVEL "debug" CACHE STRING
                "The minimum required severity level of log messages available during runtime.")
    endif()

    set_property(CACHE NITRO_LOG_LEVEL PROPERTY "STRINGS" "fatal" "error" "warn" "info" "debug"
            "trace")
endif()

set(Nitro_VERSION 1.0)

if(NOT NITRO_POSITION_INDEPENDENT_CODE)
    set(NITRO_POSITION_INDEPENDENT_CODE OFF)
endif()
option(NITRO_POSITION_INDEPENDENT_CODE "Whether to build Nitro libraries with position independent code" ${NITRO_POSITION_INDEPENDENT_CODE})

add_library(nitro-core INTERFACE)
target_include_directories(nitro-core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(nitro-core
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
        cxx_trailing_return_types
        cxx_user_literals
        cxx_variadic_templates
        cxx_template_template_parameters
)

add_library(nitro-dl INTERFACE)
target_link_libraries(nitro-dl
    INTERFACE
        Nitro::nitro-core
        ${CMAKE_DL_LIBS}
)

add_library(nitro-log INTERFACE)
target_link_libraries(nitro-log
    INTERFACE
        Nitro::nitro-core
)
target_compile_definitions(nitro-log INTERFACE NITRO_LOG_MIN_SEVERITY=${NITRO_LOG_LEVEL})

add_library(nitro-env SHARED src/env/get.cpp src/env/hostname.cpp)
target_link_libraries(nitro-env
    PUBLIC
        Nitro::nitro-core
)

add_library(nitro-env-static STATIC src/env/get.cpp src/env/hostname.cpp)
target_link_libraries(nitro-env-static
    PUBLIC
        Nitro::nitro-core
)
set_target_properties(nitro-env-static PROPERTIES
    POSITION_INDEPENDENT_CODE ${NITRO_POSITION_INDEPENDENT_CODE}
    OUTPUT_NAME nitro-env
)

add_library(nitro-jiffy src/jiffy/jiffy.cpp)
target_link_libraries(nitro-jiffy
    PUBLIC
        Nitro::nitro-core
)

add_library(nitro-jiffy-static STATIC src/jiffy/jiffy.cpp)
target_link_libraries(nitro-jiffy-static
    PUBLIC
        Nitro::nitro-core
)
set_target_properties(nitro-jiffy-static PROPERTIES
    POSITION_INDEPENDENT_CODE ${NITRO_POSITION_INDEPENDENT_CODE}
    OUTPUT_NAME nitro-jiffy
)

add_library(nitro INTERFACE)
target_link_libraries(nitro
    INTERFACE
        Nitro::nitro-core
        Nitro::nitro-log
        Nitro::nitro-dl
        Nitro::nitro-env
        Nitro::nitro-jiffy
)

add_library(Nitro::nitro ALIAS nitro)
add_library(Nitro::nitro-core ALIAS nitro-core)
add_library(Nitro::nitro-dl ALIAS nitro-dl)
add_library(Nitro::nitro-log ALIAS nitro-log)
add_library(Nitro::nitro-env ALIAS nitro-env-static)
add_library(Nitro::nitro-jiffy ALIAS nitro-jiffy-static)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # build Debug with all warnings
    target_compile_options(nitro INTERFACE $<$<CONFIG:Debug>:-Wall -pedantic -Wextra>)

    install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/nitro
        DESTINATION include
    )

    install(TARGETS
            nitro
            nitro-core
            nitro-dl
            nitro-log
            nitro-env
            nitro-env-static
            nitro-jiffy
            nitro-jiffy-static
        EXPORT NitroTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
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

    include(CTest)
    add_subdirectory(tests)
else()
    set_target_properties(nitro-env nitro-jiffy nitro-env-static nitro-jiffy-static PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

endif()
