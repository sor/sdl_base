#include "global.hpp"

void Callback( void * userdata, const char * const * filelist, int filter )
{
	fmt::print( "Hello {}! {}\n", "World", *filelist );
}

int main()
{
	using namespace JanSordid::SDL;

	SDL_Init( SDL_INIT_EVERYTHING );
	NFD::Guard g;

	//Shared<Surface> surf = IMG_Load( "" ); // will not load anything, but makes sure SDL_image is linked
	Owned<Window>   w = SDL_CreateWindow( "", 400, 400, 0 );
	Owned<Renderer> r = SDL_CreateRenderer( w, nullptr );

	SDL_ShowOpenFileDialog( Callback, nullptr, w, nullptr, 0, nullptr, false );

	auto lambdaCallback = []( void * userdata, const char * const * filelist, int filter )
	{
		fmt::print( "Hello {}! {}\n", "World", *filelist );
	};

	SDL_ShowOpenFileDialog( lambdaCallback, nullptr, w, nullptr, 0, nullptr, false );

	while( SDL_GetTicksNS() < 5'000'000'000 )
	{
		SDL_PumpEvents();
		Color clear = { (u8)rand(), (u8)rand(), (u8)rand(), (u8)rand() };
		SDL_SetRenderDrawColor( r, clear.r, clear.g, clear.b, clear.a );
		SDL_RenderClear( r );
		SDL_RenderPresent( r );
	}

	NFD::UniquePath upath;
	if( NFD::OpenDialog( upath, nullptr, 0, nullptr ) == NFD_OKAY )
	{
		fmt::print( "Hello {}! {}\n", "World", upath.get() );
	}

	return 0;
}
