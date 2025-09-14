#include "sdl_core.hpp"

static_assert( std::is_same_v<JanSordid::Core::i8,   Sint8>  );
static_assert( std::is_same_v<JanSordid::Core::i16,  Sint16> );
static_assert( std::is_same_v<JanSordid::Core::i32,  Sint32> );
static_assert( std::is_same_v<JanSordid::Core::i64,  Sint64> );
static_assert( std::is_same_v<JanSordid::Core::u8,   Uint8>  );
static_assert( std::is_same_v<JanSordid::Core::u16,  Uint16> );
static_assert( std::is_same_v<JanSordid::Core::u32,  Uint32> );
static_assert( std::is_same_v<JanSordid::Core::u64,  Uint64> );
