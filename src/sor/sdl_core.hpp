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

	// SDL3 removed the official SDL_INIT_EVERYTHING, so we define it ourselves
	constexpr SDL_InitFlags SDL_INIT_EVERYTHING
		= SDL_INIT_AUDIO
		| SDL_INIT_VIDEO
		| SDL_INIT_HAPTIC
		| SDL_INIT_GAMEPAD
		| SDL_INIT_CAMERA
		| SDL_INIT_JOYSTICK
		| SDL_INIT_SENSOR
		| SDL_INIT_EVENTS;

	constexpr const SDL_Rect  * EntireRect  = nullptr;
	constexpr const SDL_FRect * EntireFRect = nullptr;

	constexpr const bool NoDecode  = false;
	constexpr const bool PreDecode = true;

	// SDL provides the timestamps, chrono the durations
	[[nodiscard]] static inline Duration now()  { return NanoSec{ SDL_GetTicksNS() }; }

	template<typename T, typename TDel = std::default_delete<T>> using Owned     = Core::AutocastUnique<T,TDel>;
	template<typename T>                                         using Shared    = Core::AutocastShared<T>;
	template<typename T>                                         using WeakShare = Core::AutocastWeakShare<T>;

	constexpr Core::u32 ToU32( const SDL_Color & c )
	{
		return (c.r << 0)
		     | (c.g << 8)
		     | (c.b << 16)
		     | (c.a << 24);
	}

	consteval SDL_Color ToColor( const Core::u32 & c )
	{
		// If consteval is not available, change to constexpr and comment in the following lines
		//if (!std::is_constant_evaluated())
		//	assert( false );
		return SDL_Color {
			(Core::u8)(c>>0 & 0xFF),
			(Core::u8)(c>>8 & 0xFF),
			(Core::u8)(c>>16 & 0xFF),
			(Core::u8)(c>>24 & 0xFF),
		};
	}

	inline bool SDL_SetRenderDrawColor( SDL_Renderer * renderer, const SDL_Color color )
	{
		return SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );
	}

}
