#pragma once

#include <memory>

namespace JanSordid::Core
{
	template <typename T, typename TD = std::default_delete<T>>
	class AutocastUnique : public std::unique_ptr<T,TD>
	{
	public:
		using std::unique_ptr<T>::unique_ptr;

		template <typename Y>
		constexpr inline AutocastUnique( Y * && p ) noexcept : std::unique_ptr<T>( p, std::default_delete<Y>() ) {}

		constexpr inline operator       T * ()       noexcept { return this->get(); }
		constexpr inline operator const T * () const noexcept { return this->get(); }
	};

	template <typename T>
	class AutocastShared : public std::shared_ptr<T>
	{
	public:
		using std::shared_ptr<T>::shared_ptr;

		template <typename Y>
		constexpr inline AutocastShared( Y * && p )                noexcept : std::shared_ptr<T>( p, std::default_delete<Y>() ) {}
		constexpr inline AutocastShared( std::shared_ptr<T> && p ) noexcept : std::shared_ptr<T>( p ) {}

		constexpr inline operator       T * ()       noexcept { return this->get(); }
		constexpr inline operator const T * () const noexcept { return this->get(); }
	};

	template <typename T>
	class AutocastWeakShare : public std::weak_ptr<T>
	{
	public:
		using std::weak_ptr<T>::weak_ptr;

		AutocastShared<T> lock() const noexcept {
			return static_cast<AutocastShared<T>>(std::weak_ptr<T>::lock());
		}
	};
}
