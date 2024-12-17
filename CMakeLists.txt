cmake_minimum_required(VERSION 3.14)

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" )

project( sdl_base LANGUAGES CXX )

set(CMAKE_CXX_STANDARD   20)
set(CMAKE_CXX_EXTENSIONS OFF)

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( IS_DEBUG TRUE )
else()
    set( IS_DEBUG FALSE )
endif()

option( USE_DeployBinary    "Deploy the binary in the base directory"   OFF )   # For distribution of final product
option( USE_FMT_HeaderOnly  "Use header-only implementation of FMT"     ON  )
option( USE_NFD_Native      "Use native encoding in NFD"                OFF )

option( USE_ImGUI           "Use ImGUI"         ${IS_DEBUG} )   # Can be manually enabled on Optimized and Final builds
option( USE_OpenMP          "Use OpenMP"        ON )    # No-brain SIMD and MP
option( USE_RTTI            "Use RTTI"          ON )    # Don't need dynamic_cast or type_id? Then don't pay for them
option( USE_Exceptions      "Use Exceptions"    ON )    # Don't need exceptions? Then don't pay for them

#cmake_dependent_option()

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

#include( AddDependency )

#set( BENCHMARK_ENABLE_TESTING   OFF )
#set( SDL_3DNOW                  OFF )
#set( SDL_MMX                    OFF )
#set( SDL_OPENGLES               OFF )
#set( SDL2MIXER_MOD              OFF )
#set( SDL2MIXER_WAVPACK          OFF )

#AddDependency( fmt          https://github.com/fmtlib/fmt.git                   11.0.2 )
#AddDependency( nfd          https://github.com/btzy/nativefiledialog-extended   v1.2.1 )
#AddDependency( benchmark    https://github.com/google/benchmark                 v1.9.0 )

#AddDependency( SDL2         https://github.com/libsdl-org/SDL.git               release-2.30.8 )
#AddDependency( SDL2_image   https://github.com/libsdl-org/SDL_image.git         release-2.8.2 )
#AddDependency( SDL2_mixer   https://github.com/libsdl-org/SDL_mixer.git         release-2.8.0 )
#AddDependency( SDL2_net     https://github.com/libsdl-org/SDL_net.git           release-2.2.0 )
#AddDependency( SDL2_ttf     https://github.com/libsdl-org/SDL_ttf.git           release-2.22.0 )

#AddDependency( imgui        https://github.com/ocornut/imgui.git                v1.91.3 )
#FetchContent_GetProperties(imgui)

find_package( SDL2       CONFIG REQUIRED )
find_package( SDL2_ttf   CONFIG REQUIRED )
find_package( SDL2_mixer CONFIG REQUIRED )
find_package( SDL2_image CONFIG REQUIRED )
find_package( SDL2_net   CONFIG REQUIRED )
find_package( imgui      CONFIG REQUIRED )
find_package( fmt        CONFIG REQUIRED )
find_package( nfd        CONFIG REQUIRED )
find_package( benchmark  CONFIG REQUIRED )
find_package( Eigen3     CONFIG REQUIRED )

# Why does MSVC exist, but not CLANG or GCC?
if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set( CLANG 1 )
elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    set( GCC 1 )
endif()

#add_compile_options(
#        "$<$<CONFIG:DEBUG>:-D_DEBUG>"   # not DEBUG because of a stupid old define
#        "$<$<CONFIG:RELWITHDEBINFO>:-DOPTIMIZED>"
#        "$<$<CONFIG:RELEASE>:-DFINAL>" )

add_compile_definitions(
        "$<$<CONFIG:DEBUG>:_DEBUG>"    # not DEBUG because of a stupid old define
        "$<$<CONFIG:RELWITHDEBINFO>:OPTIMIZED>"
        "$<$<CONFIG:RELEASE>:FINAL>" )

if( USE_DeployBinary )
    add_compile_options( "-DDEPLOY_BINARY"  )
endif()
if( USE_NFD_Native )
    add_compile_options( "-DNFD_NATIVE"     )
endif()

if( USE_ImGUI )
    add_compile_options( "-DIMGUI"          )
endif()

#[[
add_executable( fetch_sdl
        src/example/main.cpp

        src/sor/core.cpp
        src/sor/sdl_core.cpp
)
target_include_directories( fetch_sdl PUBLIC src/ )
target_link_libraries(      fetch_sdl PUBLIC

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark_main>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,       SDL2_ttf::SDL2_ttf,     SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,   SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)
]]

add_executable( sor-pong
        src/example/pong.cpp

        src/sor/core.cpp
        src/sor/sdl_core.cpp
)
target_include_directories( sor-pong PUBLIC src/ )
target_link_libraries(      sor-pong PUBLIC

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark_main>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,       SDL2_ttf::SDL2_ttf,     SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,   SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)

add_executable( sor-mini
        src/example/mini.cpp

        src/sor/core.cpp
        src/sor/sdl_core.cpp
        src/sor/sdl_game.cpp
)
target_include_directories( sor-mini PUBLIC src/ )
target_link_libraries(      sor-mini PUBLIC

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark_main>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,       SDL2_ttf::SDL2_ttf,     SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,   SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)

add_executable( sor-example
        src/example/game/example_game.cpp

        src/example/game/introstate.cpp
        src/example/game/plasmastate.cpp
        src/example/game/sortstate.cpp
        src/example/game/camerastate.cpp
        src/example/game/shooterstate.cpp

        src/sor/core.cpp
        src/sor/sdl_core.cpp
        src/sor/sdl_game.cpp
)
target_include_directories( sor-example PUBLIC src/ )
target_link_libraries(      sor-example PUBLIC

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark>
        $<TARGET_NAME_IF_EXISTS:benchmark::benchmark_main>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,       SDL2_ttf::SDL2_ttf,     SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,   SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)
