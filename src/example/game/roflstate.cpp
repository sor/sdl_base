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
		SDL_SetColorKey( surf, SDL_TRUE, 0 );
		_tileset = SDL_CreateTextureFromSurface( renderer(), surf );

		const int   dstSize = 16 * _game.scalingFactor(); // TODO: Remove that scaling for actual gameplay
		const Point halfDst = Point{ dstSize, dstSize } / 2;
		Point       dst     = { 0, 0 };
		for( const auto & row : _level ) {
			for( const auto & cell : row ) {
				if( cell >= '0' && cell <= '9' ) {
					uint index = cell - '0';
					_paths[0][index]     = dst * dstSize + halfDst;
					_paths[1][9 - index] = dst * dstSize + halfDst;
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

	void RoflState::Update( u64 frame, u64 totalMSec, f32 deltaT )
	{

	}

	void RoflState::Render( u64 frame, u64 totalMSec, f32 deltaT )
	{
		const SDL_RendererFlip flipAnim = totalMSec / 200 % 2 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

		//SDL_SetTextureBlendMode( _tileset, SDL_BLENDMODE_BLEND );
		//SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND);
		constexpr int          srcSize  = 16;
		const int              dstSize  = 16 * _game.scalingFactor();
		Point                  dst      = { 0, 0 };
		for( const auto & row : _level ) {
			for( const auto & cell : row ) {
				const TileInfo & info    = tileInfos[cell];
				const Color    & color   = info.color;
				const Rect       srcRect = toXY( info.index * srcSize, srcSize );
				const Rect       dstRect = toXY( dst * dstSize, dstSize );
				SDL_SetTextureColorMod( _tileset, color.r, color.g, color.b );
				SDL_RenderCopyEx(
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

		SDL_SetRenderDrawColor( renderer(), 255, 255, 255, 255 );
		SDL_RenderDrawLines( renderer(), _paths[0].data(), _paths[0].size() );
	}
}