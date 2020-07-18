# chip8-interpreter
A Chip-8 interpreter written in C, for my own learning purposes.

## Dependencies

This interpreter uses SDL2 for graphics, (and eventually) sound and keyboard input.

## Cross-Platform Support

Tested and working on Windows 10 Home version 2004 (using MSYS2), as well as Debian 10.<br/>
Currently I have no plans to support macOS, due to a lack of testing hardware.

## Build System

This interpreter uses the CMake build system.<br/>
When building on Windows, an environment such as MSYS2 or Cygwin may be required.<br/>
This is because I configured CMake to search for SDL2 using FindPkgConfig, which may not be available on Windows.<br/>

## License

This interpreter is licensed under the BSD 3-Clause license, which can be viewed in the LICENSE file.
