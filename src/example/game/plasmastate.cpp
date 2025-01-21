#include "example_game.hpp"

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>

namespace JanSordid::SDL_Example
{
	void PlasmaState::Init()
	{
		Base::Init();

		font = TTF_OpenFont( BasePath "asset/font/MonkeyIsland-1991-refined.ttf", _game.scalingFactor() * 16 );
		TTF_SetFontHinting( font, TTF_HINTING_NONE );

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const Point resolution = windowSize / Scale;
		plasmaSrf = SDL_CreateRGBSurfaceWithFormat( 0, resolution.x, resolution.y, 32, SDL_PIXELFORMAT_RGBA32 );

		// Set to smoothed rendering for the plasma texture
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "best" );
		plasmaTex = SDL_CreateTexture( renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, resolution.x, resolution.y );
		// Reset to "pixelated" for further textures
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "nearest" );
	}

	void PlasmaState::Destroy()
	{
		SDL_FreeSurface( plasmaSrf );
		SDL_DestroyTexture( plasmaTex );
		plasmaSrf = nullptr;
		plasmaTex = nullptr;

		Base::Destroy();
	}

	bool PlasmaState::HandleEvent( const Event & event )
	{
		switch( event.type )
		{
			case SDL_MOUSEWHEEL:
				brightness += event.wheel.y * 3;
				return true;
		}

		return false;
	}

	bool PlasmaState::Input()
	{
		// Is not supressed during ImGui input
		const u8 * key_array = SDL_GetKeyboardState( nullptr );
		if( key_array[SDL_SCANCODE_DOWN] )
		{
			brightness -= 1;
		}
		if( key_array[SDL_SCANCODE_UP] ) // Not an else-if, as both buttons can be held at the same time
		{
			brightness += 1;
		}

		return false;
	}

	static inline float plasma( const f32 x, const f32 y, const f32 time )
	{
		const float cx = x * 0.1f + 0.5f * sin( time / 5.0f );
		const float cy = y * 0.1f + 0.5f * cos( time / 3.0f );
		float v = 0.0f;
		v += sin( (x + time) );
		v += sin( (y + time) / 2.0f );
		v += sin( (x + y + time) / 2.0f );
		v += sin( sqrt( (cx * cx + cy * cy) * 100.0f + 1.0f ) + time );
		v = v / 2.0f;
		return v;
	}

	void PlasmaState::Update( const u64 frame, const u64 totalMSec, const f32 deltaT )
	{
		u8 *        px       = (u8 *)plasmaSrf->pixels;
		const int   pitch    = plasmaSrf->pitch;
		const int   h        = plasmaSrf->h;
		const int   w        = plasmaSrf->w;
		const float xy_scale = Scale * 0.015f;
		const float xy_frame = (float)frame * 0.05f;
#pragma omp parallel
#pragma omp for
		for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w; ++x )
			{
				const f32 v      = plasma( (x - w / 2) * xy_scale, (y - h / 2) * xy_scale, xy_frame );
				const int offset = x * 4 + y * pitch;
				px[offset + 0] = std::max<int>( 0, std::min<int>( 255, 4 * brightness * (.5f + .5f * sin( Numbers::pi_v<f32> * v )) + brightness - 64 ) );
				px[offset + 1] = std::max<int>( 0, std::min<int>( 255, 4 * brightness * (.5f + .5f * cos( Numbers::pi_v<f32> * v )) + brightness - 64 ) );
				px[offset + 2] = std::max<int>( 0, std::min<int>( 255, 4 * brightness - 255 ) );
				px[offset + 3] = 255;
			}
		}
	}

	void PlasmaState::Render( const u64 frame, const u64 totalMSec, const f32 deltaT )
	{
		// Draw the plasma
		{
			SDL_UpdateTexture( plasmaTex, EntireRect, plasmaSrf->pixels, plasmaSrf->pitch );
			const Rect dst_rect { 0, 0, plasmaSrf->w * Scale, plasmaSrf->h * Scale };
			SDL_RenderCopy( renderer(), plasmaTex, EntireRect, &dst_rect );
		}

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const int offsetFromLeft = (int)(30 * _game.scalingFactor());

		// Prepare the text as Texture
		if( blendedText == nullptr )
		{
			constexpr const char * text =
				"Use mouse-wheel or [DOWN] and [UP] arrow keys\n"
				"   to change the brightness and colorization\n"
				"                  of the plasma effect!";

			if( blendedText != nullptr )
				SDL_DestroyTexture( blendedText );

			constexpr const Color white = HSNR64::Palette( HSNR64::NamedColorIndex::White );
			Surface * surf = TTF_RenderUTF8_Blended_Wrapped( font, text, white, windowSize.x - 2 * offsetFromLeft );
			blendedText = SDL_CreateTextureFromSurface( renderer(), surf );
			SDL_FreeSurface( surf );

			u32 fmt;
			int access;
			SDL_QueryTexture( blendedText, &fmt, &access, &blendedTextSize.x, &blendedTextSize.y );
		}

		// Draw the text on top of the plasma effect
		{
			const Point p {
				offsetFromLeft,
				windowSize.y - (int)(100 * _game.scalingFactor())
			};

			const int shadowOffsetFactor = (int)(1.0f*_game.scalingFactor());
			//const int shadowOffsetFactor = (int)(2.0f*_game.scalingFactor()); // chonky shadow

			// Draw the shadow
			SDL_SetTextureColorMod( blendedText, 0, 0, 0 );
			for( const Point & offset : HSNR64::ShadowOffset::Rhombus )
			{
				const Rect dst_rect = Rect{ p.x, p.y, blendedTextSize.x, blendedTextSize.y } + (offset * shadowOffsetFactor);
				SDL_RenderCopy( renderer(), blendedText, EntireRect, &dst_rect );
			}

			SDL_SetTextureColorMod( blendedText, 255, 255, 255 );
			const Rect dst_rect = p + Rect{ 0, 0, blendedTextSize.x, blendedTextSize.y };
			SDL_RenderCopy( renderer(), blendedText, EntireRect, &dst_rect );
		}
	}
}
