[![Build Status](https://travis-ci.org/tud-zih-energy/nitro.svg?branch=master)](https://travis-ci.org/tud-zih-energy/nitro)
[![Build status](https://ci.appveyor.com/api/projects/status/i8bo30mu0j95dh3k?svg=true)](https://ci.appveyor.com/project/bmario/nitro)

# nitro

Collection of various, modern C++ libraries and helpers.

## Modules

Currently the following modules are available:

*   `nitro::broken_options` - Yet another library for argument parsing
*   `nitro::dl` - A type-safe wrapper for `dlopen()`
*   `nitro::env` - A platform-dependent library for getting information about the environment, e.g., hostname
*   `nitro::jiffy` - A wrapper for date and time handling compatible to `std::chrono`
*   `nitro::lang` - A collection of various classes used to supplement the standard library
*   `nitro::log` - A lightweight logging library with support for compile-time filtering
*   `nitro::meta` - A collection of template meta functions

## Supported Compiler and Platforms

With the exception of the `jiffy`, `dl`, and `env` module, which are platform dependent, every other Nitro module can be built with a C++14 compliant compiler.

The currently tested compilers are:

On Linux:
*   gcc 4.9
*   gcc 5
*   gcc 6
*   gcc 7
*   clang 3.6
*   clang 3.7
*   clang 3.8
*   clang 3.9
*   clang 4.0
*   clang 5.0

On MacOS:
*   XCode 7.3
*   XCode 8.3
*   XCode 9.3

On Windows:
*   Visual Studio 2015<sup>1</sup>
*   Visual Studio 2017<sup>1</sup>

<sup>1</sup> These do not include tests for `dl`
