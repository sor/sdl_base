#pragma once

#include "core.hpp"
#include "smartptr.hpp"
#include "sdl_smartptr.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

// Usually ImGui is only included/enabled during development,
// but toggling USE_ImGUI to ON in CMake can make it available everywhere
#if defined( IMGUI )

	//#define IMGUI_ENABLE_FREETYPE

	#include <imgui.h>      // https://github.com/ocornut/imgui/blob/bb224c8aa1de1992c6ea3483df56fb04d6d1b5b6/examples/example_sdl2_sdlrenderer/main.cpp
	#include <imgui_impl_sdl2.h>
	#include <imgui_impl_sdlrenderer2.h>

	#define ImGuiOnly( ... )    __VA_ARGS__
	#define IfImGui             if constexpr( true )
#else
	#define ImGuiOnly( ... )    // nothing
	#define IfImGui             if constexpr( false )
#endif

// Additional convenience defines
#define BasePathFont    BasePath "asset/font/"
#define BasePathGraphic BasePath "asset/graphic/"
#define BasePathMap     BasePath "asset/map/"
#define BasePathMusic   BasePath "asset/music/"
#define BasePathSound   BasePath "asset/sound/"

// Monkey-patching NFD
namespace NFD
{
	// Usually passed by value (contrary to SDL_Window)
	using Window = nfdwindowhandle_t;

	// SDL_Window as pointer because it's the case for all other functions as well, else by reference would be preferred
	Window GetNativeWindowFromSDLWindow( SDL_Window * sdlWindow );
}

namespace JanSordid::SDL
{
	using namespace JanSordid::Core;

	/// Aliases of SDL_ for easy usage

	// Types which need memory management
	using Window      = SDL_Window;
	using Texture     = SDL_Texture;
	using Renderer    = SDL_Renderer;
	using Surface     = SDL_Surface;
	using Font        = TTF_Font;
	using Chunk       = Mix_Chunk;
	using Music       = Mix_Music;

	// Usually not user controlled, do NOT use with smart pointers in the default case
	using PixelFormat = SDL_PixelFormat;
	using Palette     = SDL_Palette;

	// Value types
	using Color       = SDL_Color;
	using Event       = SDL_Event;
	using Point       = SDL_Point;
	using FPoint      = SDL_FPoint;
	using Keysym      = SDL_Keysym;
	using Keymod      = SDL_Keymod;
	using Rect        = SDL_Rect;
	using FRect       = SDL_FRect;

	template<typename T, typename TDel = std::default_delete<T>> using Owned     = AutocastUnique<T,TDel>;
	template<typename T>                                         using Shared    = AutocastShared<T>;
	template<typename T>                                         using WeakShare = AutocastWeakShare<T>;

	constexpr JanSordid::Core::u32 ToU32( const Color & c )
	{
		return (c.r << 0)
		     | (c.g << 8)
		     | (c.b << 16)
		     | (c.a << 24);
	}

	constexpr Color ToColor( const JanSordid::Core::u32 & c )
	{
		// TODO: I don't need it now and I am tired :D
		if (!std::is_constant_evaluated())
			assert( false );
		return Color {
			(JanSordid::Core::u8)(c>>0 & 0xFF),
			(JanSordid::Core::u8)(c>>8 & 0xFF),
			(JanSordid::Core::u8)(c>>16 & 0xFF),
			(JanSordid::Core::u8)(c>>24 & 0xFF),
		};
	}
}
