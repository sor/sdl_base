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
#define BasePathMap     BasePathAsset "map/"
#define BasePathMusic   BasePathAsset "music/"
#define BasePathSound   BasePathAsset "sound/"


namespace JanSordid::SDL
{
	using namespace JanSordid::Core;

	constexpr SDL_InitFlags SDL_INIT_EVERYTHING
		= SDL_INIT_AUDIO
		| SDL_INIT_VIDEO
		| SDL_INIT_JOYSTICK
		| SDL_INIT_HAPTIC
		| SDL_INIT_GAMEPAD
		| SDL_INIT_EVENTS
		| SDL_INIT_SENSOR
		| SDL_INIT_CAMERA;

	/// Aliases of SDL_ for easy usage

	// Types which need memory management
	using Window      = SDL_Window;
	using Texture     = SDL_Texture;
	using Renderer    = SDL_Renderer;
	using Surface     = SDL_Surface;
	using Font        = TTF_Font;
	using Text        = TTF_Text;
	using TextEngine  = TTF_TextEngine;

	// This is still SDL2 style and will be changed once SDL3_mixer is officially released
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
