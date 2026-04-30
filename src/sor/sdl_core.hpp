#pragma once

#include "core.hpp"
#include "smartptr.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "sdl_smartptr.hpp"

// Additional convenience defines
#define BasePathFont    BasePathAsset "font/"
#define BasePathGraphic BasePathAsset "graphic/"
#define BasePathGUI     BasePathAsset "gui/"
#define BasePathMap     BasePathAsset "map/"
#define BasePathMusic   BasePathAsset "music/"
#define BasePathSound   BasePathAsset "sound/"


namespace JanSordid::SDL
{
	using namespace JanSordid::Core;

	constexpr SDL_InitFlags SDL_INIT_EVERYTHING
		= SDL_INIT_AUDIO
		| SDL_INIT_VIDEO
		| SDL_INIT_HAPTIC
		| SDL_INIT_GAMEPAD
		| SDL_INIT_CAMERA;

	/// Aliases of std::, SDL_, TTF_, Mix_ for easy usage

	constexpr const SDL_Rect  * EntireRect  = nullptr;
	constexpr const SDL_FRect * EntireFRect = nullptr;

	constexpr const bool NoDecode  = false;
	constexpr const bool PreDecode = true;

	// Types which need memory management
	using Window      = SDL_Window;
	using Texture     = SDL_Texture;
	using Renderer    = SDL_Renderer;
	using Surface     = SDL_Surface;
	using Font        = TTF_Font;
	using Text        = TTF_Text;
	using TextEngine  = TTF_TextEngine;
	using Mixer       = MIX_Mixer;
	using Audio       = MIX_Audio;
	using Track       = MIX_Track;

	// Usually not user controlled, do NOT use with smart pointers in the default case
	using PixelFormat = SDL_PixelFormat;
	using Palette     = SDL_Palette;

	// Value types
	using Color       = SDL_Color;
	using Event       = SDL_Event;
	using Point       = SDL_Point;
	using FPoint      = SDL_FPoint;
	using Vec         = SDL_FPoint;
	using Keymod      = SDL_Keymod;
	using Rect        = SDL_Rect;
	using FRect       = SDL_FRect;

	template<typename T, typename TDel = std::default_delete<T>> using Owned     = Core::AutocastUnique<T,TDel>;
	template<typename T>                                         using Shared    = Core::AutocastShared<T>;
	template<typename T>                                         using WeakShare = Core::AutocastWeakShare<T>;

	constexpr Core::u32 ToU32( const Color & c )
	{
		return (c.r << 0)
		     | (c.g << 8)
		     | (c.b << 16)
		     | (c.a << 24);
	}

	constexpr Color ToColor( const Core::u32 & c )
	{
		// TODO: I don't need it now and I am tired :D
		if (!std::is_constant_evaluated())
			assert( false );
		return Color {
			(Core::u8)(c>>0 & 0xFF),
			(Core::u8)(c>>8 & 0xFF),
			(Core::u8)(c>>16 & 0xFF),
			(Core::u8)(c>>24 & 0xFF),
		};
	}

	inline bool SDL_SetRenderDrawColor( Renderer * renderer, const Color color )
	{
		return SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );
	}

}
