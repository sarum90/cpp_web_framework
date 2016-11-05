C++ Web Framework
=================

This is my attempt at making a frontend/backend framework for C++.

This is constructed using emscripten, and assumes you have that set up on your
computer both for compilation to javascript and for native compilation:

There are commands in the makefiles to help you build your dev environment from
scratch. Someday they will be documented in a script. Approximately:

# Install autoconf and g++-5 or higher, then:
make gcloud
make emsdk
make deps

I'm going to try to keep a modular feeling to the codebase so this can be split
into different repositories in the future.


