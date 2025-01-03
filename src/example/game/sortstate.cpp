#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	void SortState::Init()
	{
		Base::Init();

		if( !image )
		{
			image = IMG_LoadTexture( renderer(), BasePath "asset/graphic/ball.png" );
		}

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		float iter = 0;
		const float y_center = (float) windowSize.y / 2.0f;

		balls.resize( 1000 );
		for( Ball & ball : balls )
		{
			ball = Ball {
				.x = iter * 40.f,
				.y = y_center + sin( iter / 2.f ) * (y_center / 4.0f),
				.z = 0,
				.w = iter * 0.6f,
			};
			iter += 1.0f;
		}
	}

	void SortState::Destroy()
	{
		if( image )
		{
			SDL_DestroyTexture( image );
			image = nullptr;
		}

		Base::Destroy();
	}

	bool SortState::HandleEvent( const Event & event )
	{
		if( event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F1 && event.key.repeat == 0 )
			isOrdered = !isOrdered;
		if( event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F2 && event.key.repeat == 0 )
			isDarkened = !isDarkened;
		if( event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_F3 && event.key.repeat == 0 )
			isTransparent = !isTransparent;

		return true; // Not really correct
	}

	void SortState::Update( const u64 frame, const u64 totalMSec, const f32 deltaT )
	{
		for( Ball & ball : balls )
		{
			ball.x -= deltaT * 40.f;
			ball.z = abs(sin( ball.w ));          // bounce from 0 to 1 (twice each revolution)
		//	ball.z = 0.5f + 0.5f * sin( ball.w ); // bob from 0 to 1
			ball.w += deltaT * 2.0f;
		}
	}

	void SortState::Render( const u64 frame, const u64 totalMSec, const f32 deltaTNeeded )
	{
		const u8 alpha = isTransparent ? 127 : 255;
		SDL_SetTextureAlphaMod( image, alpha );
		SDL_SetTextureColorMod( image, 255, 255, 255 );

		auto orderByZ = []( Ball & lhs, Ball & rhs )
		{
			return lhs.z < rhs.z;
		};

		if( isOrdered )
			std::sort( balls.begin(), balls.end(), orderByZ );

		for( const Ball & ball : balls )
		{
			const u8 size = (u8)(ball.z * 80.f + 48.f);
			if( isDarkened )
				SDL_SetTextureColorMod( image, size * 2 - 20, size * 2 - 20, size * 2 - 20 );
			const Rect dst_rect { (int)ball.x - (size / 2), (int)ball.y - (size), size, size };
			SDL_RenderCopy( renderer(), image, EntireRect, &dst_rect );
		}
	}
}
