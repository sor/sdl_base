#include "global.hpp"

int main()
{
	using namespace JanSordid::SDL;

	SDL_Init( SDL_INIT_EVERYTHING );
	NFD::Guard g;

	//Shared<Surface> surf = IMG_Load( "" ); // will not load anything, but makes sure SDL_image is linked
	Owned<Window>   w = SDL_CreateWindow( "", 0, 0, 400, 400, 0 );
	Owned<Renderer> r = SDL_CreateRenderer( w, -1, 0 );

	NFD::Window nw = NFD::GetNativeWindowFromSDLWindow( w );

	NFD::UniquePath upath;
	if( NFD::OpenDialog( upath, nullptr, 0, nullptr, nw ) == NFD_OKAY )
	{
		fmt::print( "Hello {}! {}\n", "World", upath.get() );
	}

	SDL_Delay( 1000 );
	return 0;
}
