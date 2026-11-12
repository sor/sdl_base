#include "simpleeditor.hpp"

#include "hsnr64/offset.hpp"

namespace JanSordid::SDL_Example
{
	void EditorState::Init()
	{
		if( !_font )
			_font = TTF_OpenFont( BasePathFont "RobotoSlab-Bold.ttf", 9 * _game.scalingFactor() );

		if( !_tileSet )
		{
			Owned<SDL_Surface> surf = IMG_Load( BasePathGraphic "bd.png" );
			_tileSet     = SDL_CreateTextureFromSurface( renderer(), surf );
			_tileSetSize = { surf->w, surf->h };
			_tileCount   = { 9, 11 };
			_tileSize    = _tileSetSize / _tileCount;

			SDL_SetTextureScaleMode( _tileSet, SDL_ScaleMode::SDL_SCALEMODE_NEAREST );
		}

		if( _doGenerateEmptyMap )
		{
			const int
				Dirt     = 1 + 7 * _tileCount.x,
				HardWall = 1 + 6 * _tileCount.x;

			for( auto & row : *_currState )
			{
				row.fill( Dirt );
				*row.begin() = HardWall;
				*row.rbegin() = HardWall;
			}
			(*_currState->begin()).fill( HardWall );
			(*_currState->rbegin()).fill( HardWall );
		}

		_camera       = SDL_FPoint{ 100, 100 };
		_paletteScale = (i32)_game.scalingFactor();
	}

	void EditorState::Destroy()
	{
		// for example, do not unload, just keep it loaded
	}

	bool EditorState::Input( const SDL_Event & evt )
	{
		switch( evt.type )
		{
			case SDL_EVENT_KEY_DOWN:
			{
				const SDL_KeyboardEvent & key = evt.key;
				if( key.scancode == SDL_SCANCODE_ESCAPE )
				{
					//game.SetNextState( 0 ); // back to intro
				}
				else if( key.scancode == SDL_SCANCODE_F1 && key.repeat == 0 ) { _mapScale = 1; }
				else if( key.scancode == SDL_SCANCODE_F2 && key.repeat == 0 ) { _mapScale = 2; }
				else if( key.scancode == SDL_SCANCODE_F3 && key.repeat == 0 ) { _mapScale = 3; }
				else if( key.scancode == SDL_SCANCODE_F4 && key.repeat == 0 ) { _mapScale = 4; }
				else if( key.scancode == SDL_SCANCODE_F5 && key.repeat == 0 )
				{
					_paletteScale = _paletteScale == 1 ? 2 : 1;
				}
				else if( key.scancode == SDL_SCANCODE_F6 && key.repeat == 0 )
				{
					_showGrid = !_showGrid;
				}
				else if( key.scancode == SDL_SCANCODE_F8 && key.repeat == 0 )
				{
					const WorldState & curr = *_currState;
					print( "\n{{{{ // these double curly braces are necessary for std::array to work" );
					for( uint y = 0; y < curr.size(); ++y )
					{
						print( "\n\t{{ " );
						for( uint x = 0; x < curr[y].size(); ++x )
						{
							// Export current worldstate to shell, for copy and paste to cpp file "load and save" functionality
							print( "{},", curr[y][x] );
						}
						print( "\b \b" );

						print( " }}," );
					}
					print( "\b \b" );
					print( "\n}}}};\n" );
				}
				break;
			}

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				const SDL_MouseButtonEvent & me = evt.button;
				if( me.button == SDL_BUTTON_LEFT )
				{
					const SDL_FPoint mousePos = { me.x, me.y };
					if( mousePos.x < _tileSetSize.x * _paletteScale
					 && mousePos.y < _tileSetSize.y * _paletteScale )
					{
						// Pick
						const SDL_Point p = toI( mousePos ) / (_tileSize * _paletteScale);
						_pickedIdx = p;
					}
					else
					{
						// Paint
						const SDL_Point p = toI( mousePos - _camera ) / (_tileSize * _mapScale);
						//level[p.x][p.y] = pickedIdx;
						if( 0 <= p.y && (uint)p.y < (*_currState).size()
						 && 0 <= p.x && (uint)p.x < (*_currState)[0].size() )
						{
							(*_currState)[p.y][p.x] = _pickedIdx.x + _pickedIdx.y * _tileCount.x;
						}
						_isPainting = true;
					}
				}
				else if( me.button == SDL_BUTTON_RIGHT )
				{
					_isPanning = true;
				}
				break;
			}

			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				const SDL_MouseButtonEvent & me = evt.button;
				if( me.button == SDL_BUTTON_LEFT )
				{
					_isPainting = false;
				}
				else if( me.button == SDL_BUTTON_RIGHT )
				{
					_isPanning = false;
				}
				break;
			}

			case SDL_EVENT_MOUSE_MOTION:
			{
				const SDL_MouseMotionEvent & me = evt.motion;
				if( _isPainting )
				{
					const SDL_FPoint mousePos = { me.x, me.y };
					const SDL_Point  p = toI( mousePos - _camera ) / _tileSize / _mapScale;
					//level[p.x][p.y] = pickedIdx;
					if( 0 <= p.y && (uint)p.y < (*_currState).size()
					 && 0 <= p.x && (uint)p.x < (*_currState)[0].size() )
					{
						(*_currState)[p.y][p.x] = _pickedIdx.x + _pickedIdx.y * _tileCount.x;
					}
				}
				else if( _isPanning )
				{
					_camera += SDL_FPoint{ me.xrel, me.yrel };
				}
				break;
			}
		}

		return true;
	}

	void EditorState::Update( const u64 framesSinceStart, const Duration timeSinceStart, const f32 deltaT )
	{
		if( timeSinceStart < _nextUpdateTime )
			return;

		const WorldState & curr = *_currState;
		WorldState & next = *_nextState;

		for( uint y = 0; y < curr.size(); ++y )
		{
			for( uint x = 0; x < curr[y].size(); ++x )
			{
				const int index = curr[y][x];
				SDL_Point tileIndex = { index % _tileCount.x, index / _tileCount.x };

				if( (tileIndex.y >= 1 && tileIndex.y <= 5) || tileIndex.y == 8 || tileIndex.y == 10 )
				{
					tileIndex.x = (tileIndex.x + 1) % 8;
				}
				else if( tileIndex.y == 9 )
				{
					tileIndex.x = ((tileIndex.x + 1) % 4) + (tileIndex.x / 4 * 4);
				}

				const int nextIndex = tileIndex.x + tileIndex.y * _tileCount.x;
				next[y][x] = nextIndex;
			}
		}

		_nextUpdateTime += UpdateDeltaTime;

		std::swap( _currState, _nextState );
	}

	void EditorState::Render( const u64 framesSinceStart, const Duration timeSinceStart, const f32 deltaTNeeded )
	{
		const WorldState & curr = *_currState;

		const SDL_FPoint mapTileSize = toF( _tileSize * _mapScale );
		const SDL_FPoint levelSize   = { (f32)curr[0].size(), (f32)curr.size() };


		/// Draw level
		for( uint y = 0; y < curr.size(); ++y )
		{
			for( uint x = 0; x < curr[y].size(); ++x )
			{
				const int        index     = curr[y][x];
				const SDL_Point  tileIndex = { index % _tileCount.x, index / _tileCount.x };
				const SDL_FPoint pos       = SDL_FPoint{ (f32)x, (f32)y } * mapTileSize + _camera;
				const SDL_FRect  srcRect   = toFRect( toF( tileIndex * _tileSize ), toF( _tileSize ) );
				const SDL_FRect  dstRect   = toFRect( pos, mapTileSize );
				SDL_RenderTexture( renderer(), _tileSet, &srcRect, &dstRect );
			}
		}

		SDL_SetRenderDrawColor( renderer(), 255, 0, 0, SDL_ALPHA_OPAQUE );
		const SDL_FRect borderRect = toFRect( _camera, levelSize * mapTileSize );
		SDL_RenderRect( renderer(), &borderRect );


		/// Draw Grid (enable via F1)
		// TODO: This is super slow, can be solved way more efficiently
		if( _showGrid )
		{
			SDL_SetRenderDrawColor( renderer(), 0, 0, 0, 63 ); // grid color
			SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND );

			for( uint y = 0; y < curr.size(); ++y )
			{
				for( uint x = 0; x < curr[y].size(); ++x )
				{
					const SDL_FPoint gridPos  = SDL_FPoint{ (f32)x, (f32)y } * mapTileSize + _camera;
					const SDL_FRect  gridRect = toFRect( gridPos, mapTileSize );
					SDL_RenderRect( renderer(), &gridRect );
				}
			}
		}


		/// Draw Palette (top left of screen)
		{
			const SDL_Point paletteTileSize = _tileSize * _paletteScale;
			const SDL_FRect paletteRect     = toWH( toF( paletteTileSize * _tileCount ) );
			const SDL_Point pickerPosition  = paletteTileSize * _pickedIdx;
			const SDL_FRect pickerRect      = toFRect( toF( pickerPosition ), toF( paletteTileSize ) );

			SDL_SetRenderDrawColor( renderer(), 0, 0, 0, 255 );
			SDL_RenderFillRect( renderer(), &paletteRect );

			SDL_RenderTexture( renderer(), _tileSet, EntireFRect, &paletteRect );

			//auto blink = (msSinceStart & 0x300) >> 8;
			const usize blink = (framesSinceStart & 0x3f) >> 3;
			const SDL_Color & c = BaseColors[blink];
			SDL_SetRenderDrawColor( renderer(), c.r, c.g, c.b, c.a );
			SDL_RenderRect( renderer(), &pickerRect );
		}

		/// Draw Help Text
		//for (uint x = 0; x < 100; ++x) // <- uncomment for cheapo benchmarking
		{
			std::ostringstream oss;
			const char * text = "--== Map Editor ==--\n\n"
			                    "Click left mouse button @ top-left: Selects the tile to draw.\n"
			                    "Click left mouse button @ rest of the screen: Places the tile (only in the red border).\n"
			                    "Hold right mouse button: Pan the map via mouse movement.\n"
			                    "F1-F4: Set zoom level of the map to 1x-4x\n"
			                    "F5: Toggle between palette scale 1x or 2x\n"
			                    "F6: Toggle grid\n"
			                    "F8: Save map to stdout\n\n";
			oss << text
				<< "current zoom level: "
				<< _mapScale
				<< "\n"
			    << deltaTNeeded * 1000.0f
			    << "ms";
			{
				constexpr SDL_Color c = { 255, 255, 255, 255 };
				//SDL::C::TTF_SetFontHinting(font, ((t & 0x600) >> 9));
				//SDL::C::TTF_SetFontOutline(font, 1);
				Owned<SDL_Surface> surf = TTF_RenderText_Blended_Wrapped( _font, oss.str().c_str(), 0, c, 400 * _game.scalingFactor() );
				Owned<SDL_Texture> t2   = SDL_CreateTextureFromSurface( renderer(), surf );
				SDL_SetTextureColorMod( t2, 0, 0, 0 );
				//SDL_SetTextureBlendMode( t2, SDL_BLENDMODE_BLEND );
				const SDL_FPoint p = { (f32)_tileSetSize.x * _paletteScale + 30, 20 };
				for( const SDL_FPoint & offset : HSNR64::ShadowOffset::Cross )
				{
					const SDL_FRect dstRect = toFRect( p + offset, SDL_FPoint{ (f32)surf->w, (f32)surf->h } );
					SDL_RenderTexture( renderer(), t2, EntireFRect, &dstRect );
				}
				SDL_SetTextureColorMod( t2, 255, 255, 255 );
				const SDL_FRect dstRect = toFRect( p, SDL_FPoint{ (f32)surf->w, (f32)surf->h } );
				SDL_RenderTexture( renderer(), t2, EntireFRect, &dstRect );
			}
		}
	}
}
