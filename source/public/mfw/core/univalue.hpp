#ifndef MFW_PUBLIC_CORE_UNIVALUE_HPP
#define MFW_PUBLIC_CORE_UNIVALUE_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/limits.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/float.hpp>
#include <public/mfw/stl/functional.hpp>

#include <public/mfw/core/internal/univalue_view.hpp>
#include <public/mfw/core/internal/univalue.hpp>

namespace mfw::stl
{
	struct type_view<univalue> final
	{ using type = univalue_view; };
}

MFW_VISIBILITY_LOCAL_PUSH()

extern ::mfw::core::univalue operator""_uv(::mfw::stl::ullong_t n) noexcept;
extern ::mfw::core::univalue operator""_uv(::mfw::stl::ldouble_t f) noexcept;
extern ::mfw::core::univalue operator""_uv(const ::mfw::core::univalue::string_type::value_type *ptr, ::mfw::stl::size_t len) noexcept;

extern ::mfw::core::univalue_view operator""_uvv(::mfw::stl::ullong_t n) noexcept;
extern ::mfw::core::univalue_view operator""_uvv(::mfw::stl::ldouble_t f) noexcept;
extern ::mfw::core::univalue_view operator""_uvv(const ::mfw::core::univalue::string_view_type::value_type *ptr, ::mfw::stl::size_t len) noexcept;

MFW_VISIBILITY_LOCAL_POP()

#endif