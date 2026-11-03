#include "tiles.hpp"

namespace JanSordid::HSNR64
{
	static_assert(SDL_FlipMode::SDL_FLIP_HORIZONTAL == (SDL_FlipMode)FlipRot::HorizontalFlip);
	static_assert(SDL_FlipMode::SDL_FLIP_VERTICAL   == (SDL_FlipMode)FlipRot::VerticalFlip);

	// How many fit a 2D Grid per 4K Page
	static_assert( sizeof(TileColor) == 1 );	// 64x64*1 = 4096
	static_assert( sizeof(Tile256)   == 2 );	// 45x45*2 = 4050
	static_assert( sizeof(Tile4K)    == 3 );	// 36x36*3 = 3888 - 36x37*3 = 3996 - 35x39*3 = 4095 // Care: Not worth the effort right now
	static_assert( sizeof(Tile64K)   == 4 );	// 32x32*4 = 4096

	// C-compatible layout
	static_assert( std::is_standard_layout_v<TileColor> );

	// Safe for memcpy/bit ops
	static_assert( std::is_trivially_copyable_v<TileColor> );
	static_assert( std::is_trivially_copyable_v<Tile256> );
	static_assert( std::is_trivially_copyable_v<Tile4K> );
	static_assert( std::is_trivially_copyable_v<Tile64K> );

	// Static initialization
	static_assert( std::is_trivial_v<TileColor>);
	static_assert( std::is_trivial_v<Tile256>);
	static_assert( std::is_trivial_v<Tile4K>);
	static_assert( std::is_trivial_v<Tile64K>);

	void DrawTile( Renderer * renderer, Texture * tex, const Tile & curr_tile, const Tile & last_tile, const FRect & dst, const FPoint & center )
	{
		// Bullshit below: Index 0 should be usable and visible, color 0 (and also alpha 0?) is invisible
		// An index of 0 means: Do not render anything, which fills in for alpha 0% (which therefore does not exist as value in alpha)
		// There needs to be a sentinel last_tile for the first call, which has color and alpha set to the default values
		//assertCE( curr_tile.index() != 0 ); // Prevent entering this function on index == 0
		//assertCE( last_tile.index() != 0 ); // The last_tile can also not be index == 0, pass in the one before that

		// A color == 0 means: Do not draw, TODO: Probably do not even enter this function on color == 0
		if( curr_tile.color == 0 )
		{
			return;
		}

		if( curr_tile.color != last_tile.color )
		{
			const Color & color = Palette( curr_tile.color );
			SDL_SetTextureColorMod( tex, color.r, color.g, color.b );
		}

		if( curr_tile.alpha != last_tile.alpha )
		{
			// Variable alpha ends up with these 4 possible values
			//  0: ~25% = 0b00'11'11'11 =  63(/255)
			//  1: ~50% = 0b01'11'11'11 = 127(/255)
			//  2: ~75% = 0b10'11'11'11 = 191(/255)
			//  3: 100% = 0b11'11'11'11 = 255(/255)
			const u8 alpha = curr_tile.alpha << 6 | to_underlying( NamedColor::AlphaFill );
			SDL_SetTextureAlphaMod( tex, alpha );
		}

		constexpr Point tileSize = { 16, 16 };
		constexpr u16   stride   = 32; // How many per row
		const     u16   index    = curr_tile.index();
		const     Point sel      = Point{ (index % stride), (index / stride) } * tileSize;
		const     FRect src      = toF( toRect( sel, tileSize ) ); // toF( toXY( idx2d, tileSize.x ) * tileSize );
		const     f64   angle    = IsSet( curr_tile.flipRot, FlipRot::Rotate45 ) * 45
		                         + IsSet( curr_tile.flipRot, FlipRot::Rotate90 ) * 90;
		const SDL_FlipMode flip  = (SDL_FlipMode)(IsSet( curr_tile.flipRot, FlipRot::HorizontalFlip ) * SDL_FlipMode::SDL_FLIP_HORIZONTAL
		                                        + IsSet( curr_tile.flipRot, FlipRot::VerticalFlip )   * SDL_FlipMode::SDL_FLIP_VERTICAL);

		SDL_RenderTextureRotated( renderer, tex, &src, &dst, angle, &center, flip );
	}
}
