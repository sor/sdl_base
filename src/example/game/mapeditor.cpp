#include "example_game.hpp"
#include "mapeditor.hpp"

namespace JanSordid::SDL_Example
{
	/// static functions

	lambda MapEditorState::uniformTile( const TileColor color, const FlipRot flipRot, const u16 index )
	{
		return [ color, flipRot, index ]( const Point pos, const Point size )
		{
			return Tile{ color, flipRot, index };
		};
	}

	Tile MapEditorState::emptyTile( const Point pos, const Point size )
	{
		return Tile{ TileColor{ 0, 0 }, FlipRot::None, 0 };
	}

	Tile MapEditorState::randomTile( const Point pos, const Point size )
	{
		const int tIndex = 832 + (rand() & 255);
		return Tile{ TileColor{ (u8)(27 + (rand() % 6)), 3 }, (FlipRot)(rand() % 9), (u16)tIndex };
	}

	Tile MapEditorState::randomSolidTile( const Point pos, const Point size )
	{
		return Tile{ TileColor{ (u8)(16 + (rand() % 6)), 3 }, FlipRot::None, 1000 };
	}

	/// instance functions

	// Care: This is far from finished and still buggy
	void MapEditorState::Init()
	{
		Base::Init();

		Owned<SDL_Surface> surface = IMG_Load( BasePathGraphic "hsnr64.png" );
		if( surface != nullptr )
		{
			//SDL_SetColorKey( surface,  true, SDL_MapRGB( surface->format, 0, 0, 0 ) );
			const u32 paletteIndex = SDL_MapRGB( SDL_GetPixelFormatDetails( surface->format ), SDL_GetSurfacePalette( surface ), 0, 0, 0 );
			SDL_SetSurfaceColorKey( surface, true, paletteIndex );

			_tileSet.texture     = SDL_CreateTextureFromSurface( renderer(), surface );
			_tileSet.textureSize = { surface->w, surface->h };
			_tileSet.tileSize    = { 16, 16 };
			_tileSet.tileCount   = _tileSet.textureSize / _tileSet.tileSize; // kind of unnecessary
		}

		_tileMaps[0].tileSet   = &_tileSet;
		_tileMaps[0].tileDist  = toF( _tileMaps[0].tileSet->tileSize );
		_tileMaps[0].sizeScale = { 2, 2 };
		_tileMaps[0].center    = toF( _tileMaps[0].tileSet->tileSize ) * _tileMaps[0].sizeScale * _tileMaps[0].halfSize;
		_tileMaps[0].size      = { 128, 128 };
		_tileMaps[0].stride    = _tileMaps[0].size.x; // Needed if tileMap.tiles is one dimensional
	//	_tileMaps[0].scaleMode = SDL_ScaleModeBest;

		// TODO: Move this to a "create" method?
		_tileMaps[0].tiles.resize( _tileMaps[0].size.x * _tileMaps[0].size.y );

		_tileMaps[1].tileSet     = &_tileSet;
		_tileMaps[1].tileDist    = toF( _tileMaps[1].tileSet->tileSize );
	//	_tileMaps[1].tileDist    = { 15, 15 };
		_tileMaps[1].tileOffset  = { 8, 8 }; // TODO: Not yet used
		_tileMaps[1].sizeScale   = { 2, 2 };
		_tileMaps[1].scrollScale = { 1, 1 };
		_tileMaps[1].center      = toF( _tileMaps[1].tileSet->tileSize ) * _tileMaps[1].sizeScale * _tileMaps[1].halfSize;
		_tileMaps[1].size        = { 128, 128 };
		_tileMaps[1].stride      = _tileMaps[1].size.x; // Needed if tileMap.tiles is one dimensional
	//	_tileMaps[1].scaleMode   = SDL_ScaleModeBest;

		_tileMaps[1].tiles.resize( _tileMaps[1].size.x * _tileMaps[1].size.y );

		// Fill map layer 0 with randomness, color layer
		FillLayer( 0, randomSolidTile );

		// Fill map layer 1 with randomness
		FillLayer( 1, randomTile );

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		_paletteOffset = { (f32)windowSize.x - 512, -768 };
	}

	void MapEditorState::Destroy()
	{
		Base::Destroy();
	}

	template <typename TLambda>
	void MapEditorState::FillLayer( const u8 layer, TLambda callback )
	{
		TileMap   & tileMap       = _tileMaps[layer];
		const Point tileMapSize   = tileMap.size;
		const int   tileMapStride = tileMap.stride;

		for( int y = 0; y < tileMapSize.y; ++y )
		{
			for( int x = 0; x < tileMapSize.x; ++x )
			{
				const int index = x + y * tileMapStride;
				tileMap.tiles[index] = callback( Point{ x, y }, tileMapSize );
			}
		}
	}

	void MapEditorState::FillLayer( const u8 layer, PerTileCallback callback )
	{
		TileMap   & tileMap       = _tileMaps[layer];
		const Point tileMapSize   = tileMap.size;
		const int   tileMapStride = tileMap.stride;

		for( int y = 0; y < tileMapSize.y; ++y )
		{
			for( int x = 0; x < tileMapSize.x; ++x )
			{
				const int index = x + y * tileMapStride;
				tileMap.tiles[index] = callback( Point{ x, y }, tileMapSize );
			}
		}
	}

	constexpr FlipRot toFlipRot( const u8 flip, const u8 rot )
	{
		FlipRot flipRot = FlipRot::None;
		if( /*0 <= flip &&*/ flip <= 3 )
		{
			flipRot = flipRot
				| ((flip & (1 << 0)) ? FlipRot::HorizontalFlip : FlipRot::None)
				| ((flip & (1 << 1)) ? FlipRot::VerticalFlip   : FlipRot::None);
		}
		if( /*0 <= rot &&*/ rot <= 3 )
		{
			flipRot = flipRot
				| ((rot & (1 << 0)) ? FlipRot::Rotate90 : FlipRot::None)
				| ((rot & (1 << 1)) ? FlipRot::Rotate45 : FlipRot::None);
		}
		return flipRot;
	}

	void MapEditorState::SetTile( const u8 layer, const Point tileIndex, const i8 color, const i8 alpha, const FlipRot flipRot, const u16 paletteIndex )
	{
		Assert( -1 <= color && color <= 63 && "Color is only capable of storing 6 bits or staying the same on -1" );
		Assert( -1 <= alpha && alpha <=  3 && "Alpha is only capable of storing 2 bits or staying the same on -1" );

		TileMap & tileMap = _tileMaps[layer];
		const u16 index1d = tileIndex.x + tileIndex.y * tileMap.size.x;
		Tile    & modTile = tileMap.tiles[index1d];

		if( color >= 0 )
			modTile.color = color;

		if( alpha >= 0 )
			modTile.alpha = alpha;

		modTile.flipRot = flipRot;
		modTile.index1  = paletteIndex;
	}

	template <typename E>
	void MapEditorState::HandleSpecificEvent( const E & ev )
	{
		// Not implemented by design
		// Assert here to catch unhandled events
		//assert( false );
	}

	template <>
	void MapEditorState::HandleSpecificEvent( const SDL_KeyboardEvent & ev )
	{
		_isCtrlHeld = ev.mod & SDL_KMOD_CTRL;

		if( !ev.down )
			return;

		switch( ev.key )
		{
			case SDLK_LEFT:  _selectedIndex--;                                break;
			case SDLK_RIGHT: _selectedIndex++;                                break;
			case SDLK_UP:    _selectedIndex-=_tileSet.tileCount.x;            break;
			case SDLK_DOWN:  _selectedIndex+=_tileSet.tileCount.x;            break;
			case SDLK_H:     _selectedFlip    ^= SDL_FLIP_HORIZONTAL;         break;
			case SDLK_V:     _selectedFlip    ^= SDL_FLIP_VERTICAL;           break;
			case SDLK_R:     _selectedRotation = (_selectedRotation + 1) % 4; break;
		}
	}

	template <>
	void MapEditorState::HandleSpecificEvent( const SDL_MouseMotionEvent & ev )
	{
		if( ev.state == 0 )
			return;

		if( _buttonHeld == 1 )
		{
			// Palette
			FPoint pos     = FPoint{ ev.x, ev.y } - _paletteOffset;
			Point  index2d = toI( pos ) / toI( toF( _tileSet.tileSize ) * _paletteScale );
			u16    index1d = index2d.x + index2d.y * _tileSet.tileCount.x;
			print( "pos:   {} {}\n", pos.x, pos.y );
			print( "index: {} {}\n", index2d.x, index2d.y );
			print( "index: {}\n", index1d );
			_selectedIndex = index1d;
		}
		else if ( _buttonHeld == 2 )
		{
			// Map
			FPoint pos     = FPoint{ ev.x, ev.y } - _viewOffset * _tileMaps[1].scrollScale;
			Point  index2d = toI( pos ) / toI( toF( _tileSet.tileSize ) * _viewScale );
			print( "pos:   {} {}\n", pos.x, pos.y );
			print( "index: {} {}\n", index2d.x, index2d.y );

			SetTile( _selectedLayer, index2d, _selectedColor, _selectedAlpha, toFlipRot( _selectedFlip, _selectedRotation ), _selectedIndex );
		}
	}

	template <>
	void MapEditorState::HandleSpecificEvent( const SDL_MouseButtonEvent & ev )
	{
		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		if( ev.button == SDL_BUTTON_LEFT && !ev.down )
		{
			_buttonHeld = 0;
		}
		else if( ev.button == SDL_BUTTON_LEFT && ev.down )
		{
			int xDivider = (windowSize.x - 512);

			if( ev.x >= xDivider )
			{
				// Right: Palette side

				const FPoint paletteTileSize = toF( _tileSet.tileSize ) * _paletteScale;
				if( ev.y > 128 )
				{
					// Down: Palette
					_buttonHeld = 1;

					FPoint pos     = FPoint { ev.x, ev.y } - _paletteOffset;
					Point  index2d = toI( pos ) / toI( paletteTileSize );
					u16    index1d = index2d.x + index2d.y * _tileSet.tileCount.x;
					print( "pos:   {} {}\n", pos.x, pos.y );
					print( "index: {} {}\n", index2d.x, index2d.y );
					print( "index: {}\n", index1d );
					_selectedIndex = index1d;
				}
				else
				{
					// Up: Modifiers
					_buttonHeld = 0; // can not hold

					const f32    spacing   = 2.f;
					const FPoint modOffset = { (f32)windowSize.x - 512, 0 };

				//	const Point sel      = { x, y };
				//	const FRect dst      = toF( sel ) * paletteTileSize * spacing + toF( modOffset ) + toWH( paletteTileSize );

					FPoint pos        = FPoint { ev.x, ev.y } - modOffset;
					Point  index2d    = toI( pos ) / toI( paletteTileSize * spacing );
					u16    index1d    = index2d.x + index2d.y * 8;
					_selectedFlip     = (index2d.x & 0b11);
					_selectedRotation = ((index2d.x & 4) != 0) + index2d.y * 2;
					print( "pos:   {} {}\n", pos.x, pos.y );
					print( "index: {} {}\n", index2d.x, index2d.y );
					print( "index: {}\n", index1d );
					//selectedIndex = index1d;
				}
			}
			else
			{
				// Left: Map
				_buttonHeld = 2;

				FPoint pos = FPoint{ev.x, ev.y} - _viewOffset * _tileMaps[1].scrollScale;
				Point index2d = toI( pos ) / toI( toF( _tileSet.tileSize ) * _viewScale );
				print( "pos:   {} {}\n", pos.x, pos.y );
				print( "index: {} {}\n", index2d.x, index2d.y );

				if( _isCtrlHeld )
				{
					//PickTile( _selectedLayer, index2d, _selectedColor, _selectedAlpha, toFlipRot( _selectedFlip, _selectedRotation ), _selectedIndex );
				}
				else
				{
					SetTile( _selectedLayer, index2d, _selectedColor, _selectedAlpha, toFlipRot( _selectedFlip, _selectedRotation ), _selectedIndex );
				}
			}
		}
	}

	template <>
	void MapEditorState::HandleSpecificEvent( const SDL_MouseWheelEvent & ev )
	{
		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const Keymod keyMod    = SDL_GetModState();
		const bool   isModded  = keyMod & SDL_KMOD_SHIFT;
		const bool   isFlipped = ev.direction == SDL_MouseWheelDirection::SDL_MOUSEWHEEL_FLIPPED;
		const f32    flipFactor= isFlipped ? -1 : 1;
		const f32    xDivider  = (f32)(windowSize.x - 512);
		const FPoint scroll    = isModded ? FPoint { ev.y * flipFactor, -ev.x } : FPoint { -ev.x, ev.y * flipFactor };

		if( ev.mouse_x >= xDivider )
		{
			// Palette
			_paletteOffset  += scroll * 16.0f;
			_paletteOffset.x = std::clamp<f32>( _paletteOffset.x, windowSize.x - _tileSet.textureSize.x * _paletteScale.x - 16, xDivider + 16 );
			_paletteOffset.y = std::clamp<f32>( _paletteOffset.y, windowSize.y - _tileSet.textureSize.y * _paletteScale.y - 16, 128 + 16 );
		}
		else
		{
			// Map
			const Point min = windowSize - toI( toF( _tileMaps[1].size * _tileSet.tileSize ) * _viewScale );
			_viewOffset     += scroll * 16.0f;
			_viewOffset.x    = std::clamp<f32>( _viewOffset.x, min.x - 512, 0 );
			_viewOffset.y    = std::clamp<f32>( _viewOffset.y, min.y      , 0 );
		}
	}

	bool MapEditorState::Input( const Event & ev )
	{
		switch( ev.type )
		{
			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
				HandleSpecificEvent( ev.key );
				break;

			case SDL_EVENT_MOUSE_MOTION:
				HandleSpecificEvent( ev.motion );
				break;

			case SDL_EVENT_MOUSE_BUTTON_UP:
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				HandleSpecificEvent( ev.button );
				break;

			case SDL_EVENT_MOUSE_WHEEL:
				HandleSpecificEvent( ev.wheel );
				break;

			case SDL_EVENT_GAMEPAD_AXIS_MOTION:
				HandleSpecificEvent( ev.gaxis );
				break;

			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
				HandleSpecificEvent( ev.gbutton );
				break;
		}

		return true; // Not really correct
	}

	void MapEditorState::Update( const u64 framesSinceStart, const Duration timeSinceStart, const f32 deltaT )
	{}

	void MapEditorState::Render( const u64 framesSinceStart, const Duration timeSinceStart, const f32 deltaTNeeded )
	{
		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		const Rect  mapClipRect  = { 0, 0, windowSize.x - 512, windowSize.y };
		const FRect mapClipRectF = toF( mapClipRect );
		SDL_SetRenderClipRect( renderer(), &mapClipRect );

		const u8     randColor        = (9 + (rand() % 55));
		const FRect  selectorOverhang = FRect{ -8, -8, 32, 32 } * toXYWH( _paletteScale );
		const Tile   selectorTile     = Tile{ TileColor { randColor, 3 }, FlipRot::None,  827 };
		const Tile   sentinelTile     = Tile{ TileColor { 9,         3 }, FlipRot::None, 1000 };
		const Tile * lastTile         = &sentinelTile;

		// Reset Color- and Alpha-Mod according to the sentinel Tile
		SDL_SetTextureColorMod( _tileSet.texture, 255, 255, 255 );
		SDL_SetTextureAlphaMod( _tileSet.texture, SDL_ALPHA_OPAQUE );

		// Draw Map
		//int drawTileCalls = 0;
		for( int l = 0; l < 2; ++l )
		{
			const TileMap & tileMap = _tileMaps[l];

			SDL_SetTextureBlendMode( _tileSet.texture, tileMap.blendMode );
			SDL_SetTextureScaleMode( _tileSet.texture, tileMap.scaleMode );

			//const Point  viewSize     = { 640, 480 };
			//const Point  startIndex   = toI( _viewOffset / tileMap.tileDist );

			const FPoint size        = toF( tileMap.tileSet->tileSize ) *  tileMap.sizeScale;
			const FPoint distScale   = tileMap.tileDist * tileMap.sizeScale;
			const FRect  sizedOffset = toFRect( _viewOffset * tileMap.scrollScale, size );
			for( int y = 0; y < tileMap.size.y; ++y )
			{
				for( int x = 0; x < tileMap.size.x; ++x )
				{
					const FPoint pos      = FPoint{ (f32)x, (f32)y } * distScale;
					const FRect  dst      = pos + sizedOffset;
//					const FRect  dst      = toXY( pt, 1 ) * toXYWH( toF( tileMap.tileSet->tileSize ) * viewScale ) + toF( viewOffset );

					// TODO: It would be better to have the start and end indexes of the x and y loops do the culling already
					if( dst.x + dst.w < 0
						|| dst.y + dst.h < 0
						|| dst.x > mapClipRectF.w
						|| dst.y > mapClipRectF.h )
						continue;

					const int    index    = x + y * tileMap.stride;
					const Tile * currTile = &tileMap.tiles[index];
					if( currTile->index() != 0 )
					{
						DrawTile( renderer(), _tileSet.texture, *currTile, *lastTile, dst, tileMap.center );
						lastTile = currTile;
						//drawTileCalls++;
					}
				}
			}
		}

		//IfDebug print( "drawtile calls: {}\n", drawTileCalls );

		const FPoint paletteTileSize = toF( _tileSet.tileSize ) * _paletteScale;

		// Draw selected colored Tile with all possible Modifiers (Flip, Rotate)
		{
			// unlock clipping
			SDL_SetRenderClipRect( renderer(), nullptr );

			SDL_SetTextureColorMod( _tileSet.texture, 255, 255, 255 );
			SDL_SetTextureAlphaMod( _tileSet.texture, SDL_ALPHA_OPAQUE );

			const f32    spacing   = 2.f;
			const FPoint modOffset = { (f32)(windowSize.x - 512 + 16), 16 };

			if( _selectedIndex != 0 )
			{
				const Color color    = HSNR64::Palette( _selectedColor );
				//const bool  isBright = color.r > 160 || color.g > 140 || color.b > 180;
				//if(!isBright)
				{
					const Color compColor = { (u8)(color.r ^ 0x80), (u8)(color.g ^ 0x80), (u8)(color.b ^ 0x80), SDL_ALPHA_OPAQUE };
					const FRect dst       = modOffset + FRect { -16, -16, 512, 128 };
					SDL_SetRenderDrawColor( renderer(), compColor.r, compColor.g, compColor.b, compColor.a );
					//SDL_SetRenderDrawColor( renderer(), 180, 180, 180, SDL_ALPHA_OPAQUE );
					SDL_RenderFillRect( renderer(), &dst );
				}

				for( int y = 0; y <= 1; ++y )
				{
					for( int x = 0; x <= 7; ++x )
					{
						const FPoint  sel       = { (f32)x, (f32)y };
						const FPoint  selScaled = sel * paletteTileSize * spacing;
						const FRect   dst      = toFRect( selScaled + modOffset, paletteTileSize );
						const FlipRot flipRot  = FlipRot::None
						                       | (((x & 1) != 0) ? HSNR64::FlipRot::HorizontalFlip : HSNR64::FlipRot::None)
						                       | (((x & 2) != 0) ? HSNR64::FlipRot::VerticalFlip   : HSNR64::FlipRot::None)
						                       | (((x & 4) != 0) ? HSNR64::FlipRot::Rotate90       : HSNR64::FlipRot::None)
						                       | ((y != 0)       ? HSNR64::FlipRot::Rotate45       : HSNR64::FlipRot::None);
						const Tile    currTile = Tile { TileColor{ (u8)_selectedColor, 3 }, flipRot, (u16)_selectedIndex };
						// Draw Cursor
						DrawTile( renderer(), _tileSet.texture, currTile, sentinelTile, dst, paletteTileSize * 0.5f );
					}
				}
			}

			_tileMaps[1].center = toF( _tileMaps[1].tileSet->tileSize ) * _tileMaps[1].sizeScale * _tileMaps[1].halfSize;

			_selectedMod         = _selectedFlip | _selectedRotation << 2;
			constexpr int stride = 8;
			const Point   sel    = { _selectedMod % stride, _selectedMod / stride };
			const FRect   dst    = toF( sel * _tileSet.tileSize ) * _paletteScale * spacing + modOffset + selectorOverhang;

			// Draw Mod Cursor
			DrawTile( renderer(), _tileSet.texture, selectorTile, sentinelTile, dst, {} );
		}

		// Draw Tile-Palette
		{
			SDL_SetTextureColorMod( _tileSet.texture, 255, 255, 255 );
			SDL_SetTextureAlphaMod( _tileSet.texture, SDL_ALPHA_OPAQUE );

			const Rect  clipRect = { windowSize.x - 512, 128, 512, windowSize.y };
			SDL_SetRenderClipRect( renderer(), &clipRect );

			const FRect palSrc = { 0, 0, (f32)_tileSet.textureSize.x, (f32)_tileSet.textureSize.y };
			const FRect palDst = toFRect( _paletteOffset, toF( _tileSet.textureSize ) * _paletteScale );

			// Draw the Palette (the whole TileSet)
			SDL_RenderTexture( renderer(), _tileSet.texture, &palSrc, &palDst );

			const int   stride   = _tileSet.tileCount.x;
			const Point sel      = { _selectedIndex % stride, _selectedIndex / stride };
			//const FRect overhang = FRect{ -8, -8, 32, 32 } * toXYWH( _paletteScale );
			const FRect dst      = toF( sel ) * paletteTileSize + _paletteOffset + selectorOverhang;

			const Rect clipRect2 = {
				(windowSize.x - 512) + (int)selectorOverhang.x,
				128 + (int)selectorOverhang.y,
				512 - (int)selectorOverhang.x,
				windowSize.y };
			SDL_SetRenderClipRect( renderer(), &clipRect2 );

			// Draw Palette Cursor
			DrawTile( renderer(), _tileSet.texture, selectorTile, sentinelTile, dst, {} );
		}

		// unlock clipping
		SDL_SetRenderClipRect( renderer(), nullptr );

		if(false)
		{
			const FRect
				src = { 16 * 16, 39 * 16, 3 * 16, 3 * 16 },
				dst = { 100, 100, 400, 400 };
			SDL_RenderTexture9Grid( renderer(), _tileSet.texture, &src, 16, 16, 16, 16, 4, &dst );
		}
	}

#if USE_IMGUI

	void MapEditorState::RenderUI( const u64 framesSinceStart, const Duration timeSinceStart, const float deltaTNeeded )
	{
		static bool autoUpdate = false;
		static bool drawColorNumber = false;

		//ImGuiIO & io = ImGui::GetIO();
		ImGui::Begin( "MapEditor", nullptr, ImGuiWindowFlags_NoFocusOnAppearing );
		if( framesSinceStart == 0 )
			ImGui::SetWindowFocus( nullptr );

		const u8     randColor        = (10 + (rand() % 36));

		//if( ImGui::SliderInt( "int", &p.x, 0, 320 ) && auto_update )
			//_blendedText = nullptr;
		//	nullptr;

		ImGui::Checkbox( "Auto-Redraw", &autoUpdate );     // Edit bools storing our window open/close state

		if( ImGui::Button( "Save" ) )                     // Buttons return true when clicked (most widgets return true when edited/activated)
			SaveMap();

		ImGui::SliderFloat2( "Blah", (float*)(void*)&_viewScale, 0.1, 8.0 );

		if( ImGui::SliderInt( "Layer", &_selectedLayer, 0, 4 ) ) { }

		if( ImGui::SliderInt( "Color", &_selectedColor, -1, 63 ) ) { }

		//ImGui::Checkbox( "Keep Alpha", &isKeepAlpha );
		if( ImGui::SliderInt( "Alpha", &_selectedAlpha, -1, 3 ) ) { }

		if( ImGui::SliderInt( "Flip", &_selectedFlip, -1, 3 ) ) { }

		if( ImGui::SliderInt( "Rotation", &_selectedRotation, -1, 3 ) ) { }

		if( ImGui::Button( "Clear" ) )
		{
			FillLayer( _selectedLayer, emptyTile );
		}

		ImGui::SameLine();
		if( ImGui::Button( "Reset Layer to uniform" ) )
		{
			const TileColor color = { (u8)_selectedColor, (u8)(_selectedAlpha) };
			FillLayer( _selectedLayer, uniformTile( color, FlipRot::None, 1000 ) );
		}

		ImGui::SameLine();
		if( ImGui::Button( "... random Color" ) )
		{
			FillLayer( _selectedLayer, randomSolidTile );
			SDL_SetCursor( SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_MOVE ) );
		}

		ImGui::SameLine();
		if( ImGui::Button( "... & Index" ) )
		{
			FillLayer( _selectedLayer, randomTile );
			SDL_SetCursor( SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_CROSSHAIR ) );
		}


		ImGui::Checkbox( "Draw Color Number", &drawColorNumber );

		ImGui::PushID( "fg color" );
		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );
		// CARE: ImU32 as color is 0XAABBGGRR - opposite of what might be expected
		ImGui::PushStyleColor( ImGuiCol_Border, 0xAAFFFFFF );

		using namespace StringViewLiterals;
		const std::string_view prefix = drawColorNumber
			? ""sv      // draws number                             "23"
			: "  ##"sv; // draws spaces, names internally by number "  ##23"

		for( int i = 0; i < 64; ++i )
		{
			//ImU32 rcol = std::bit_cast<ImU32>( HSNR64::Palette( randColor ) );
			ImU32 pcol = std::bit_cast<ImU32>( HSNR64::Palette( i ) );
			ImU32 bcol = std::bit_cast<ImU32>( HSNR64::Palette( _selectedColor == i ? randColor : i ) );
			//Color color = hsnr64::Palette[i];
			ImGui::PushStyleColor( ImGuiCol_Button,       pcol );
			ImGui::PushStyleColor( ImGuiCol_Border,       bcol );
			ImGui::PushStyleColor( ImGuiCol_BorderShadow, bcol );
			ImGui::PushStyleColor( ImGuiCol_Text,         pcol ^ 0x00808080 );
			if( ImGui::Button( format( "{}{:02}", prefix, i ).c_str() ) )
				_selectedColor = i;
			ImGui::PopStyleColor( 4 );
			//ImGui::ColorButton( format( "color{}", i ).c_str(), *((ImVec4*)&sor::hsnr64::Palette[i]), ImGuiColorEditFlags_Uint8 );
			//if(i%10 != 0)
			if( !ColorNeedsLinebreak( i ) )
				ImGui::SameLine();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::NewLine();

		ImGui::End();
	}

#endif

	void MapEditorState::SaveMap() const
	{
		// TODO: Show filepicker to to allow custom naming
		const char * filename = "map-i-map.json5"; // json5 is a relaxed version of json
		File * file = std::fopen( filename, "w" );
		if( file == nullptr )
			throw system_error( errno, "cannot open file '{}'", filename );

		print( file,
			"{{"
			"\n\ttileByteSize: {},"
			"\n\tmap: {{"
			"\n\t\tlayers: [ ",
			sizeof( Tile ) );

		for( int l = 0; l < 2; ++l )
		{
			const TileMap & tileMap = _tileMaps[l];

			if( l != 0 )
				print( file, ",\n\t\t" );

			print( file,
				"{{"
				"\n\t\t\tnum: {0},"
				"\n\t\t\tsize: [ {1}, {2} ],"
				"\n\t\t\ttiles:"
				,
				l,
				tileMap.size.x,
				tileMap.size.y,
				nullptr );

			//const Point  viewSize     = { 640, 480 };
			//const Point  startIndex   = toI( _viewOffset / tileMap.tileDist );

			print( file, " [" );
			for( int y = 0; y < tileMap.size.y; ++y )
			{
				if( y == 0 ) [[unlikely]]
					print( file, "\n\t\t\t\t[" );
				else
					print( file, ",\n\t\t\t\t[" );

				for( int x = 0; x < tileMap.size.x; ++x )
				{
					constexpr u32 mask = (sizeof(Tile) == 3) ? 0x00ffffff : 0xffffffff;
					const int    index    = x + y * tileMap.stride;
					const Tile * currTile = &tileMap.tiles[index];
					if( x == 0 ) [[unlikely]]
					{
						u32 tileData = mask & *reinterpret_cast<const u32 *>( currTile );
						print( file, "{:#x}", tileData );
					}
					else
					{
						print( file, ",{:#x}", mask & *reinterpret_cast<const u32 *>( currTile ) );
					}
				}
				print( file, "]" );
			}
			print( file,
				"\n\t\t\t]"
				"\n\t\t}}" );
		}

		print( file,
			" ]"
			"\n\t}}"
			"\n}}" );

		std::fclose( file );
	}

}
