# SDL Base

---

This project is a cross-platform modern C++ layer, adaptor and bootstrap
to get you started making your own games. It is meant to not solve too much stuff,
so that you can have some fun coding as well.

## Contents

Base-Classes **Game** and **GameState** to inherit in your own game.

Aliases, Macros, a consistent Style, Glue-Code to make SDLs Objects RAII capable,
a tile-based font and tile-based font rendering code, as well as other free assets
to get you started and a lot more.

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


## License

This library is distributed under the terms of the zlib license,
available in [LICENSE.txt](LICENSE.txt).

Cheers 🖖\
*~ Jan aka Sordid*
