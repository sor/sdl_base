#include "example_game.hpp"

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>

namespace JanSordid::SDL_Example
{
	void PlasmaState::Init()
	{
		Base::Init();

		_font = TTF_OpenFont( BasePath "asset/font/MonkeyIsland-1991-refined.ttf", _game.scalingFactor() * 16 );
		TTF_SetFontHinting( _font, TTF_HINTING_NONE );

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const Point resolution = windowSize / Scale;
		_plasmaSrf = SDL_CreateSurface( resolution.x, resolution.y, SDL_PIXELFORMAT_RGBA32 );

		_plasmaTex = SDL_CreateTexture( renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, resolution.x, resolution.y );
		// Set to smoothed rendering for the plasma texture
		SDL_SetTextureScaleMode( _plasmaTex, SDL_SCALEMODE_LINEAR );
	}

	void PlasmaState::Destroy()
	{
		SDL_DestroySurface( _plasmaSrf );
		SDL_DestroyTexture( _plasmaTex );
		_plasmaSrf = nullptr;
		_plasmaTex = nullptr;

		Base::Destroy();
	}

	bool PlasmaState::HandleEvent( const Event & event )
	{
		switch( event.type )
		{
			case SDL_EVENT_MOUSE_WHEEL:
				_brightness += event.wheel.y * 3;
				return true;
		}

		return false;
	}

	bool PlasmaState::Input()
	{
		// Is not supressed during ImGui input
		const bool * key_array = SDL_GetKeyboardState( nullptr );
		if( key_array[SDL_SCANCODE_DOWN] )
		{
			_brightness -= 1;
		}
		if( key_array[SDL_SCANCODE_UP] ) // Not an else-if, as both buttons can be held at the same time
		{
			_brightness += 1;
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

	void PlasmaState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		u8 *        px       = (u8 *)_plasmaSrf->pixels;
		const int   pitch    = _plasmaSrf->pitch;
		const int   h        = _plasmaSrf->h;
		const int   w        = _plasmaSrf->w;
		const float xy_scale = Scale * 0.015f;
		const float xy_frame = (float)framesSinceStart * 0.05f;
#pragma omp parallel
#pragma omp for
		for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w; ++x )
			{
				const f32 v      = plasma( (x - w / 2) * xy_scale, (y - h / 2) * xy_scale, xy_frame );
				const int offset = x * 4 + y * pitch;
				px[offset + 0] = std::max<int>( 0, std::min<int>( 255, 4 * _brightness * (.5f + .5f * sin( Numbers::pi_v<f32> * v )) + _brightness - 64 ) );
				px[offset + 1] = std::max<int>( 0, std::min<int>( 255, 4 * _brightness * (.5f + .5f * cos( Numbers::pi_v<f32> * v )) + _brightness - 64 ) );
				px[offset + 2] = std::max<int>( 0, std::min<int>( 255, 4 * _brightness - 255 ) );
				px[offset + 3] = 255;
			}
		}
	}

	void PlasmaState::Render( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaTNeeded )
	{
		// Draw the plasma
		{
			SDL_UpdateTexture( _plasmaTex, EntireRect, _plasmaSrf->pixels, _plasmaSrf->pitch );
			const FRect dst_rect { 0, 0, (f32)_plasmaSrf->w * Scale, (f32)_plasmaSrf->h * Scale };
			SDL_RenderTexture( renderer(), _plasmaTex, EntireFRect, &dst_rect );
		}

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const f32 offsetFromLeft = (30 * _game.scalingFactor());

		// Prepare the text as Texture
		if( _blendedText == nullptr )
		{
			constexpr const char * text =
				"Use mouse-wheel or [DOWN] and [UP] arrow keys\n"
				"   to change the brightness and colorization\n"
				"                  of the plasma effect!";

			if( _blendedText != nullptr )
				SDL_DestroyTexture( _blendedText );

			Owned<Surface> surf = TTF_RenderText_Blended_Wrapped( _font, text, 0, White, windowSize.x - 2 * offsetFromLeft );
			_blendedText = SDL_CreateTextureFromSurface( renderer(), surf );

			SDL_GetTextureSize( _blendedText, &_blendedTextSize.x, &_blendedTextSize.y );
		}

		// Draw the text on top of the plasma effect
		{
			const FPoint p = {
				offsetFromLeft,
				windowSize.y - (100 * _game.scalingFactor())
			};

			const f32 shadowOffsetFactor = (1.0f*_game.scalingFactor());
			//const int shadowOffsetFactor = (int)(2.0f*_game.scalingFactor()); // chonky shadow

			// Draw the shadow
			SDL_SetTextureColorMod( _blendedText, 0, 0, 0 );
			for( const Point & offset : HSNR64::ShadowOffset::Rhombus )
			{
				const FRect dstRect = FRect{ p.x, p.y, _blendedTextSize.x, _blendedTextSize.y } + (offset * shadowOffsetFactor);
				SDL_RenderTexture( renderer(), _blendedText, EntireFRect, &dstRect );
			}

			SDL_SetTextureColorMod( _blendedText, 255, 255, 255 );
			const FRect dstRect = p + FRect{ 0, 0, _blendedTextSize.x, _blendedTextSize.y };
			SDL_RenderTexture( renderer(), _blendedText, EntireFRect, &dstRect );
		}
	}
}
