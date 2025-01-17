#include "simpleeditor.hpp"

#include "hsnr64/offset.hpp"

namespace JanSordid::SDL_Example
{
	void EditorState::Init()
	{
		if( !font )
			font = TTF_OpenFont( BasePathFont "RobotoSlab-Bold.ttf", 9 * _game.scalingFactor() );

		if( !tileSet )
		{
			//tileSet = IMG_LoadTexture( renderer(), "../assets/graphics/streets.png" );
			Owned<Surface> surf = IMG_Load( BasePathGraphic "bd.png" );
			tileSetSize = { surf->w, surf->h };
			tileSet     = SDL_CreateTextureFromSurface( renderer(), surf );
		}

		if( doGenerateEmptyMap )
		{
			constexpr const int
				dirt     = 1 + 7 * TileCount.x,
				hardWall = 1 + 6 * TileCount.x;

			for( auto & row : *currState )
			{
				row.fill( dirt );
				*row.begin() = hardWall;
				*row.rbegin() = hardWall;
			}
			(*currState->begin()).fill( hardWall );
			(*currState->rbegin()).fill( hardWall );
		}

		camera = Point{ 100, 100 };
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
				SDL::Keysym what_key = kd.keysym;
				if( what_key.scancode == SDL_SCANCODE_ESCAPE )
				{
					//game.SetNextState( 0 ); // back to intro
				}
				else if( what_key.scancode == SDL_SCANCODE_F1 && kd.repeat == 0 )
				{
					showGrid = !showGrid;
				}
				else if( what_key.scancode == SDL_SCANCODE_F8 && kd.repeat == 0 )
				{
					const WorldState & curr = *currState;
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
					if( me.x < tileSetSize.x && me.y < tileSetSize.y )
					//if( me.x < 320 && me.y < 320 )
					{
						const Point p = Point( me.x, me.y ) / TileSize;
						// pick
						pickedIdx = p;
					}
					else
					{
						const Point p = (Point( me.x, me.y ) - camera) / TileSize;
						// paint
						//level[p.x][p.y] = pickedIdx;
						(*currState)[p.y][p.x] = pickedIdx.x + pickedIdx.y * TileCount.x;
						isPainting = true;
					}
				}
				else if( me.button == 3 )
				{
					isPanning = true;
				}
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				const SDL_MouseButtonEvent & me = evt.button;
				if( me.button == 1 )
				{
					isPainting = false;
				}
				else if( me.button == 3 )
				{
					isPanning = false;
				}
				break;
			}

			case SDL_MOUSEMOTION:
			{
				const SDL_MouseMotionEvent & me = evt.motion;
				if( isPainting )
				{
					const Point p = (Point( me.x, me.y ) - camera) / TileSize;
					//level[p.x][p.y] = pickedIdx;
					if( p.y >= 0 && p.y < (*currState).size()
					 && p.x >= 0 && p.x < (*currState)[0].size() )
					{
						(*currState)[p.y][p.x] = pickedIdx.x + pickedIdx.y * TileCount.x;
					}
				}
				else if( isPanning )
				{
					camera += Point( me.xrel, me.yrel );
				}
				break;
			}
		}

		return true;
	}

	constexpr const static u32 UpdateDeltaTicks = 100;

	u32 nextUpdateTicks = 0;
	u32 updateFrames    = 0;

	void EditorState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
		const u32 currTicks = SDL_GetTicks();
		if( currTicks < nextUpdateTicks )
			return;

		const WorldState & curr = *currState;
		WorldState & next = *nextState;

		for( uint y = 0; y < curr.size(); ++y )
		{
			for( uint x = 0; x < curr[y].size(); ++x )
			{
				const int s = curr[y][x];
				// SDL_Point
				Point pt = { s % TileCount.x, s / TileCount.x };

				if( pt.y >= 1 && pt.y <= 5 || pt.y == 8 || pt.y == 10 )
				{
					pt.x = (pt.x + 1) % 8;
				}
				else if( pt.y == 9 )
				{
					pt.x = ((pt.x + 1) % 4) + (pt.x / 4 * 4);
				}
				const int next_s = pt.x + pt.y * TileCount.x;
				next[y][x] = next_s;
			}
		}

		nextUpdateTicks += UpdateDeltaTicks;
		updateFrames++;

		std::swap( currState, nextState );
	}

	void EditorState::Render( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaTNeeded )
	{
		// draw level

		const WorldState & level = *currState;

		const int w_rel = tileSetSize.x / TileCount.x;
		const int h_rel = tileSetSize.y / TileCount.y;

		for( int y = 0; y < level.size(); ++y )
		{
			for( int x = 0; x < level[y].size(); ++x )
			{
				/*tileSet.DrawSprite(
					Point( x * TILESIZE, y * TILESIZE ) + camera,
					Point( 9, 9 ),
					level[x][y],
					0.5f );*/

				const int   s  = level[y][x];
				const Point pt = { s % TileCount.x, s / TileCount.x };
				const Point pos = Point( x, y ) * TileSize + camera;
				const Rect  src = Rect( w_rel * pt.x, h_rel * pt.y, w_rel, h_rel );
				const Rect  dst = Rect( pos.x, pos.y, w_rel, h_rel );
				SDL_RenderCopy( renderer(), tileSet, &src, &dst );
			}
		}

		SDL_SetRenderDrawColor( renderer(), 255, 0, 0, SDL_ALPHA_OPAQUE );
		Rect border = Rect( camera.x, camera.y, TileSize * level[0].size(), TileSize * level.size() );

		SDL_RenderDrawRect( renderer(), &border );

		// Draw grid on F1
		// TODO: This is super slow, can be solved way more efficiently
		if( showGrid )
		{
			SDL_SetRenderDrawColor( renderer(), 0, 0, 0, 63 ); // grid color
			SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND );

			for( uint y = 0; y < level.size(); ++y )
			{
				for( uint x = 0; x < level[y].size(); ++x )
				{
					Rect grid = Rect( x * TileSize, y * TileSize, TileSize, TileSize ) + camera;
					SDL_RenderDrawRect( renderer(), &grid );
				}
			}
		}

		Rect box = Rect( 0, 0, TileCount.x * TileSize, TileCount.y * TileSize );
		SDL_SetRenderDrawColor( renderer(), 0, 0, 0, 255 );
		SDL_RenderFillRect( renderer(), &box );

		{
			//u32 t = SDL::C::SDL_GetTicks();
			//auto blink = (t & 0x300) >> 8;
			const u64 blink = (framesSinceStart & 0x3f) >> 3;
			const Color & c = BaseColors[blink];
			SDL_SetRenderDrawColor( renderer(), c.r, c.g, c.b, c.a );
			Rect dst = toWH( tileSetSize );
			SDL_RenderCopy( renderer(), tileSet, nullptr, &dst );

			Rect picker = Rect( pickedIdx.x * TileSize, pickedIdx.y * TileSize, TileSize, TileSize );
			SDL_RenderDrawRect( renderer(), &picker );
		}

		// cheapo benchmarking
		//for (uint x = 0; x < 100; ++x)
		{
			std::ostringstream oss;
			const char * text = "Mapeditor:\n\nLinksklick: Oben links im Bild selektiert das zu zeichnende Tile.\n"
			                    "Linksklick: Im Rest des Bildschirms platziert die Tile (nur im roten Rahmen).\n"
			                    "Rechte Maustaste halten: Karte läßt sich mit der Maus bewegen.\n\n";
			oss << text
			    << deltaTNeeded * 1000.0f
			    << "ms";
			{
				Color c( 255, 255, 255, 255 );
				//SDL::C::TTF_SetFontHinting(font, ((t & 0x600) >> 9));
				//SDL::C::TTF_SetFontOutline(font, 1);
				Owned<Surface> surf = TTF_RenderUTF8_Blended_Wrapped( font, oss.str().c_str(), c, 400 * _game.scalingFactor() );
				Owned<Texture> t2   = SDL_CreateTextureFromSurface( renderer(), surf );
				SDL_SetTextureColorMod( t2, 0, 0, 0 );
				//SDL::C::SDL_SetTextureBlendMode(t2, SDL::C::SDL_BLENDMODE_BLEND);
				const Point p( 360, 20 );
				for( const Point & pd : HSNR64::ShadowOffset::Cross )
				{
					Rect dst = (p + pd) + toWH( Point{ surf->w, surf->h } );
					SDL_RenderCopy( renderer(), t2, nullptr, &dst );
				}
				SDL_SetTextureColorMod( t2, 255, 255, 255 );
				Rect dst = p + toWH( Point{ surf->w, surf->h } );
				SDL_RenderCopy( renderer(), t2, nullptr, &dst );
			}
		}
	}
}
