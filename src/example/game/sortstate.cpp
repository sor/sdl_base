#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	void SortState::Init()
	{
		Base::Init();

		if( !_image )
		{
			_image = IMG_LoadTexture( renderer(), BasePath "asset/graphic/ball.png" );
		}

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		f32 iter = 0;
		const f32 y_center = (f32)windowSize.y / 2.0f;

		_balls.resize( 1000 );
		for( Ball & ball : _balls )
		{
			ball = Ball {
				.x = iter * 40.f,
				.y = y_center + (f32)sin( iter / 2.f ) * (y_center / 4.0f),
				.z = 0,
				.w = iter * 0.6f,
			};
			iter += 1.0f;
		}
	}

	void SortState::Destroy()
	{
		if( _image )
		{
			SDL_DestroyTexture( _image );
			_image = nullptr;
		}

		Base::Destroy();
	}

	bool SortState::HandleEvent( const Event & event )
	{
		if( event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_F1 && event.key.repeat == 0 )
			_isOrdered = !_isOrdered;
		if( event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_F2 && event.key.repeat == 0 )
			_isDarkened = !_isDarkened;
		if( event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_F3 && event.key.repeat == 0 )
			_isTransparent = !_isTransparent;

		return true; // Not really correct
	}

	void SortState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		for( Ball & ball : _balls )
		{
			ball.x -= deltaT * 40.f;
			ball.z = abs( sin( ball.w ) );          // bounce from 0 to 1 (twice each revolution)
		//	ball.z = 0.5f + 0.5f * sin( ball.w ); // bob from 0 to 1
			ball.w += deltaT * 2.0f;
		}
	}

	void SortState::Render( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaTNeeded )
	{
		const u8 alpha = _isTransparent ? 127 : 255;
		SDL_SetTextureAlphaMod( _image, alpha );
		SDL_SetTextureColorMod( _image, 255, 255, 255 );

		auto orderByZ = []( Ball & lhs, Ball & rhs )
		{
			return lhs.z < rhs.z;
		};

		if( _isOrdered )
			std::sort( _balls.begin(), _balls.end(), orderByZ );

		for( const Ball & ball : _balls )
		{
			const u8 size = (u8)(ball.z * 80.f + 48.f);
			if( _isDarkened )
				SDL_SetTextureColorMod( _image, size * 2 - 20, size * 2 - 20, size * 2 - 20 );
			const FRect dst_rect { ball.x - (size / 2), ball.y - (size), (f32)size, (f32)size };
			SDL_RenderTexture( renderer(), _image, EntireFRect, &dst_rect );
		}
	}
}
