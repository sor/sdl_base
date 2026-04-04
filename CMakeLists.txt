cmake_minimum_required( VERSION 3.14 )

# Unused
#list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" )

project( sdl_base LANGUAGES CXX )

set( CMAKE_C_STANDARD               17  )
set( CMAKE_CXX_STANDARD             20  )
set( CMAKE_CXX_STANDARD_REQUIRED    ON  )
set( CMAKE_CXX_EXTENSIONS           OFF )
set( BUILD_SHARED_LIBS              ON  )
set( BUILD_STATIC_LIBS              OFF )

set( SDL_SHARED ON  CACHE BOOL "" )
set( SDL_STATIC OFF CACHE BOOL "" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( IS_DEBUG TRUE )
else()
    set( IS_DEBUG FALSE )
endif()

option( USE_DeployBinary    "Deploy the binary in the base directory"    OFF )   # For distribution of final product
option( USE_FMT_HeaderOnly  "Use header-only implementation of FMT"      ON  )
option( USE_NFD_Native      "Use native encoding instead of UTF8 in NFD" OFF )

option( USE_TGUI            "Use Texus' GUI"          ON          )   # Simple Embedding GUI Framework
option( USE_ImGui           "Use Dear ImGui"          ${IS_DEBUG} )   # Can be manually enabled on Optimized and Final builds
option( USE_ImGuiFreetype   "Use Dear ImGui Freetype" OFF         )   # Usage of Freetype in ImGui
option( USE_OpenMP          "Use OpenMP"              ON          )   # No-brain SIMD and MP
option( USE_RTTI            "Use RTTI"                ON          )   # Don't need dynamic_cast or type_id? Then don't pay for them
option( USE_Exceptions      "Use Exceptions"          ON          )   # Don't need exceptions? Then don't pay for them
option( USE_TargetNative    "Use march=native"        OFF         )   # Don't need to run this somewhere else? Build for THIS machine only

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

find_package( SDL3          CONFIG REQUIRED )
find_package( SDL3_image    CONFIG REQUIRED )
find_package( SDL3_ttf      CONFIG REQUIRED )
find_package( SDL3_mixer    CONFIG REQUIRED )
#find_package( SDL2_net      CONFIG REQUIRED ) # There is no SDL3_net yet...
find_package( fmt           CONFIG REQUIRED )
find_package( nfd           CONFIG REQUIRED )
find_package( benchmark     CONFIG REQUIRED )
find_package( Eigen3        CONFIG REQUIRED )
find_package( nlohmann_json CONFIG REQUIRED )
if( USE_TGUI )
    find_package( TGUI      CONFIG REQUIRED )
endif()
if( USE_ImGui )
    find_package( imgui     CONFIG REQUIRED )
    find_package( implot    CONFIG REQUIRED )
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

# SDL OLD NAMES
#add_compile_definitions( SDL_DISABLE_OLD_NAMES )

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

if( GCC OR CLANG )
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

        -Werror=array-bounds                                # Out-of-bounds array access
        -Werror=format                                      # printf/scanf format string mismatches
        -Werror=format-security                             # Dangerous format string usage
        -Werror=null-dereference                            # Potential NULL pointer dereferences
        $<$<CXX_COMPILER_ID:GNU>:-Werror=stringop-overflow> # Buffer overflows in string operations

        $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration> # Using functions without declaration
        $<$<COMPILE_LANGUAGE:C>:-Werror=incompatible-pointer-types>    # Assigning incompatible pointer types
        $<$<COMPILE_LANGUAGE:C>:-Werror=int-conversion>                # Implicit conversion between int and pointer

        -Werror=misleading-indentation           # Indentation suggests different control flow
        -Werror=dangling-else                    # Ambiguous else clauses
        -Werror=switch                           # Switch statement issues (missing break, etc)
        -Wswitch-enum                            # Switch on enum missing cases
        $<$<CONFIG:RELEASE>:-Werror=switch-enum> # Switch on enum missing cases

        $<$<CXX_COMPILER_ID:GNU>:-Werror=div-by-zero>                             # Compile-time division by zero
        $<$<CXX_COMPILER_ID:Clang>:-Werror=division-by-zero>                      # Compile-time division by zero
        $<$<CXX_COMPILER_ID:Clang>:-Werror=missing-braces>                        # Missing braces in initializers
        $<$<COMPILE_LANGUAGE:C>:$<$<CXX_COMPILER_ID:GNU>:-Werror=missing-braces>> # Missing braces in initializers
        -Werror=sizeof-pointer-memaccess                                          # sizeof on pointer when array intended

        ## Soften up some general rules which are too harsh, with no-* options
        -Wno-unused-parameter                                                       # Base class functions can have plenty of unused parameters
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>>    # Do not strive for prehistoric compatibility
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wno-volatile>>          # Senseless C++20 deprecation that has been de-deprecated for C++23, works in GCC 11 but not Clang 15
        -Wno-unused-parameter                                                     # Base class functions can have plenty of unused parameters
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat>>  # Do not strive for prehistoric compatibility
        $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wno-volatile>>        # Senseless C++20 deprecation that has been de-deprecated for C++23, works in GCC 11 but not Clang 15

        $<$<COMPILE_LANGUAGE:CXX>:-Werror=deprecated-copy>
        $<$<COMPILE_LANGUAGE:CXX>:-Werror=deprecated-copy-dtor>
        $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>

        # Outlaw variable shadowing
        $<$<CXX_COMPILER_ID:GNU>:-Wshadow>
        $<$<CXX_COMPILER_ID:GNU>:-Werror=shadow=local>
        $<$<CXX_COMPILER_ID:Clang>:-Wshadow-all>
        $<$<CXX_COMPILER_ID:Clang>:-Werror=shadow>

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

        # Get messages from the vectorizer
        #$<$<CXX_COMPILER_ID:Clang>:-Rpass=loop-vectorize>
    )
else()
    add_compile_options(
            /W4

            /we4706   # assignment within conditional (parentheses)
            /we4715   # not all control paths return a value (return-type)
            /we4717   # recursive on all control paths (return-type related)
            /we4552   # operator has no effect (unused-value)
            /we4553   # operator has no effect; did you intend '!='? (comma operator misuse)
            /we4700   # uninitialized local variable used
            #/we6031   # return value ignored (unused-result, requires /analyze)

            /we4013   # implicit function declaration
            /we4047   # different levels of indirection (pointer type mismatch)
            /we4024   # different types for formal and actual parameter
            /we4013   # function undefined; assuming extern returning int
            /we4716   # must return a value

            /constexpr:steps 127124200
    )
endif()


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
if( USE_TargetNative )
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

        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>

        $<IF:$<TARGET_EXISTS:SDL3::SDL3>,               SDL3::SDL3,             SDL3::SDL3-static>
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,   SDL3_image::SDL3_image, SDL3_image::SDL3_image-static>
)


add_executable( sor-smartptr
        src/sor/core.cpp
        src/sor/sdl_core.cpp

        src/example/smartptrtest.cpp
)
target_include_directories( sor-smartptr PUBLIC src/ )
target_link_libraries(      sor-smartptr PUBLIC

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>

        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>

        $<IF:$<TARGET_EXISTS:SDL3::SDL3>,               SDL3::SDL3,             SDL3::SDL3-static>
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,   SDL3_image::SDL3_image, SDL3_image::SDL3_image-static>
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

        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>

        $<IF:$<TARGET_EXISTS:SDL3::SDL3>,               SDL3::SDL3,             SDL3::SDL3-static>
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,   SDL3_image::SDL3_image, SDL3_image::SDL3_image-static>
        $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,       SDL3_ttf::SDL3_ttf,     SDL3_ttf::SDL3_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL3_mixer::SDL3_mixer>,   SDL3_mixer::SDL3_mixer, SDL3_mixer::SDL3_mixer-static>
        #$<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
        $<TARGET_NAME_IF_EXISTS:implot::implot>
)


add_executable( sor-example
        src/sor/core.cpp
        src/sor/sdl_core.cpp
        src/sor/sdl_game.cpp

        src/sor/component/perftrack.cpp
        src/sor/component/perftrack.hpp

        src/hsnr64/tilefont.cpp
        src/hsnr64/tiles.cpp

        src/example/game/example_game.cpp

        src/example/game/introstate.cpp
        src/example/game/plasmastate.cpp
        src/example/game/sortstate.cpp
        src/example/game/camerastate.cpp
        src/example/game/shooterstate.cpp
        src/example/game/simpleeditor.cpp
        src/example/game/roflstate.cpp
        src/example/game/mapeditor.cpp
        src/example/game/tgui.cpp
)
target_include_directories( sor-example PUBLIC src/ )
target_link_libraries(      sor-example PUBLIC

        $<IF:$<BOOL:${USE_FMT_HeaderOnly}>, fmt::fmt-header-only, fmt::fmt>

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>
        $<TARGET_NAME_IF_EXISTS:nfd::nfd>

        #$<TARGET_NAME_IF_EXISTS:benchmark::benchmark>
        #$<TARGET_NAME_IF_EXISTS:benchmark::benchmark_main>

        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>

        $<IF:$<TARGET_EXISTS:SDL3::SDL3>,               SDL3::SDL3,             SDL3::SDL3-static>
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,   SDL3_image::SDL3_image, SDL3_image::SDL3_image-static>
        $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,       SDL3_ttf::SDL3_ttf,     SDL3_ttf::SDL3_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL3_mixer::SDL3_mixer>,   SDL3_mixer::SDL3_mixer, SDL3_mixer::SDL3_mixer-static>
        #$<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,       SDL2_net::SDL2_net,     SDL2_net::SDL2_net-static>

        $<TARGET_NAME_IF_EXISTS:Eigen3::Eigen>

        $<TARGET_NAME_IF_EXISTS:TGUI::TGUI>

        $<TARGET_NAME_IF_EXISTS:imgui::imgui>
        $<TARGET_NAME_IF_EXISTS:implot::implot>

    PRIVATE
        $<TARGET_NAME_IF_EXISTS:nlohmann_json::nlohmann_json>

        $<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>
)
