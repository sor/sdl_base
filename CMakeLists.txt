cmake_minimum_required( VERSION 3.14 )

# Unused
#list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" )

project( sdl_base LANGUAGES CXX )

set( CMAKE_C_STANDARD               17  )
set( CMAKE_CXX_STANDARD             20  )
set( CMAKE_CXX_STANDARD_REQUIRED    ON  )
set( CMAKE_CXX_EXTENSIONS           OFF )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( IS_DEBUG TRUE )
else()
    set( IS_DEBUG FALSE )
endif()

option( USE_DeployBinary    "Deploy the binary in the base directory"    OFF )   # For distribution of final product
option( USE_FMT_HeaderOnly  "Use header-only implementation of FMT"      ON  )
option( USE_NFD_Native      "Use native encoding instead of UTF8 in NFD" OFF )

option( USE_ImGui           "Use Dear ImGui"          ${IS_DEBUG} )   # Can be manually enabled on Optimized and Final builds
option( USE_ImGuiFreetype   "Use Dear ImGui Freetype" OFF         )   # Usage of Freetype in ImGui
option( USE_OpenMP          "Use OpenMP"              ON          )   # No-brain SIMD and MP
option( USE_RTTI            "Use RTTI"                ON          )   # Don't need dynamic_cast or type_id? Then don't pay for them
option( USE_Exceptions      "Use Exceptions"          ON          )   # Don't need exceptions? Then don't pay for them

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

find_package( SDL2          CONFIG REQUIRED )
find_package( SDL2_ttf      CONFIG REQUIRED )
find_package( SDL2_mixer    CONFIG REQUIRED )
find_package( SDL2_image    CONFIG REQUIRED )
find_package( SDL2_net      CONFIG REQUIRED )
find_package( fmt           CONFIG REQUIRED )
find_package( nfd           CONFIG REQUIRED )
find_package( benchmark     CONFIG REQUIRED )
find_package( Eigen3        CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
if( USE_ImGUI )
    find_package( imgui     CONFIG REQUIRED )
endif()
if( USE_OpenMP )
    find_package( OpenMP           REQUIRED )
endif()


# Why does MSVC exist, but not CLANG or GCC?
if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set( CLANG 1 )
elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    set( GCC 1 )
endif()


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

if( USE_ImGui )
    add_compile_definitions( IMGUI          )

    if( USE_ImGuiFreetype )
        add_compile_definitions( IMGUI_ENABLE_FREETYPE )
    endif()
endif()


if( WIN32 ) # Not only MSVC, also Clang on Windows
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


add_compile_options(
        -Wall
        -Wextra
        -Werror=parentheses
        -Werror=return-type
        -Werror=sequence-point
        -Werror=uninitialized
        -Werror=unused-result
        -Werror=unused-value
        $<$<CXX_COMPILER_ID:Clang>:-Werror=comma>

        ## Soften up some general rules which are too harsh, with no-* options
        -Wno-unused-parameter                                                       # Base class functions can have plenty of unused parameters
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>>    # Do not strive for prehistoric compatibility

        $<$<COMPILE_LANGUAGE:CXX>:-Werror=deprecated-copy>
        $<$<COMPILE_LANGUAGE:CXX>:-Werror=deprecated-copy-dtor>
        $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>

        # Outlaw variable shadowing
        $<$<CXX_COMPILER_ID:GNU>:-Wshadow>
        $<$<CXX_COMPILER_ID:GNU>:-Werror=shadow=local>
        $<$<CXX_COMPILER_ID:Clang>:-Wshadow-all>
        $<$<CXX_COMPILER_ID:Clang>:-Werror=shadow>

        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wno-volatile>>      # Senseless C++20 deprecation that has been de-deprecated for C++23, works in GCC 11 but not Clang 15

        $<$<COMPILE_LANGUAGE:CXX>:-Wdelete-non-virtual-dtor>
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wdeprecated-dynamic-exception-spec>>
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wdeprecated-implementations>>

        #$<$<CXX_COMPILER_ID:Clang>:-Wno-unknown-attributes>                    # Clang 15 does not know [[(un)likely]], or does it?

        $<$<COMPILE_LANGUAGE:CXX>:-Werror=suggest-override>
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wweak-vtables>>

        # Extend constexpr recursion depth
        $<$<COMPILE_LANGUAGE:CXX>:-fconstexpr-depth=127>
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=127124200>>
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=127124200>>
        # MSVC /constexpr:steps 127124200

        # Get messages from the vectorizer
        #$<$<CXX_COMPILER_ID:Clang>:-Rpass=loop-vectorize>
)

if( NOT USE_RTTI )
    #add_compile_options( /GR- ) # MSVC?
    add_compile_options( $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> )
endif()

if( NOT USE_Exceptions )
    #add_compile_options( "-D_HAS_EXCEPTIONS=0" ) # MSVC?
    #add_compile_options( "-D_STATIC_CPPLIB" )
    add_compile_options( $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> )
endif()

# Enabled before releasing final product
if( USE_DeployBinary )
    add_compile_options( -Werror=unused-variable )
endif()

# Silence unknown OpenMP pragmas
if( NOT USE_OpenMP )
    add_compile_options( -Wno-unknown-pragmas )
endif()

# Only build for this machine
if( MaximumOptimize )
    add_compile_options( -march=native )
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
        src/sor/adapt_sdl_nfd.cpp
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
        src/sor/adapt_sdl_nfd.cpp
        src/sor/sdl_core.cpp
        src/sor/sdl_game.cpp

        src/example/game/example_game.cpp

        src/example/game/introstate.cpp
        src/example/game/plasmastate.cpp
        src/example/game/sortstate.cpp
        src/example/game/camerastate.cpp
        src/example/game/shooterstate.cpp
        src/example/game/simpleeditor.cpp
        src/example/game/roflstate.cpp
)
target_include_directories( sor-example PUBLIC src/ )
target_link_libraries(      sor-example
    PUBLIC

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>
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

    PRIVATE
        $<TARGET_NAME_IF_EXISTS:nlohmann_json::nlohmann_json>

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>
)
