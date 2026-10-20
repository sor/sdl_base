# SDL Base

This project is a cross-platform modern C++ framework & bootstrap
to get you started making your own games. Primarily it uses **Simple DirectMedia Layer**
(short **SDL**) in Version **3**.
It is meant to not solve _too much_ stuff, so that you can _have fun_ coding as well.

This project is optimized to be used within **CLion**, but also works with
other IDEs or with just raw **CMake** on the command line.

Tested with C++20 compilers: **GCC** and **Clang**.
**MSVC** should work as well (multi-config generators are supported now),
but is not regularly validated.

## Getting Started

You need a C++20 compiler, **CMake** >= 3.20 and a **vcpkg** installation.
Best install **vcpkg** into the `CLionProjects/vcpkg/` folder, so it sits
next to this project (e.g. `CLionProjects/sdl_base/`) — that way no
configuration needs to be changed. From the project root:

    cmake -B build -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
    cmake --build build

All dependencies are pulled in automatically (see [vcpkg.json](vcpkg.json)).
Binaries land in `build/bin/` — run them from there, so the relative
asset paths resolve:

    cd build/bin
    ./sor-example

Build types map to the framework tiers:
**Debug**, **RelWithDebInfo** ("Optimized") and **Release** ("Final").
Optional features (TGUI, ImGui, RTTI, exceptions, ...) are CMake options,
see [CMakeLists.txt](CMakeLists.txt).

The demo binaries:
- **sor-example** — the big example game with all states (switch with keys 1-9)
- **sor-mini** — the minimal "hello world" to start your own game from
- **sor-pong** — a small standalone game without the framework
- **sor-smartptr** — showcase of the RAII / smart-pointer wrappers

## Contents

Base-Classes **Game** and **GameState** to inherit your own game from.
A tile-based font and tile-map rendering toolkit (`hsnr64`), free assets,
and a lot of example states as inspiration to copy & paste from.

The codebase speaks a small dialect to stay terse:
- Aliases for std types (`u8`..`u64`, `f32`/`f64`, `String`, `DynArray`, ...)
- `Owned<T>` / `Shared<T>` — RAII wrappers for SDL objects that call the right destroy function
- Build tiers: `Assert` (Debug), `AssertInOptimized` (Debug + Optimized),
  `DebugOnly` / `OptimizedOnly` / `FinalOnly`, `IfDebug` / `IfOptimized` / `IfFinal`, `ImGuiOnly`
- Attach assert messages with `&&`, never with a comma:
  `Assert( x > 0 && "x must be positive" )` —
  `Assert( x > 0, "message" )` silently discards the condition
  (and no longer compiles in Optimized/Final builds)

Two demo notes: state key **0** deliberately triggers the invalid-state path
(it asserts on purpose in Debug builds), and **F11** toggles the frame-time
graph (ImGui, Debug builds).

## External Contents

This project pulls in and enables **SDL3** with **SDL3_image**, **SDL3_ttf**
and **SDL3_mixer** (audio), **fmt** (fallback when `std::print` / `std::format`
are unavailable), the linear algebra library **Eigen**, cross-platform file
dialogs **NFD**, immediate mode GUI **Dear ImGui** with **ImPlot**, the widget
GUI **TGUI**, **nlohmann/json**, **range-v3**, **cxxopts**, Google's micro-
**benchmark** library, and **OpenMP** for the plasma effect.
See [vcpkg.json](vcpkg.json)

## Assets & Credits

- `hsnr64` tileset & font: by Jan Sordid
- Roboto Slab font: SIL Open Font License
- "MonkeyIsland-1991-refined" font: free fan remake
- Music `severance.ogg`: Karl Casey @ White Bat Audio (royalty-free)
- <!-- TODO: list the actual sources of the remaining graphics (bg-layers, fire/ice/poison, space_merc, ...) -->

## License

This library is distributed under the terms of the zlib license,
available in [LICENSE.txt](LICENSE.txt).

Cheers 🖖\
~ Jan aka Sordid
