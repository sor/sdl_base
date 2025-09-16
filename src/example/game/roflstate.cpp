#include "roflstate.hpp"

namespace JanSordid::SDL_Example
{
	void RoflState::Init()
	{
		IGameState::Init();

		{
			JSON j;

			/// Export to file
			j["map"]["name"] = "Summoners Backyard";
			j["map"]["data"] = _level;
			std::ofstream outFile( BasePathMap "rofl.json" );
			outFile << j.dump( 1, '\t' );
			outFile.close();

			j.clear();
			_level.clear();

			/// Import from file
			std::ifstream inFile( BasePathMap "rofl.json" );
			inFile >> j;
			_level = j["map"]["data"]; // Add the following if it does not just work: .get<decltype( _level )>();
		}

		Owned<Surface> surf = IMG_Load( BasePathGraphic "hsnr64.png" );
		SDL_SetSurfaceColorKey( surf, true, 0 );
		_tileset = SDL_CreateTextureFromSurface( renderer(), surf );
		SDL_SetTextureScaleMode( _tileset, SDL_ScaleMode::SDL_SCALEMODE_NEAREST );

		const f32    dstSize = 16 * _game.scalingFactor(); // TODO: Remove that scaling for actual gameplay
		const FPoint halfDst = FPoint{ dstSize, dstSize } / 2;
		Point        dst     = { 0, 0 };
		for( const auto & row : _level ) {
			for( const auto & cell : row ) {
				if( cell >= '0' && cell <= '9' ) {
					uint index = cell - '0';
					_paths[0][index]     = toF( dst ) * dstSize + halfDst;
					_paths[1][9 - index] = toF( dst ) * dstSize + halfDst;
				}
				dst.x += 1;
			}
			dst.x = 0;
			dst.y += 1;
		}
	}

	void RoflState::Destroy()
	{
		//_tileset.reset(); // Will also be called by the destructor, enable to free _tileset before destruction

		IGameState::Destroy();
	}

	bool RoflState::HandleEvent( const Event & event )
	{
		return false;
	}

	void RoflState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		if( _respawnCD < msSinceStart )
		{
			_minions[0].push_back( Entity{ _paths[0][0], 1, 0, 100 } );
			_minions[1].push_back( Entity{ _paths[1][0], 1, 1, 100 } );

			_respawnCD = msSinceStart + 2000;
		}

		constexpr f32 speed = 50;
		for( int faction = 0; faction <= 1; ++faction )
		{
			for( auto & minion : _minions[faction] )
			{
				const FPoint & nextPos   = _paths[faction][minion.nextPath];
				const FPoint   diffPos   = nextPos - minion.position;
				const f32      length    = calcLength( diffPos );
				const f32      distance  = speed * deltaT;
				const bool     isReached = distance >= length;
				if( isReached )
				{
					// TODO: This only moves onto the point, not further, so there might be some minor distance lost
					minion.position  = nextPos;
					minion.nextPath += 1;
					if( minion.nextPath == 10 )
					{
						// TODO: Damage HQ and remove from game for the moment
					}
				}
				else
				{
					const FPoint normalizedDiff = diffPos / length * distance;
					minion.position += normalizedDiff;
				}
			}
		}

		for( int faction = 0; faction <= 1; ++faction )
		{
			const int oppositeFaction = 1 - faction;
			for( auto & minion : _minions[faction] )
			{
				if( minion.health <= 0 )
				{
					// TODO: Actually remove the dead minions, now dead minions are still in
					continue;
				}

				for( auto & oppoMinion : _minions[oppositeFaction] )
				{
					if( oppoMinion.health <= 0 )
					{
						// TODO: Actually remove the dead minions, now dead minions are still in
						continue;
					}

					const FPoint diffPos = oppoMinion.position - minion.position;
					const f32    length  = calcLength( diffPos );
					if( length < 15 )
					{
						// TODO: actually fight, for now just die
						minion.health = 0;
						oppoMinion.health = 0;
					}
				}
			}
		}
	}

	void RoflState::Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded )
	{
		const SDL_FlipMode flipAnim = msSinceStart / 200 % 2 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

		//SDL_SetTextureBlendMode( _tileset, SDL_BLENDMODE_BLEND );
		//SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND);
		constexpr int srcSize = 16;
		const int     dstSize = 16 * _game.scalingFactor();
		const FPoint  halfDst = FPoint{ (f32)dstSize, (f32)dstSize } / 2;
		Point         dst     = { 0, 0 };
		for( const auto & row : _level ) {
			for( const auto & cell : row ) {
				const TileInfo & info    = tileInfos[cell];
				const Color    & color   = info.color;
				const FRect      srcRect = toF( toXY( info.index * srcSize, srcSize ) );
				const FRect      dstRect = toF( toXY( dst * dstSize, dstSize ) );
				SDL_SetTextureColorMod( _tileset, color.r, color.g, color.b );
				SDL_RenderTextureRotated(
					renderer(),
					_tileset,
					&srcRect,
					&dstRect,
					info.rotation,
					nullptr,
					(cell | 0b100000) == 'f' ? flipAnim : SDL_FLIP_NONE );
				dst.x += 1;
			}
			dst.x  = 0;
			dst.y += 1;
		}

		SDL_SetRenderDrawColor( renderer(), 156, 107, 48, 255 );
		SDL_RenderLines( renderer(), _paths[0].data(), _paths[0].size() );

		for( int faction = 0; faction <= 1; ++faction )
		{
			const Color & color = factionColors[faction];
			SDL_SetTextureColorMod( _tileset, color.r, color.g, color.b );

			for( auto & minion : _minions[faction] )
			{
				if( minion.health <= 0 )
				{
					// TODO: Actually remove the dead minions, now dead minions are still in
					continue;
				}

				const FRect srcRect = toXY( FPoint{ 18, 33 } * srcSize, srcSize );
				const FRect dstRect = toXY( minion.position - halfDst, dstSize );
				SDL_RenderTextureRotated(
					renderer(),
					_tileset,
					&srcRect,
					&dstRect,
					0,
					nullptr,
					SDL_FLIP_NONE );
			}
		}
	}
}
