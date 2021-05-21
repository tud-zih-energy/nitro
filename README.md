[![Build Status](https://travis-ci.org/tud-zih-energy/nitro.svg?branch=master)](https://travis-ci.org/tud-zih-energy/nitro)
[![Build status](https://ci.appveyor.com/api/projects/status/i8bo30mu0j95dh3k?svg=true)](https://ci.appveyor.com/project/bmario/nitro)
![Build status](https://github.com/tud-zih-energy/nitro/workflows/Modern%20C++%20CI/badge.svg)

# nitro

Collection of various, modern C++ libraries and helpers.

## Modules

Currently the following modules are available:

*   `nitro::options` - Yet another library for argument parsing
*   `nitro::dl` - A type-safe wrapper for `dlopen()`
*   `nitro::env` - A platform-dependent library for getting information about the environment, e.g., hostname
*   `nitro::lang` - A collection of various classes used to supplement the standard library
*   `nitro::log` - A lightweight logging library with support for compile-time filtering
*   `nitro::meta` - A collection of template meta functions

## Supported Compiler and Platforms

With the exception of the `dl` and `env` module, which are platform dependent, every other Nitro module can be built with a C++14 compliant compiler.

The currently tested compilers are:

On Linux:
*   gcc 7
*   gcc 9
*   gcc 10
*   clang 9
*   clang 10
*   clang 11

On MacOS:
*   XCode 10
*   XCode 11
*   XCode 12

On Windows:
*   Visual Studio 2017<sup>1</sup>
*   Visual Studio 2019<sup>1</sup>

<sup>1</sup> These do not include tests for `dl`
