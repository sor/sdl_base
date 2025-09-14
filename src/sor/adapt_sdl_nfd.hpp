#pragma once

#include "SDL_video.h"

// Native File Dialogues
#include "adapt_nfd.hpp"

// Monkey-patching NFD
namespace NFD
{
	// Usually passed by value (contrary to SDL_Window)
	using Window = nfdwindowhandle_t;

	// SDL_Window as pointer because it's the case for all other functions as well, else by reference would be preferred
	Window GetNativeWindowFromSDLWindow( SDL_Window * sdlWindow );
}
