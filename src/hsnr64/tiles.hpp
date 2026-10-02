#pragma once

#include "palette.hpp"
#include "sor/core.hpp"
#include "sor/sdl_core.hpp"
#include "sor/sdl_shapeops.hpp"

namespace JanSordid::HSNR64
{
	using namespace JanSordid::SDL;

	//#define TILE_INDEX_MAX_4096

	/// Reflection (Flip)
	/// https://en.wikipedia.org/wiki/Reflection_(mathematics)
	/// https://en.wikipedia.org/wiki/Point_reflection
	/// Rotation is Clockwise
	enum class FlipRot : u8 // only 4 bits used
	{
		None            = 0b0000,
		HorizontalFlip  = 0b0001, // SDL_FlipMode::SDL_FLIP_HORIZONTAL
		VerticalFlip    = 0b0010, // SDL_FlipMode::SDL_FLIP_VERTICAL
		Rotate90        = 0b0100, // SDL_RenderTextureRotated::angle +90
		Rotate45        = 0b1000, // SDL_RenderTextureRotated::angle +45, last in list to be able to be cut off, CARE: Can visibly overlap with surrounding tiles

		// TODO: Check if Rotate45 could be written to a : 3 version of this enum

		Rotate180       = HorizontalFlip | VerticalFlip,  // Flipping on both axis is the same as a 180-degree turn

		Rotate135       = Rotate45 | Rotate90,
		Rotate225       = Rotate45 |            Rotate180,
		Rotate270       =            Rotate90 | Rotate180,
		Rotate315       = Rotate45 | Rotate90 | Rotate180,
	};

	constexpr FlipRot operator &( const FlipRot lhs, const FlipRot rhs ) { return (FlipRot)(to_underlying( lhs ) & to_underlying( rhs )); }
	constexpr FlipRot operator |( const FlipRot lhs, const FlipRot rhs ) { return (FlipRot)(to_underlying( lhs ) | to_underlying( rhs )); }
	constexpr bool    operator !( const FlipRot v ) { return v != FlipRot::None; }

	// The Tileset to use is specified by the TileLayer

	// A single Tile of the TileSet
	// Uses 3 or 3.5 (rounded up to 4) bytes,

	// A Tile with only color and alpha, usable as background or to shade what is beneath
	struct TileColor
	{
		u8 color :6;        // SDL_SetTextureColorMod, color == 0 means don't draw == invisible / transparent
		u8 alpha :2;        // SDL_SetTextureAlphaMod // 0 = 25%, 1 = 50%, 2 = 75%, 3 = 100% - if color == 0, then this is irrelevant OR must also be 0?
	};

	// Has no flipping nor rotation
	struct Tile256 : TileColor
	{
		u8 index :8;
	};

	// The 3 byte version only uses a 12 bit index, meaning 4096 indices (e.g. 64^2 or 32x128) are possible
	struct Tile4K : TileColor
	{
		u8      index1  :8;        // index1 == 0 && index2 == 0 means don't draw... <- not needed since color == invis already says so
		u8      index2  :4;        // index == index1 + index2 * 256
		FlipRot flipRot :4;

		#define TileIndex3Byte( index ) (u8)(index%256), (u8)(index/256)
		[[nodiscard]] inline u16  index() const  { return index1 + index2 * 256; }
		              inline void index( u16 i ) { index1 = i % 256; index2 = i / 256; }
	};

	// The 3.5/4 byte version uses a 16 bit index, meaning 64k indices (e.g. 256^2) are possible,
	// without the bitfield-micro-optimization this would be 8 bytes
	struct Tile64K : TileColor
	{
		FlipRot flipRot :4;     // 4 bit still unused, what could be put here?
		u16     index1  :16;    // Needs to be after flipRot here, else alignment will cause the struct to grow

		#define TileIndex4Byte( index ) (index)
		[[nodiscard]] inline u16  index() const  { return index1; }
		              inline void index( u16 i ) { index1 = i; }
	};

	// HACK: This must go
	#ifdef TILE_INDEX_MAX_4096
		using   Tile = Tile4K;
		// index must be free of side effects
		#define TileIndex( index ) TileIndex3Byte( index )
		#define TileIndexCtor( index ) (u8)(index%256), (u8)(index/256)
	#else
		using   Tile = Tile64K;
		#define TileIndex( index ) TileIndex4Byte( index )
		#define TileIndexCtor( index ) (index)
	#endif

	void DrawTile( Renderer * renderer, Texture * tex, const Tile & curr_tile, const Tile & last_tile, const FRect & dst, const FPoint & center );

	struct TileInfo
	{
		// Collider, Hitbox, Hurtbox (does it hurt you or others?)
		// Bounding volumes/areas
		u16     bitset;
		Point   orderPivot;  // might be interesting for DrawingOrder::BackToFront
	};

	// Should be able to be used multiple times
	struct TileSet
	{
		Owned<Texture>  texture;
		Point           textureSize = {  512,  1024 };     // Refering to the texture x,y = size of one tile; w,h = amount of tiles
		Point           tileSize    = {   16,    16 };     // Size of a Tile in pixels
		Point           tileCount   = {   32,    64 };     // Refering to the texture x,y = size of one tile; w,h = amount of tiles
	//	Point           startOffset = {    0,     0 };     // Empty space left/top of the first Tile
	//	Point           margin      = {    0,     0 };     // Empty space between the Tiles
	};

	// Uniform Tiling of 2D Space
	// TODO: cacheability of the whole layer
	struct TileMap // TileLayer
	{
		static constexpr f32    halfSize  = 0.5f;
		static constexpr FPoint halfPoint = { 0.5f, 0.5f };

		DynArray<Tile>  tiles;
		TileSet *       tileSet;
		FPoint          tileDist    = { 16.0f, 16.0f };     // This should usually be the same as tileSet.tileSize, but for an overlapping TileSet this can for example be lower
		FPoint          tileOffset  = {  0.0f,  0.0f };     // Offset from the zero coordinate
		FPoint          sizeScale   = {  1.0f,  1.0f };     // Output size scaling
		FPoint          scrollScale = {  1.0f,  1.0f };     // Parallax scroll scaling
		FPoint          center;  // = toF( tileSet->tileSize ) * sizeScale * halfSize;
		Point           size;
		int             stride; // Same as size.x?
		SDL_BlendMode   blendMode   = SDL_BLENDMODE_BLEND;  // SDL_SetTextureBlendMode
		SDL_ScaleMode   scaleMode   = SDL_ScaleMode::SDL_SCALEMODE_NEAREST; // SDL_SetTextureScaleMode -OR- SDL_HINT_RENDER_SCALE_QUALITY 0, 1, 2
	//	DrawingOrder    order       = DrawingOrder::DontCare;
	//	bool            enabled     = true;
	};

	struct MultiTileMap // MultiLayer
	{
		DynArray<TileMap> layers;
		DynArray<bool>    enabledLayers;
	};


	// TODO: Create a 2D vector (more DynArray!), that supports growing in 2 dimensions.
	// Consists of single-alloc-sectors which are a quadratic area each, e.g. 32x32x4bytes = 4096
	template <typename T, size_t S>
	class Vector2D
	{
	//	static_assert( isPowerOfTwo( S ) );
		static_assert( S * S * sizeof( T ) <= 4096 );
		DynArray<Array<T, S * S>> sectors;

		// method to grow x direction -> just append on the end
		// method to grow y direction -> insert at 1..n * stride
	};

	inline Vector2D<Tile,32> v2d;
}
