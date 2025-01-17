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

#set( BENCHMARK_ENABLE_TESTING   OFF )
#set( SDL_3DNOW                  OFF )
#set( SDL_MMX                    OFF )
#set( SDL_OPENGLES               OFF )
#set( SDL2MIXER_MOD              OFF )
#set( SDL2MIXER_WAVPACK          OFF )

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
        $<$<CONFIG:DEBUG>:_DEBUG>               # not DEBUG because of a stupid old define
        $<$<CONFIG:RELWITHDEBINFO>:OPTIMIZED>
        $<$<CONFIG:RELEASE>:FINAL> )

if( USE_DeployBinary )
    add_compile_definitions( DEPLOY_BINARY  )
endif()

if( USE_NFD_Native )
    add_compile_definitions( NFD_NATIVE     )
endif()

if( USE_ImGUI )
    add_compile_definitions( IMGUI          )
endif()

if( WIN32 ) # Not only MSVC
    add_compile_definitions(
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
    )
endif()

if( GCC OR CLANG )
    add_compile_definitions(
            _GLIBCXX_USE_DEPRECATED=0
            _LIBCPP_ENABLE_DEPRECATION_WARNINGS=1
    )
endif()

add_executable( sor-pong
        src/sor/core.cpp
        src/sor/sdl_core.cpp

        src/example/pong.cpp
)
target_include_directories( sor-pong PUBLIC src/ )
target_link_libraries(      sor-pong PUBLIC

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
)

add_executable( sor-mini
        src/sor/core.cpp
        src/sor/sdl_core.cpp
        src/sor/sdl_game.cpp

        src/example/mini.cpp
)
target_include_directories( sor-mini PUBLIC src/ )
target_link_libraries(      sor-mini PUBLIC

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,       SDL2_ttf::SDL2_ttf,     SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,   SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)

add_executable( sor-example
        src/sor/core.cpp
        src/sor/sdl_core.cpp
        src/sor/sdl_game.cpp

        src/example/game/example_game.cpp

        src/example/game/introstate.cpp
        src/example/game/plasmastate.cpp
        src/example/game/sortstate.cpp
        src/example/game/camerastate.cpp
        src/example/game/shooterstate.cpp
        src/example/game/simpleeditor.cpp
)
target_include_directories( sor-example PUBLIC src/ )
target_link_libraries(      sor-example PUBLIC

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>

        #$<TARGET_NAME_IF_EXISTS:benchmark::benchmark>
        #$<TARGET_NAME_IF_EXISTS:benchmark::benchmark_main>

        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>

        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,               SDL2::SDL2,             SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,       SDL2_ttf::SDL2_ttf,     SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,   SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,   SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)
