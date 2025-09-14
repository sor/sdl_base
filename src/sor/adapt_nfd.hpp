#pragma once

// Native File Dialogues
#include <nfd.hpp>

// Monkey-patching NFD
namespace NFD
{
	constexpr const nfdfilteritem_t * EmptyFilter = nullptr;
	constexpr const nfdchar_t       * EmptyPath   = nullptr;
	constexpr const nfdchar_t       * EmptyName   = nullptr;

	using Result = nfdresult_t;
}
