#pragma once

#include "example_game.hpp"

#include <hsnr64/tiles.hpp>

namespace JanSordid::SDL_Example
{
	using namespace JanSordid::SDL;
	using namespace JanSordid::HSNR64;

	class MapEditorState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		using Tile = Tile64K;

		TileSet _tileSet;
		TileMap _tileMaps[2];

		// Map Viewport
		FPoint  _viewScale       = { 2, 2 };
		FPoint  _viewOffset      = { -256, -256 };

		// Palette
		FPoint  _paletteScale    = { 2, 2 };
		FPoint  _paletteOffset   = { 0, 0 };

		int     _selectedLayer   = 1;
		int     _selectedIndex   = 0;
		int     _selectedMod     = 0; // Index of Rotations & Flips
		int     _selectedColor   = 10;
		int     _selectedAlpha   = 3;
		int     _selectedRotation= 0;
		int     _selectedFlip    = 0;
		int     _buttonHeld      = 0;
		bool    _isKeepColor     = false;
		bool    _isKeepAlpha     = false;
		bool    _isCtrlHeld      = false;

	public:
		using PerTileCallback = Tile( Point pos, Point size );

		static auto            uniformTile( TileColor color, FlipRot flipRot, u16 index );
		static PerTileCallback emptyTile;
		static PerTileCallback randomTile;
		static PerTileCallback randomSolidTile;

		/// Ctors & Dtor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		template <typename TCallback>
		void FillLayer( u8 layer, TCallback       callback );
		void FillLayer( u8 layer, PerTileCallback callback = randomSolidTile );
		void SetTile( u8 layer, Point tileIndex, i8 color, i8 alpha, FlipRot flipRot, u16 paletteIndex );

		template <typename E>
		void HandleSpecificEvent( const E & ev );
		bool Input( const Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;
		ImGuiOnly(
			void RenderUI( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;
		)

		void SaveMap() const;
	};
}
