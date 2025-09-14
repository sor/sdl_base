#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	void CameraState::Init()
	{
		Base::Init();

		if( !_background[0] )
		{
			//SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "best" );

			_background[0] = IMG_LoadTexture( renderer(), BasePathGraphic "bg-layer-4.png" );
			_background[1] = IMG_LoadTexture( renderer(), BasePathGraphic "bg-layer-3.png" );
			_background[2] = IMG_LoadTexture( renderer(), BasePathGraphic "bg-layer-2.png" );
			_background[3] = IMG_LoadTexture( renderer(), BasePathGraphic "bg-layer-1.png" );

			SDL_QueryTexture( _background[0], nullptr, nullptr, &_backgroundSize[0].x, &_backgroundSize[0].y );
			SDL_QueryTexture( _background[1], nullptr, nullptr, &_backgroundSize[1].x, &_backgroundSize[1].y );
			SDL_QueryTexture( _background[2], nullptr, nullptr, &_backgroundSize[2].x, &_backgroundSize[2].y );
			SDL_QueryTexture( _background[3], nullptr, nullptr, &_backgroundSize[3].x, &_backgroundSize[3].y );

			SDL_SetTextureColorMod( _background[0], 163, 163, 163 );
			SDL_SetTextureColorMod( _background[1], 191, 191, 191 );
			SDL_SetTextureColorMod( _background[2], 191, 191, 191 );
			SDL_SetTextureColorMod( _background[3], 225, 225, 255 );

			SDL_SetTextureAlphaMod( _background[2], 210 );
			SDL_SetTextureAlphaMod( _background[3], 127 );

			SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "nearest" );
		}

		// Reinit on reenter
		_camera = { .x = 0, .y = 0 };
	}

	void CameraState::Destroy()
	{
		// TODO

		Base::Destroy();
	}

	bool CameraState::HandleEvent( const Event & event )
	{
		if( event.type == SDL_KEYDOWN && event.key.repeat == 0 )
		{
			if( event.key.keysym.scancode == SDL_SCANCODE_F1 ) _isBackgroundVisible[0] = !_isBackgroundVisible[0];
			if( event.key.keysym.scancode == SDL_SCANCODE_F2 ) _isBackgroundVisible[1] = !_isBackgroundVisible[1];
			if( event.key.keysym.scancode == SDL_SCANCODE_F3 ) _isBackgroundVisible[2] = !_isBackgroundVisible[2];
			if( event.key.keysym.scancode == SDL_SCANCODE_F4 ) _isBackgroundVisible[3] = !_isBackgroundVisible[3];

			// Toggle all
			if( event.key.keysym.scancode == SDL_SCANCODE_F5 )
				_isBackgroundVisible[0] = _isBackgroundVisible[1] = _isBackgroundVisible[2] = _isBackgroundVisible[3] = !_isBackgroundVisible[0];

			if( event.key.keysym.scancode == SDL_SCANCODE_F6 ) _isInverted = !_isInverted;
			if( event.key.keysym.scancode == SDL_SCANCODE_F7 ) _isEased    = !_isEased;
			if( event.key.keysym.scancode == SDL_SCANCODE_F8 ) _isFlux     = !_isFlux;

			return true; // Not 100% correct
		}
		else if( (event.type == SDL_MOUSEBUTTONDOWN)
			  || (event.type == SDL_MOUSEMOTION && event.motion.state != 0) )
		{
			Point windowSize;
			SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

			const FPoint halfWinSize = toF( windowSize / 2 );
			const FPoint mousePos    = { (float) event.motion.x, (float) event.motion.y };

			_mouseOffset = _isInverted
				? halfWinSize - mousePos
				: mousePos - halfWinSize;

			return true;
		}
		else if( event.type == SDL_MOUSEBUTTONUP )
		{
			_mouseOffset = {0, 0 };

			return true;
		}
		else
		{
			return false;
		}
	}

	bool CameraState::Input()
	{
		const u8 *  key_array = SDL_GetKeyboardState( nullptr );
		const float factor    = key_array[SDL_SCANCODE_RSHIFT]
			? 600.0f
			: 80.0f;

		// Reset direction
		_direction = { 0, 0 };
		if( key_array[SDL_SCANCODE_LEFT]  ) _direction.x += factor;	// No deltaT here, will be applied in Update
		if( key_array[SDL_SCANCODE_RIGHT] ) _direction.x -= factor;

		if( key_array[SDL_SCANCODE_UP]    ) _direction.y += factor;
		if( key_array[SDL_SCANCODE_DOWN]  ) _direction.y -= factor;

		return false;
	}

	void CameraState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		_camera += _direction * deltaT;

		if( _isEased )
		{
			SDL_FPoint diff = (_mouseOffset - _mouseOffsetEased);

			//constexpr const float thresh = 20.0f; // 1.2f;
			//if( -thresh < diff.x && diff.x < thresh ) diff.x = 0;
			//if( -thresh < diff.y && diff.y < thresh ) diff.y = 0;

			//constexpr const float thresh = 2.2f;
			//if( -thresh < diff.x && diff.x < thresh ) { mouseOffsetEased.x = mouseOffset.x; diff.x = 0; }
			//if( -thresh < diff.y && diff.y < thresh ) { mouseOffsetEased.y = mouseOffset.y; diff.y = 0; }

			_mouseOffsetEased += diff * max( 0.1f, min( 0.4f, deltaT * 10.0f ) );
		}
		else
		{
			_mouseOffsetEased = _mouseOffset;
		}
	}

	FPoint CameraState::CalcFluxCam( const u64 msSinceStart ) const
	{
		const FPoint flux = _isFlux
			? FPoint {
				.x = (f32)sin( msSinceStart / 650.0f  ) *  5.0f,
				.y = (f32)sin( msSinceStart / 500.0f  ) * 10.0f
				   + (f32)sin( msSinceStart / 850.0f  ) *  5.0f
				   + (f32)cos( msSinceStart / 1333.0f ) *  5.0f }
			: FPoint { 0, 0 };
		const FPoint fluxCam = _camera + flux + _mouseOffsetEased;
		return fluxCam;
	}

	void CameraState::Render( const u64 framesSinceStart, u64 msSinceStart, const f32 deltaTNeeded )
	{
		// Try the limits, moments before wraparound,
		// only showing how bad they would behave if this var would be float instead, the cast to float happens inside CalcFluxCam
		//msSinceStart += 2147470000u + 2147480000u;

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const FPoint fluxCam = CalcFluxCam( msSinceStart );

		for( int i = 0; i <= 3; ++i ) // The 4 layers, rendered back to front
		{
			RenderLayer( windowSize, fluxCam, i );
		}
	}

	void CameraState::RenderLayer( const Point windowSize, const FPoint camPos, const int index ) const
	{
		if( !_isBackgroundVisible[index] )
			return;

		const Point size = _backgroundSize[index];
		const FPoint offset = BackgroundStartOffset[index] + camPos * BackgroundFactor[index];
		for( float x = offset.x; x < windowSize.x; x += size.x * 2 )
		{
			for( float y = offset.y; y < windowSize.y; y += size.y * 2 )
			{
				Rect off = { .x = (int)x, .y = (int)y, .w = size.x * 2, .h = size.y * 2 };
				SDL_RenderCopy( renderer(), _background[index].get(), EntireRect, &off );

				// Makes only sense with texture hint == best
				//FRect offset = { .x = x, .y = y, .w = size.x * 2.0f, .h = size.y * 2.0f };
				//SDL_RenderCopyF( renderer, bg[i], EntireRect, &offset );
			}
		}
	}
}
