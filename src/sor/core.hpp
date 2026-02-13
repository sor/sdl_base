#pragma once

#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cassert>

#include <version>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <memory>
#include <numbers>
#include <string>

#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "adapt_std.hpp"


#ifdef DEPLOY_BINARY
	#define BasePath ""
#else
	#define BasePath "../../../"
#endif

/*
 * Ask how could the macro be extended to add the msg if none given
 * by stringifying the cond, or having the ability to receive a handwritten msg
template<typename T>
constexpr void MyAssert( T condition, const char * msg) {
    if (std::is_constant_evaluated()) {
        if( !condition )
            throw msg;
    } else {
        assert( condition && msg );
    }
}
*/

#define assertCE(expr)                                                         \
	(std::is_constant_evaluated() && !static_cast<bool>(expr)                  \
		? throw std::logic_error( "Assertion failed in constant expression!" ) \
		: assert(expr))

#if defined( _DEBUG )
	#define Assert(            ... ) assertCE( __VA_ARGS__ )
	#define AssertInOptimized( ... ) assertCE( __VA_ARGS__ )
	#define DebugOnly(         ... ) __VA_ARGS__
	#define OptimizedOnly(     ... )
	#define FinalOnly(         ... )
	#define IfDebug     if constexpr( true  )
	#define IfOptimized if constexpr( false )
	#define IfFinal     if constexpr( false )
	#define IfNotFinal  if constexpr( true  )
#elif defined( OPTIMIZED )
	#define Assert(            ... ) do{ (void)sizeof((__VA_ARGS__)); } while( 0 )
	#define AssertInOptimized( ... ) assertCE( __VA_ARGS__ )
	#define DebugOnly(         ... )
	#define OptimizedOnly(     ... ) __VA_ARGS__
	#define FinalOnly(         ... )
	#define IfDebug     if constexpr( false )
	#define IfOptimized if constexpr( true  )
	#define IfFinal     if constexpr( false )
	#define IfNotFinal  if constexpr( true  )
#elif defined( FINAL )
	#define Assert(            ... ) do{ (void)sizeof((__VA_ARGS__)); } while( 0 )
	#define AssertInOptimized( ... ) do{ (void)sizeof((__VA_ARGS__)); } while( 0 )
	#define DebugOnly(         ... )
	// Final also counts as an Optimized build, therefore the OptimizedOnly define is active
	#define OptimizedOnly(     ... ) __VA_ARGS__
	#define FinalOnly(         ... ) __VA_ARGS__
	#define IfDebug     if constexpr( false )
	#define IfOptimized if constexpr( true  )
	#define IfFinal     if constexpr( true  )
	#define IfNotFinal  if constexpr( false )
#else
	#error Unknown build configuration
#endif


namespace JanSordid::Core//::inline V1
{
	/// Aliases of a lot of std:: for easy usage

	namespace Chrono         { using namespace std::chrono;          }
	namespace ChronoLiterals { using namespace std::chrono_literals; }
	namespace Numbers        { using namespace std::numbers;         }

	// Usage of `int` and `uint` conveys:
	//  I don't mind about the size, avoid for preserved data (structs, classes, globals)
	using uint  = unsigned int;

	// Types with defined size
	using i8    = std::int8_t;
	using i16   = std::int16_t;
	using i32   = std::int32_t;
	using i64   = std::int64_t;
	using u8    = std::uint8_t;
	using u16   = std::uint16_t;
	using u32   = std::uint32_t;
	using u64   = std::uint64_t;

//	using iptr  = std::intptr_t; // Kind of the same as below
//	using uptr  = std::uintptr_t;

	using idiff = std::ptrdiff_t; // or isize?
	using usize = std::size_t;

	using byte  = std::byte;

	using f32   = float;
	using f64   = double;

	// E.g. for use as type in user defined literals
	using imax  = std::intmax_t;
	using umax  = std::uintmax_t;
	using fmax  = long double; // This is not consistent, can be: f64, f80, f96, or f128

	// TODO: Do all the other types as well?
	constexpr f32  operator""_f32 (unsigned long long n) { return (f32)n; }
	constexpr f64  operator""_f64 (unsigned long long n) { return (f64)n; }
	constexpr f32  operator""_f32 (long double n) { return (f32)n; }
	constexpr f64  operator""_f64 (long double n) { return (f64)n; }

	// Classes / Structs
	using String        = std::string;
	using StringView    = std::string_view;

	using NanoSec       = Chrono::nanoseconds;                     // Nanoseconds
	using MilliSec      = Chrono::milliseconds;                    // Milliseconds
	using Sec           = Chrono::seconds;                         // Nanoseconds
	using FNanoSec      = Chrono::duration<f32, NanoSec::period>;  // Nanoseconds as float
	using FMilliSec     = Chrono::duration<f32, MilliSec::period>; // Milliseconds as float
	using FSec          = Chrono::duration<f32, Sec::period>;      // Seconds as float
	using Clock         = Chrono::high_resolution_clock;
	using TimePoint     = Clock::time_point;
	using Duration      = Clock::duration; // Should be nanoseconds, but maybe don't rely on it
	static_assert( std::is_same_v<Duration, NanoSec> ); // If this breaks, revisit a lot of Update and Render calls

	using File          = std::FILE;

	// Math Functions
	using std::min, std::max;
	using std::abs, std::sin, std::cos;
	using std::floor, std::ceil, std::round, std::lround, std::llround;

	// Chrono Functions
	using Chrono::duration_cast;
	using ChronoLiterals::operator ""ns,
	      ChronoLiterals::operator ""ms,
	      ChronoLiterals::operator ""s;

	// Functions
	using std::move, std::forward;
	using std::make_unique, std::make_shared;
	using std::to_underlying;

	// Templates
	template<typename T, usize Size>            using Array     = std::array<T, Size>;
	template<typename T>                        using DynArray  = std::vector<T>;
//	template<typename T>                        using Vector    = std::vector<T>;
	template<typename T>                        using HashSet   = std::unordered_set<T>;
	template<typename TKey, typename TValue>    using HashMap   = std::unordered_map<TKey,TValue>;

	template<typename T>                                           using RawPtr    = T*;
	template<typename T, typename TDel = std::default_delete<T>>   using UniquePtr = std::unique_ptr<T,TDel>;
	template<typename T>                                           using SharedPtr = std::shared_ptr<T>;
	template<typename T>                                           using WeakPtr   = std::weak_ptr<T>;

	template <typename... T>
	void print_once( format_string<T...> fmt, T && ... args )
	{
		static HashSet<String> all;
		const String msg = format( fmt, forward<T>( args )... );
		if( all.find( msg ) == all.end() )
		{
			all.insert( msg );
			print( "{}", msg );
		}
	}
}
