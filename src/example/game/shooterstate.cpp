#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	void ShooterState::Init()
	{
		Base::Init();

		if( !_projectile[0] )
		{
			_projectile[0] = IMG_LoadTexture( renderer(), BasePathGraphic "fire.png"   );
			_projectile[1] = IMG_LoadTexture( renderer(), BasePathGraphic "ice.png"    );
			_projectile[2] = IMG_LoadTexture( renderer(), BasePathGraphic "poison.png" );

			SDL_SetTextureColorMod( _projectile[2], 191, 191, 191 );
		}

		if( !_sound )
		{
			_sound = Mix_LoadWAV( BasePathSound "pew.wav" );
			if( !_sound )
				print( stderr, "Mix_LoadWAV failed: {}\n", Mix_GetError() );
		}
	}

	void ShooterState::Destroy()
	{
		// TODO

		Base::Destroy();
	}

	bool ShooterState::HandleEvent( const Event & event )
	{
		if( event.type == SDL_KEYDOWN && event.key.repeat == 0 )
		{
			if( event.key.keysym.scancode == SDL_SCANCODE_F1 ) _isBackgroundVisible[0] = !_isBackgroundVisible[0];
			if( event.key.keysym.scancode == SDL_SCANCODE_F2 ) _isBackgroundVisible[1] = !_isBackgroundVisible[1];
			if( event.key.keysym.scancode == SDL_SCANCODE_F3 ) _isBackgroundVisible[2] = !_isBackgroundVisible[2];
			if( event.key.keysym.scancode == SDL_SCANCODE_F4 ) _isBackgroundVisible[3] = !_isBackgroundVisible[3];

			// Toggle all
			if( event.key.keysym.scancode == SDL_SCANCODE_F5 )
				_isBackgroundVisible[0] = _isBackgroundVisible[1] = _isBackgroundVisible[2] = _isBackgroundVisible[3]
					= !_isBackgroundVisible[0];

			if( event.key.keysym.scancode == SDL_SCANCODE_F6 ) _isInverted = !_isInverted;
			if( event.key.keysym.scancode == SDL_SCANCODE_F7 ) _isEased    = !_isEased;
			if( event.key.keysym.scancode == SDL_SCANCODE_F8 ) _isFlux     = !_isFlux;

			return true; // Not 100% correct
		}
		else if( event.type == SDL_MOUSEBUTTONDOWN )
		{
			const FPoint mousePos = FPoint{
				(f32)event.button.x,
				(f32)event.button.y } - _mouseOffsetEased - _camera;
			//enemyProjectiles.push_back( mousePos );
			//rvProjectiles.Spawn(mousePos);
			//rvProjectiles.Spawn({1,1});

			// This is spawning a projectile inside Update()
			_spawnProjectileAt = mousePos;

			return true;
		}
		else if( event.type == SDL_MOUSEMOTION )
		{
			Point windowSize;
			SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

			const FPoint halfWindowSize = toF( windowSize / 2 );
			const FPoint mousePos = { (float)event.motion.x, (float)event.motion.y };

			_mouseOffset = halfWindowSize - mousePos;

			if( event.motion.state != 0 )
			{
				const FPoint mousePosOffsetted = FPoint{
					(float)event.motion.x,
					(float)event.motion.y } - _mouseOffsetEased - _camera;
				//enemyProjectiles.push_back( mousePosOffsetted );

				// This is spawning a projectile inside Update()
				_spawnProjectileAt = mousePosOffsetted;
			}

			return true;
		}
		else if( event.type == SDL_MOUSEBUTTONUP )
		{
			//mouseOffset = { 0, 0 };

			return true;
		}
		else
		{
			return false;
		}
	}

	void ShooterState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		const float travelDist = deltaT * 60.0f;

		_progress += travelDist;

		_camera.x -= travelDist;

		_player.x += travelDist;
		_player.y += (((int) (_player.x / 50)) % 2)
		            ?  travelDist * 2
		            : -travelDist * 2;

		for( auto & e : _enemies )
			e.x += travelDist;

		const float angle = (float)msSinceStart * 0.001f;
		const float satOffsetAngle = std::numbers::pi_v<f32> * 2.0f / (float)_satellites.size();
		for( uint i = 0; i < _satellites.size(); ++i )
		{
			_satellites[i].x = cos( angle + satOffsetAngle * (float)i ) * (-150) /*- sin(angle) * (100)*/ + (_player.x + _player.w / 2);
			_satellites[i].y = sin( angle + satOffsetAngle * (float)i ) * (-150) /*+ cos(angle) * (100)*/ + (_player.y + _player.h / 2);
		}

		if( _spawnProjectileAt.x >= 0
		    && _spawnProjectileAt.y >= 0 )
		{
			SpawnEnemyProjectile( _spawnProjectileAt );

			if( _spawnProjectileSoundCD < msSinceStart )
			{
				Mix_PlayChannel( -1, _sound, 0 );
				_spawnProjectileSoundCD = msSinceStart + 45;
			}

			_spawnProjectileAt = {-1, -1 };
		}

		for( auto & p : _enemyProjectiles )
		{
			p.x -= deltaT * 300.f;
		}

		for( auto & p : _myProjectiles )
		{
			p.x += deltaT * 500.f;
		}

		if( _shootCooldown < msSinceStart )
		{
			_shootCooldown += 250;
			if( _shootCooldown < msSinceStart )
				_shootCooldown = msSinceStart + 250;

			SpawnMyProjectile( FPoint{ _player.x + _player.w,      _player.y + _player.h / 2      } );
			SpawnMyProjectile( FPoint{ _player.x + _player.w - 20, _player.y + _player.h / 2 - 10 } );
			SpawnMyProjectile( FPoint{ _player.x + _player.w - 20, _player.y + _player.h / 2 + 10 } );
			for( uint i = 0; i < _satellites.size(); ++i )
			{
				SpawnMyProjectile( _satellites[i] );
			}
		}

		const Rect playerRect = { (int)_player.x, (int)_player.y, (int)_player.w, (int)_player.h };

		for( auto it = _enemyProjectiles.begin(); it != _enemyProjectiles.end(); ++it )
		{
			auto & p = *it;

			if( !IsProjectileAlive( it ) )
				continue;

			const Point pos              = { (int)p.x, (int)p.y };
			const bool  isJustOutOfLevel = p.x < 0;
			const bool  isPlayerHit      = SDL_PointInRect( &pos, &playerRect );

			bool isPlayerExtraHit = false;
			for( uint i = 0; i < _satellites.size(); ++i )
			{
				const float dx = p.x - _satellites[i].x;
				const float dy = p.y - _satellites[i].y;
				const float dist_squared = dx * dx + dy * dy;
				isPlayerExtraHit |= dist_squared < (SatRadius * SatRadius);

				//const bool isPlayerExtraHit2 = SDL_PointInRect( &pos, &satRect );
			}

			if( isPlayerHit )
			{
				// TODO: Damage the player
			}

			if( isPlayerHit || isPlayerExtraHit || isJustOutOfLevel )
			{
				RetireProjectile( it );
			}
		}

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		for( auto it = _myProjectiles.begin(); it != _myProjectiles.end(); ++it )
		{
			auto & p = *it;

			if( !IsProjectileAlive( it ) )
				continue;

			const Point pos = { (int)p.x, (int)p.y };
			const bool isJustOutOfLevel = p.x > _progress + windowSize.x * 2;
			if( isJustOutOfLevel )
			{
				RetireMyProjectile( it );
				continue;
			}

			for( auto & e : _enemies )
			{
				const Rect enemyRect  = { (int)e.x, (int)e.y, (int)e.w, (int)e.h };
				const bool isEnemyHit = SDL_PointInRect( &pos, &enemyRect );
				if( isEnemyHit )
				{
					// TODO: Damage the enemy

					RetireMyProjectile( it );
					break;
				}
			}
		}

		CameraState::Update( framesSinceStart, msSinceStart, deltaT );
	}

	Vector<SDL_Point> PixelizeCircle( SDL_Point center, int radius )
	{
		Vector<SDL_Point> points;
		const int arrSize = RoundUpMultiple<8>( radius * 8 * 35 / 49 ); // 35 / 49 is a slightly biased approximation of 1/sqrt(2)
		points.reserve( arrSize );

		const i32 diameter = (radius * 2);

		i32 x = (radius - 1);
		i32 y = 0;
		i32 tx = 1;
		i32 ty = 1;
		i32 error = (tx - diameter);

		while( x >= y )
		{
			// Each of the following renders an octant of the circle
			points.push_back( center + SDL_Point{ +x, -y } );
			points.push_back( center + SDL_Point{ +x, +y } );
			points.push_back( center + SDL_Point{ -x, -y } );
			points.push_back( center + SDL_Point{ -x, +y } );
			points.push_back( center + SDL_Point{ +y, -x } );
			points.push_back( center + SDL_Point{ +y, +x } );
			points.push_back( center + SDL_Point{ -y, -x } );
			points.push_back( center + SDL_Point{ -y, +x } );

			if( error <= 0 )
			{
				++y;
				error += ty;
				ty += 2;
			}

			if( error > 0 )
			{
				--x;
				tx += 2;
				error += (tx - diameter);
			}
		}

		return points; // NRVO will make this good, fingers crossed
	}

	void DrawCircle( SDL_Renderer * renderer, const Vector<SDL_Point> & points )
	{
		SDL_RenderDrawPoints( renderer, points.data(), points.size() );
	}

	void DrawCircle( SDL_Renderer * renderer, SDL_Point center, int radius )
	{
		const Vector<SDL_Point> points = PixelizeCircle( center, radius );
		DrawCircle( renderer, points );
	}

	void ShooterState::Render( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaTNeeded )
	{
		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const FPoint fluxCam = CalcFluxCam( msSinceStart );

		for( int i = 0; i <= 2; ++i ) // Only the first 3 layers, rendered back to front
		{
			RenderLayer( windowSize, fluxCam, i );
		}

		{
			constexpr const Point pivot   = { 10, 45 };
			constexpr const Point texSize = { 10, 26 };
			constexpr const Rect  src     = {
				texSize.x,
				texSize.y,
				texSize.x,
				texSize.y };
			//for( auto & p : enemyProjectiles )
			for( auto it = _enemyProjectiles.begin(); it != _enemyProjectiles.end(); ++it )
			{
				auto & p = *it;

				if( !IsProjectileAlive( it ) )
					continue;

				const Point index      = IndexUnpackClamped<10, 6>( framesSinceStart + p.y );
				const Rect  srcIndexed = src * index;
				const Rect  pos        = {
					(int)(p.x + fluxCam.x) - pivot.x / 2,
					(int)(p.y + fluxCam.y) - pivot.y,
					texSize.x * 2,
					texSize.y * 2 };
				SDL_RenderCopyEx( renderer(), _projectile[0], &srcIndexed, &pos, 90, &pivot, SDL_RendererFlip::SDL_FLIP_NONE );
				//SDL_RenderCopy( renderer(), projectile[2], &src, &pos );
			}
		}

		{
			constexpr const Point pivot   = { 10, 9 };
			constexpr const Point texSize = { 65, 9 };
			constexpr const Rect  src     = {
				texSize.x,
				texSize.y,
				texSize.x,
				texSize.y };
			for( auto it = _myProjectiles.begin(); it != _myProjectiles.end(); ++it )
			{
				auto & p = *it;

				if( !IsProjectileAlive( it ) )
					continue;

				const Point index      = IndexUnpackClamped<10, 6>( framesSinceStart + p.y );
				const Rect  srcIndexed = src * index;
				const Rect  pos        = {
					(int)(p.x + fluxCam.x) - pivot.x / 2,
					(int)(p.y + fluxCam.y) - pivot.y,
					texSize.x * 2,
					texSize.y * 2 };
				SDL_RenderCopyEx( renderer(), _projectile[2], &srcIndexed, &pos, 180, &pivot, SDL_RendererFlip::SDL_FLIP_NONE );
				//SDL_RenderCopy( renderer(), _projectile[2], &src, &pos );
			}
		}

		// Draw player
		SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND );
		SDL_SetRenderDrawColor( renderer(), 0, 255, 0, 127 );

		//const Point fluxCamI = { (int)fluxCam.x, (int)fluxCam.y };
		const FRect pos = _player + fluxCam;
		SDL_RenderFillRectF( renderer(), &pos );

		// Draw player satellites
		SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_NONE );
		SDL_SetRenderDrawColor( renderer(), 0, 255, 255, 255 );

		for( uint i = 0; i < _satellites.size(); ++i )
		{
			const FPoint sat2Pos = _satellites[i] + fluxCam;
			DrawCircle( renderer(), Point{ (int)(sat2Pos.x), (int)(sat2Pos.y) }, SatRadius );
		}

		// Draw enemies
		SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND );
		SDL_SetRenderDrawColor( renderer(), 255, 0, 0, 127 );

		for( auto & e : _enemies )
		{
			//const Point fluxCamI = { (int)fluxCam.x, (int)fluxCam.y };
			const FRect posOffsetted = e + fluxCam;
			SDL_RenderFillRectF( renderer(), &posOffsetted );
		}

		for( int i = 3; i <= 3; ++i ) // Render the last 1 layers, rendered back to front
			RenderLayer( windowSize, fluxCam, i );
	}

	bool ShooterState::IsProjectileAlive( const Vector<FPoint>::iterator & it ) const
	{
		return isfinite( it->x );
	}

	void ShooterState::SpawnEnemyProjectile( const FPoint pos )
	{
		if( _numDeadEnemyProj > 20 )
		{
			DebugOnly( int count = 0; )
			while( IsProjectileAlive( _enemyProjReuse ) )
			{
				DebugOnly( ++count; )
				++_enemyProjReuse;
				if( _enemyProjReuse == _enemyProjectiles.end() ) [[unlikely]]
				{
					DebugOnly( print( "seeking wrapped around\n" ); )
					_enemyProjReuse = _enemyProjectiles.begin();
				}
			}
			DebugOnly( print( "seeked for {} entries in enemyProjectiles until a reusable was found\n", count ); )
			*_enemyProjReuse = pos;
			--_numDeadEnemyProj;
		}
		else
		{
			const usize oldCapa = _enemyProjectiles.capacity();

			_enemyProjectiles.push_back( pos );

			const usize newCapa = _enemyProjectiles.capacity();

			// Determine if iterators were invalidated
			if( oldCapa != newCapa ) [[unlikely]]
			{
				DebugOnly( print( "oc: {} nc: {}\n", oldCapa, newCapa ); )
				_enemyProjReuse = _enemyProjectiles.begin();
			}
		}
	}

	void ShooterState::SpawnMyProjectile( const FPoint pos )
	{
		if( _numDeadMyProj > 20 )
		{
			DebugOnly( int count = 0; )
			while( IsProjectileAlive( _myProjReuse ) )
			{
				DebugOnly( ++count; )
				++_myProjReuse;
				if( _myProjReuse == _myProjectiles.end() ) [[unlikely]]
				{
					DebugOnly( print( "seeking wrapped around\n" ); )
					_myProjReuse = _myProjectiles.begin();
				}
			}
			DebugOnly( print( "seeked for {} entries in myProjectiles until a reusable was found\n", count ); )
			*_myProjReuse = pos;
			--_numDeadMyProj;
		}
		else
		{
			const usize oldCapa = _myProjectiles.capacity();

			_myProjectiles.push_back( pos );

			const usize newCapa = _myProjectiles.capacity();

			// Determine if iterators were invalidated
			if( oldCapa != newCapa ) [[unlikely]]
			{
				DebugOnly( print( "oc: {} nc: {}\n", oldCapa, newCapa ); )
				_myProjReuse = _myProjectiles.begin();
			}
		}
	}

	void ShooterState::RetireProjectile( const Vector<FPoint>::iterator & it )
	{
		it->x = INFINITY;
		_enemyProjReuse = it;
		++_numDeadEnemyProj;
	}

	void ShooterState::RetireMyProjectile( const Vector<FPoint>::iterator & it )
	{
		it->x = -INFINITY;
		_myProjReuse = it;
		++_numDeadMyProj;
	}
}
