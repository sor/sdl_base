
# SDL Base

This project is a cross-platform modern C++ framework & bootstrap
to get you started making your own games. Primarily it uses **Simple DirectMedia Layer**
(short **SDL**) in the brand new Version **3**.
It is meant to not solve _too much_ stuff, so that you can _have fun_ coding as well.

This project is optimized to be used within **CLion**, but should also work with
other IDEs or with just raw **CMake** on the command line.

Tested with C++20 compilers: **GCC**, **Clang** and **MSVC**
(MSVC needs to be validated again).

## Contents

Base-Classes **Game** and **GameState** to inherit your own game from.

Aliases, Macros, a consistent Style, Glue-Code to make SDLs Objects RAII capable,
a tile-based font and tile-based font rendering code, as well as other free assets
to get you started and **a lot more**.

There are also a lot of Examples as inspiration you, as well to copy & paste stuff.


## External Contents

This project pulls in and enables **SDL3** with associated Libraries
for **Font**, **Image** & **Audio** loading & handling
(Audio still uses the API from SDL2),
as well as modern stdio / iostream alternative **FMT**,
the linear algebra library **Eigen**,
cross plattform file dialogs **NFD**,
simple immediate mode GUI library **Dear ImGui**,
an easy-to-use **JSON** library,
and micro-**benchmark** library from Google.
See [vcpkg.json](vcpkg.json)

## License

This library is distributed under the terms of the zlib license,
available in [LICENSE.txt](LICENSE.txt).

Cheers 🖖\
~ Jan aka Sordid
