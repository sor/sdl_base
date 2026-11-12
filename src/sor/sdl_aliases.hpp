#pragma once

namespace JanSordid
{
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
}
