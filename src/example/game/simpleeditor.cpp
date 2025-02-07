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
			Owned<Surface> surf = IMG_Load( BasePathGraphic "bd.png" );
			_tileSet     = SDL_CreateTextureFromSurface( renderer(), surf );
			_tileSetSize = { surf->w, surf->h };
			_tileCount   = { 9, 11 };
			_tileSize    = _tileSetSize / _tileCount;
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

		_camera       = Point{ 100, 100 };
		_paletteScale = (i32)_game.scalingFactor();
	}

	void EditorState::Destroy()
	{
		// for example, do not unload, just keep it loaded
	}

	bool EditorState::HandleEvent( const Event & evt )
	{
		switch( evt.type )
		{
			case SDL_KEYDOWN:
			{
				const SDL_KeyboardEvent & kd = evt.key;
				const SDL::Keysym what_key = kd.keysym;
				if( what_key.scancode == SDL_SCANCODE_ESCAPE )
				{
					//game.SetNextState( 0 ); // back to intro
				}
				else if( what_key.scancode == SDL_SCANCODE_F1 && kd.repeat == 0 ) { _mapScale = 1; }
				else if( what_key.scancode == SDL_SCANCODE_F2 && kd.repeat == 0 ) { _mapScale = 2; }
				else if( what_key.scancode == SDL_SCANCODE_F3 && kd.repeat == 0 ) { _mapScale = 3; }
				else if( what_key.scancode == SDL_SCANCODE_F4 && kd.repeat == 0 ) { _mapScale = 4; }
				else if( what_key.scancode == SDL_SCANCODE_F5 && kd.repeat == 0 )
				{
					_paletteScale = _paletteScale == 1 ? 2 : 1;
				}
				else if( what_key.scancode == SDL_SCANCODE_F6 && kd.repeat == 0 )
				{
					_showGrid = !_showGrid;
				}
				else if( what_key.scancode == SDL_SCANCODE_F8 && kd.repeat == 0 )
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

			case SDL_MOUSEBUTTONDOWN:
			{
				const SDL_MouseButtonEvent & me = evt.button;
				if( me.button == 1 )
				{
					const Point mousePos = { me.x, me.y };
					if( mousePos.x < _tileSetSize.x * _paletteScale
					 && mousePos.y < _tileSetSize.y * _paletteScale )
					{
						const Point p = mousePos / (_tileSize * _paletteScale);
						// pick
						_pickedIdx = p;
					}
					else
					{
						const Point p = (mousePos - _camera) / (_tileSize * _mapScale);
						// paint
						//level[p.x][p.y] = pickedIdx;
						if( p.y >= 0 && p.y < (*_currState).size()
						 && p.x >= 0 && p.x < (*_currState)[0].size() )
						{
							(*_currState)[p.y][p.x] = _pickedIdx.x + _pickedIdx.y * _tileCount.x;
						}
						_isPainting = true;
					}
				}
				else if( me.button == 3 )
				{
					_isPanning = true;
				}
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				const SDL_MouseButtonEvent & me = evt.button;
				if( me.button == 1 )
				{
					_isPainting = false;
				}
				else if( me.button == 3 )
				{
					_isPanning = false;
				}
				break;
			}

			case SDL_MOUSEMOTION:
			{
				const SDL_MouseMotionEvent & me = evt.motion;
				if( _isPainting )
				{
					const Point mousePos = { me.x, me.y };
					const Point p = (mousePos - _camera) / _tileSize / _mapScale;
					//level[p.x][p.y] = pickedIdx;
					if( p.y >= 0 && p.y < (*_currState).size()
					 && p.x >= 0 && p.x < (*_currState)[0].size() )
					{
						(*_currState)[p.y][p.x] = _pickedIdx.x + _pickedIdx.y * _tileCount.x;
					}
				}
				else if( _isPanning )
				{
					_camera += Point{ me.xrel, me.yrel };
				}
				break;
			}
		}

		return true;
	}

	void EditorState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		if( msSinceStart < _nextUpdateTimeMS )
			return;

		const WorldState & curr = *_currState;
		WorldState & next = *_nextState;

		for( uint y = 0; y < curr.size(); ++y )
		{
			for( uint x = 0; x < curr[y].size(); ++x )
			{
				const int index = curr[y][x];
				Point tileIndex = { index % _tileCount.x, index / _tileCount.x };

				if( tileIndex.y >= 1 && tileIndex.y <= 5 || tileIndex.y == 8 || tileIndex.y == 10 )
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

		_nextUpdateTimeMS += UpdateDeltaTimeMS;

		std::swap( _currState, _nextState );
	}

	void EditorState::Render( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaTNeeded )
	{
		const WorldState & curr = *_currState;

		const Point mapTileSize = _tileSize * _mapScale;
		const Point levelSize   = { (int)curr[0].size(), (int)curr.size() };


		/// Draw level
		for( int y = 0; y < curr.size(); ++y )
		{
			for( int x = 0; x < curr[y].size(); ++x )
			{
				const int   index     = curr[y][x];
				const Point tileIndex = { index % _tileCount.x, index / _tileCount.x };
				const Point pos       = Point{ x, y } * mapTileSize + _camera;
				const Rect  srcRect   = toRect( tileIndex * _tileSize, _tileSize );
				const Rect  dstRect   = toRect( pos, mapTileSize );
				SDL_RenderCopy( renderer(), _tileSet, &srcRect, &dstRect );
			}
		}

		SDL_SetRenderDrawColor( renderer(), 255, 0, 0, SDL_ALPHA_OPAQUE );
		const Rect borderRect = toRect( _camera, levelSize * mapTileSize );
		SDL_RenderDrawRect( renderer(), &borderRect );


		/// Draw Grid (enable via F1)
		// TODO: This is super slow, can be solved way more efficiently
		if( _showGrid )
		{
			SDL_SetRenderDrawColor( renderer(), 0, 0, 0, 63 ); // grid color
			SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND );

			for( int y = 0; y < curr.size(); ++y )
			{
				for( int x = 0; x < curr[y].size(); ++x )
				{
					const Point gridPos  = Point{ x, y } * mapTileSize + _camera;
					const Rect  gridRect = toRect( gridPos, mapTileSize );
					SDL_RenderDrawRect( renderer(), &gridRect );
				}
			}
		}


		/// Draw Palette (top left of screen)
		{
			const Point paletteTileSize = _tileSize * _paletteScale;
			const Rect  paletteRect     = toRect( { 0, 0 }, paletteTileSize * _tileCount );
			const Point pickerPosition  = paletteTileSize * _pickedIdx;
			const Rect  pickerRect      = toRect( pickerPosition, paletteTileSize );

			SDL_SetRenderDrawColor( renderer(), 0, 0, 0, 255 );
			SDL_RenderFillRect( renderer(), &paletteRect );

			SDL_RenderCopy( renderer(), _tileSet, nullptr, &paletteRect );

			//auto blink = (msSinceStart & 0x300) >> 8;
			const usize blink = (framesSinceStart & 0x3f) >> 3;
			const Color & c = BaseColors[blink];
			SDL_SetRenderDrawColor( renderer(), c.r, c.g, c.b, c.a );
			SDL_RenderDrawRect( renderer(), &pickerRect );
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
				constexpr const Color c = { 255, 255, 255, 255 };
				//SDL::C::TTF_SetFontHinting(font, ((t & 0x600) >> 9));
				//SDL::C::TTF_SetFontOutline(font, 1);
				Owned<Surface> surf = TTF_RenderUTF8_Blended_Wrapped( _font, oss.str().c_str(), c, 400 * _game.scalingFactor() );
				Owned<Texture> t2   = SDL_CreateTextureFromSurface( renderer(), surf );
				SDL_SetTextureColorMod( t2, 0, 0, 0 );
				//SDL::C::SDL_SetTextureBlendMode(t2, SDL::C::SDL_BLENDMODE_BLEND);
				const Point p = { _tileSetSize.x * _paletteScale + 30, 20 };
				for( const Point & pd : HSNR64::ShadowOffset::Cross )
				{
					const Rect dstRect = toRect( p + pd, Point{ surf->w, surf->h } );
					SDL_RenderCopy( renderer(), t2, nullptr, &dstRect );
				}
				SDL_SetTextureColorMod( t2, 255, 255, 255 );
				const Rect dstRect = toRect( p, Point{ surf->w, surf->h } );
				SDL_RenderCopy( renderer(), t2, nullptr, &dstRect );
			}
		}
	}
}
