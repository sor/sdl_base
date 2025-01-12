#pragma once

//#define NFD_NATIVE 1

// Activate this line to be able to put the executable in the root folder
//#define DEPLOY

#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cassert>

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

#if defined( __cpp_lib_format ) && defined( __cpp_lib_print )
	#include <format>
	#include <print>
#else
	#include <fmt/core.h>   // https://fmt.dev/latest/index.html
#endif

#include <nfd.hpp>

#ifdef DEPLOY_BINARY
	#define BasePath ""
#else
	#define BasePath "../../../"
#endif

#if defined( _DEBUG )
	#define Assert(            ... ) assert( __VA_ARGS__ )
	#define AssertInOptimized( ... ) assert( __VA_ARGS__ )
	#define DebugOnly(         ... ) __VA_ARGS__
	#define OptimizedOnly(     ... )
	#define FinalOnly(         ... )
	#define IfDebug     if constexpr( true  )
	#define IfOptimized if constexpr( false )
	#define IfFinal     if constexpr( false )
	#define IfNotFinal  if constexpr( true  )
#elif defined( OPTIMIZED )
	#define Assert(            ... )
	#define AssertInOptimized( ... ) assert( __VA_ARGS__ )
	#define DebugOnly(         ... )
	#define OptimizedOnly(     ... ) __VA_ARGS__
	#define FinalOnly(         ... )
	#define IfDebug     if constexpr( false )
	#define IfOptimized if constexpr( true  )
	#define IfFinal     if constexpr( false )
	#define IfNotFinal  if constexpr( true  )
#elif defined( FINAL )
	#define Assert(            ... )
	#define AssertInOptimized( ... )
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


/// Polyfills for disabled functionality

#if ! __cpp_rtti
	// If -fno-rtti, try replacing dynamic_cast with static_cast
	// If this works, then the original code should probably be changed
	#define dynamic_cast static_cast
#endif

#if ! __cpp_exceptions
	// If -fno-exceptions, transform error handling code to work without it.
	#define try      if constexpr( true )
	#define catch(X) if constexpr( false )
	#define throw    /* HACK: Come up with a solution for throw */
#endif


// Monkey-patching NFD
namespace NFD
{
	constexpr const nfdfilteritem_t * EmptyFilter      = nullptr;
	constexpr const nfdchar_t       * EmptyDefaultPath = nullptr;
	constexpr const nfdchar_t       * EmptyDefaultName = nullptr;

	using Result = nfdresult_t;
}

namespace JanSordid::Core
{
	/// Aliases of a lot of std:: for easy usage

	// Usage of `int` and `uint` conveys:
	//  I don't mind about the size, don't use for preserved data (structs, classes, globals)
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

	using isize = std::ptrdiff_t;
	using usize = std::size_t;

	using byte  = std::byte;

	using f32   = float;
	using f64   = double;

	// Classes / Structs
	using String        = std::string;
	using StringView    = std::string_view;

	using Clock         = std::chrono::high_resolution_clock;
	using TimePoint     = Clock::time_point;
	using Duration      = Clock::duration;

	using File          = std::FILE;

	// Functions
	using std::min;
	using std::max;
	using std::forward;
	using std::move;
	using std::make_unique;
	using std::make_shared;

	// Templates
	template<typename T, std::size_t Size>                       using Array     = std::array<T, Size>;
	template<typename T>                                         using DynArray  = std::vector<T>;
	template<typename T>                                         using Vector    = std::vector<T>;
	template<typename T>                                         using HashSet   = std::unordered_set<T>;
	template<typename TKey, typename TValue>                     using HashMap   = std::unordered_map<TKey,TValue>;

	template<typename T>                                         using RawPtr    = T*;

	template<typename T, typename TDel = std::default_delete<T>> using UniquePtr = std::unique_ptr<T,TDel>;
	template<typename T>                                         using SharedPtr = std::shared_ptr<T>;
	template<typename T>                                         using WeakPtr   = std::weak_ptr<T>;

	// String formatting
#if defined( __cpp_lib_format ) && defined( __cpp_lib_print )
	using std::print, std::println, std::format, std::format_string;
#else
	using fmt::print, fmt::println, fmt::format, fmt::format_string;
#endif

	namespace ChronoLiterals { using namespace std::chrono_literals; }
	namespace Numbers        { using namespace std::numbers;         }

	template <typename... T>
	inline void print_once( format_string<T...> fmt, T && ... args )
	{
		static std::unordered_set<String> all;
		const String msg = format( fmt, forward<T>( args )... );
		if( all.find( msg ) == all.end() )
		{
			all.insert( msg );
			print( "{}", msg );
		}
	}
}

#include <nfd.hpp>
