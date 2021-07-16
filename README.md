# chip8-interpreter
A Chip-8 interpreter written in C, for my own learning purposes.<br/>
SDL2 is used for graphics, and will eventually be used for sound and keyboard input.<br/>
Now maintained on GitLab.

## Cross-Platform Support

Working on Debian 10 (Buster).<br/>
Working on Windows 10 Home version 2004, using MSYS2 with the following packages:
* ``mingw-w64-x86_64-clang`` version 10.0.0-3<br/>
* ``mingw-w64-x86_64-pkg-config`` version 0.29.2-1<br/>
* ``mingw-w64-x86_64-SDL2`` version 2.0.12-4<br/>
* ``cmake`` version 3.17.3-2<br/>
* ``ninja`` version 1.10.0-1<br/>

Currently I have no plans to support macOS, due to a lack of testing hardware.

## Build System

This interpreter uses the CMake build system.<br/>
When building on Windows, an environment that provides ``pkg-config`` will be required.

## License

This interpreter is licensed under the BSD 3-Clause license, which can be viewed in the LICENSE file.
