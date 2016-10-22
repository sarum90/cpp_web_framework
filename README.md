C++ Web Framework
=================

This is my attempt at making a frontend/backend framework for C++.

This is constructed using emscripten, and assumes you have that set up on your
computer both for compilation to javascript and for native compilation:

$ emcc --version
emcc (Emscripten gcc/clang-like replacement) 1.36.0 (commit 07b87426f898d6e9c677db291d9088c839197291)

$ clang --version
clang version 3.9.0 (https://github.com/kripken/emscripten-fastcomp-clang/ 271ce598c3d1fe74efadc254f5be1b57edea9f41) (https://github.com/kripken/emscripten-fastcomp/ 61acfb230665464544f2e8db292f8999fc3c628c) (emscripten 1.36.0 : 1.36.0)

I'm going to try to keep a modular feeling to the codebase so this can be split
into different repositories in the future.
