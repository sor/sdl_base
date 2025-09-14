#pragma once

// Usually ImGui is only included/enabled during development,
// but toggling USE_ImGUI to ON in CMake can make it available everywhere
#if defined( IMGUI )
	#include <imgui.h>      // https://github.com/ocornut/imgui/blob/bb224c8aa1de1992c6ea3483df56fb04d6d1b5b6/examples/example_sdl2_sdlrenderer/main.cpp
	#include <imgui_impl_sdl2.h>
	#include <imgui_impl_sdlrenderer2.h>

	#define ImGuiOnly( ... )    __VA_ARGS__
	#define IfImGui             if constexpr( true )
#else
	#define ImGuiOnly( ... )
	#define IfImGui             if constexpr( false )
#endif
