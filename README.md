[![Build Status](https://travis-ci.org/tud-zih-energy/nitro.svg?branch=master)](https://travis-ci.org/tud-zih-energy/nitro)

# nitro

Collection of various, modern C++ libraries and helpers.

## Modules

Currently the following modules are available:

*   `nitro::broken_options` - Yet another library for argument parsing
*   `nitro::dl` - A typesafe wrapper for `dlopen()`
*   `nitro::env` - A platform-dependent libray for getting information about the environment, e.g., hostname
*   `nitro::lang` - A collection of various classes used to supplement the standard library
*   `nitro::log` - A lightweight logging library with support for compile-time filtering
*   `nitro::meta` - A collection of template meta functions

## Supported Compiler and Platforms

With the exception of the `dl` and `env` module, which are platform dependent, every other Nitro module should build with a C++14 compliant compiler.

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
*   XCode 7
*   XCode 8
