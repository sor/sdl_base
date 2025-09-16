#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

#include "global.hpp"

//#include "sor/sdl_shapeops.hpp"


// Global/GameState/AppState wäre hier vielleicht auch angebracht, WorldState/LevelState erst ab "wall"
struct WorldState
{
	SDL_Window   * window;
	SDL_Renderer * renderer;
	SDL_Texture  * texture;

	SDL_FRect  wall     = { 200, 200, 200, 200 };
	SDL_FRect  ball     = { 20, 20, 20, 20 };
	SDL_FPoint vel_ball = { 144.0, 99.0 };
	SDL_FPoint mouse_pos;
	//SDL_FPoint vel_ball = { 0.4, 0.1 };

	bool moveToRight;
};

void initWorldState( WorldState & ws )
{
	SDL_Init( SDL_INIT_EVENTS | SDL_INIT_VIDEO );

	// 640x360  16:9    >80%
	// 640x400  16:10   ~5%  or 576x360
	// 640x480   4:3    <5%  or 480x360
	constexpr int windowScale = 2;
	ws.window   = SDL_CreateWindow( "Pong Game", 640 * windowScale, 360 * windowScale, SDL_WINDOW_OPENGL );
	ws.renderer = SDL_CreateRenderer( ws.window, nullptr );
	ws.texture  = IMG_LoadTexture( ws.renderer, "../../../asset/graphic/space_merc.png" );

	SDL_SetRenderVSync( ws.renderer, 1 );
	//SDL_SetRenderVSync( ws.renderer, SDL_RENDERER_VSYNC_ADAPTIVE );
	SDL_SetTextureScaleMode( ws.texture, SDL_SCALEMODE_NEAREST );


//	SDL_SetHint( SDL_HINT_MOUSE_RELATIVE_SCALING,   "1"        );
//	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY,     "nearest"  );

//	SDL_SetHint( SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "overscan" );

//	SDL_RenderSetIntegerScale( ws.renderer, SDL_TRUE );
//	SDL_RenderSetLogicalSize( ws.renderer, 640, 360 );
	SDL_SetRenderLogicalPresentation( ws.renderer, 640, 360, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_INTEGER_SCALE );
}

void readEvents( WorldState & ws )
{
	SDL_PumpEvents();
	const bool * keystate = SDL_GetKeyboardState( nullptr );

	if( keystate[SDL_SCANCODE_Q] && keystate[SDL_SCANCODE_LCTRL] )
	{
		exit( 0 );
	}

	ws.moveToRight = keystate[SDL_SCANCODE_D];

	SDL_Event ev;
	while( SDL_PollEvent(&ev) )
	{
		if( ev.type == SDL_EVENT_MOUSE_MOTION )
		{
			ws.mouse_pos = {ev.motion.x/2, ev.motion.y/2};
		}
	}
}

void mutateWorldState( WorldState & ws )
{
	//ws.ball.x += ws.vel_ball.x;
	//ws.ball.y += ws.vel_ball.y;

	using namespace JanSordid::SDL;
	constexpr float dt = 0.01666f;
	ws.ball += ws.vel_ball * dt;

	if( ws.ball.x < 0 )
	{
		ws.ball.x = 0;
		ws.vel_ball.x *= -1;
	}
	else if( ws.ball.x > (640 - ws.ball.w) )
	{
		ws.ball.x = 620;
		ws.vel_ball.x *= -1;
	}

	if( ws.ball.y < 0 )
	{
		ws.ball.y = 0;
		ws.vel_ball.y = -ws.vel_ball.y;
	}
	else if( ws.ball.y > 340 )
	{
		ws.ball.y = 340;
		ws.vel_ball.y = -ws.vel_ball.y;
	}

	ws.ball.x += 0.1f * ws.moveToRight;
}

void renderGraphics( const WorldState & ws )
{
	SDL_SetRenderDrawColor( ws.renderer, 0, 0, 0, 255 );
	SDL_RenderClear( ws.renderer );

	// To make alpha working
	//SDL_SetRenderDrawBlendMode(ws.renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND );

//	SDL_Rect src { (((int)ball.x)/4/16)*16, 0, 640, 500 };
	SDL_FRect src = { 0, 0, 640, 360 };
	SDL_FRect dst = { 0, 0, 640, 360 };

	//SDL_RenderSetLogicalSize( ws.renderer, 640*2, 360*2 );
	// Do something hi-res
	//SDL_RenderSetLogicalSize( ws.renderer, 640, 360 );

	SDL_RenderTexture( ws.renderer, ws.texture, &src, &dst );

	const bool isIntersecting = SDL_HasRectIntersectionFloat( &ws.ball, &ws.wall );
	SDL_SetRenderDrawColor( ws.renderer, 255, 0, isIntersecting ? 255 : 0, 255 );
	isIntersecting
		? SDL_RenderRect( ws.renderer, &ws.wall )
		: SDL_RenderFillRect( ws.renderer, &ws.wall );

	SDL_SetRenderDrawColor( ws.renderer, 255, 0, 0, 127 );
	SDL_RenderFillRect( ws.renderer, &ws.ball ); // This still does display pixel scaling, not clamped to scaled up logical scale
	//auto bal = JanSordid::SDL::toI( ws.ball );
	//SDL_RenderFillRect( ws.renderer, &bal );

	SDL_RenderLine(ws.renderer, 0,0, ws.mouse_pos.x, ws.mouse_pos.y);

	SDL_SetRenderDrawColor( ws.renderer, 0, 255, 0, 127 );
	SDL_RenderLine(ws.renderer, 0,0, ws.mouse_pos.x, ws.mouse_pos.y);

	SDL_RenderPresent( ws.renderer );
}

int main( int argc, char * argv [] )
{
	print( "\n"
		"---=================================---\n"
		"   | Use CTRL + Q to quit the game |\n"
		"---=================================---\n"
		);

	WorldState worldState;

	initWorldState( worldState );

	while( true )
	{
		readEvents( worldState );

		mutateWorldState( worldState );

		renderGraphics( worldState );
	}
}

































/*
// This is still SDL2, wayne?
int mainx( int argc, char * argv [] )
{
	SDL_Init( SDL_INIT_EVERYTHING );

	SDL_Window   * window   = SDL_CreateWindow( "Game", 100, 200, 640, 480, SDL_WINDOW_OPENGL );
	SDL_Renderer * renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_PRESENTVSYNC );

	//std::shared_ptr<SDL_Window> window = std::shared_ptr<SDL_Window>(
	//	SDL_CreateWindow( "Game2", 100, 200, 640, 480, SDL_WINDOW_OPENGL ),
	//	&SDL_DestroyWindow
	//);
	//SDL_Renderer * renderer = SDL_CreateRenderer( window.get(), -1, SDL_RENDERER_PRESENTVSYNC );

	SDL_Rect ball  = { 200, 220, 20, 20 };
	SDL_Rect left  = {  10,  40, 10, 50 };
	SDL_Rect right = { 620, 380, 10, 50 };
	float v_x = 4.8f, v_y = 2.5f;
	float x = 200.0f, y = 160.0f;
	while( true )
	{
		// -=- Read Events -=-

		SDL_PumpEvents();
		const Uint8 * keystate = SDL_GetKeyboardState( nullptr );

		if( (keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL])
		 && (keystate[SDL_SCANCODE_Q]) )
		{
			if( renderer ) SDL_DestroyRenderer( renderer );
			if( window   ) SDL_DestroyWindow  ( window   );
			SDL_Quit();
			return 0; // QUIT
		}

		const int left_bar_move  = keystate[SDL_SCANCODE_S]    - keystate[SDL_SCANCODE_W];
		const int right_bar_move = keystate[SDL_SCANCODE_DOWN] - keystate[SDL_SCANCODE_UP];

		// Cheat zur ballgeschwindigkeits kontrolle
		v_x += (keystate[SDL_SCANCODE_L] - keystate[SDL_SCANCODE_J]) * 0.1f;
		v_y += (keystate[SDL_SCANCODE_K] - keystate[SDL_SCANCODE_I]) * 0.1f;

		// -=- Update World -=-

		left.y  += left_bar_move  * 5;
		right.y += right_bar_move * 5;

		// Gravity / Gravitation?
		//v_y += 0.3f;

		x += v_x;
		y += v_y;

		bool flash = false;
		if( x < 20 ) // prüfe linkes Aus, 20 = 10 abstand + 10 balkenbreite
		{
			int ldy = y - left.y; // relative y-pos des Balls zum Schläger
			if( -15 < ldy && ldy < 45 ) // Hit
			{
				x = 40 - x;

				// komplizierte Berechnung einfügen
				//ldy -= 15;
				//v_y = (v_y*0.8) + (ldy * 0.2);

				//if( v_y > 5 ) v_y = 5;
				//else if( v_y < -5 ) v_y = -5;
			}
			else // Miss
			{
				flash = true;
				x = 310;
				y = 220;
			}
			v_x *= -1;
		}
		else if( x > 600 ) // prüfe rechtes Aus, 600 = 640 bildbreite - 10 abstand - 10 balkenbreite - 20 ballbreite
		{
			const int rdy = y - right.y;
			if( -15 < rdy && rdy < 50 )
			{
				x = 1200 - x;
			}
			else
			{
				flash = true;
				x = 310;
				y = 220;
			}
			v_x *= -1;
		}

		if( y < 20 )
		{
			y = 40 - y;
			v_y *= -1;
		}
		else if( y > 440)
		{
			y = 880 - y;
			v_y *= -1;
		}

		ball.x = x;
		ball.y = y;

		/// -=- Rendering -=-

		if( flash )	SDL_SetRenderDrawColor( renderer, 127, 127, 127, 255 );
		else		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		// Clear the screen with the selected DrawColor
		SDL_RenderClear( renderer );

		// Walls
		SDL_SetRenderDrawColor( renderer, 127, 127, 127, 255 );
		const SDL_Rect walls [] = {
			{ 10,  10,  620, 10 },
			{ 10,  460, 620, 10 },
			{ 318, 30,  4,   420 },
		};
		SDL_RenderFillRects( renderer, walls, 3 );

		// Red/left paddle
		SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );
		SDL_RenderFillRect( renderer, &left );

		// Blue/right paddle
		SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
		SDL_RenderFillRect( renderer, &right );

		// Green Ball
		SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
		SDL_RenderFillRect( renderer, &ball );

		// Show everything
		SDL_RenderPresent( renderer );
	}
}
//*/
