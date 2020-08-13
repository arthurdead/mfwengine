#ifndef MFW_PUBLIC_CORE_SEARCHPATH_HPP
#define MFW_PUBLIC_CORE_SEARCHPATH_HPP

#pragma once

#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/type_traits.hpp>

#include <public/mfw/core/internal/searchpath_view.hpp>
#include <public/mfw/core/internal/searchpath.hpp>

namespace mfw::stl
{
	struct type_view<searchpath> final
	{ using type = searchpath_view; };
}

MFW_VISIBILITY_LOCAL_PUSH()

extern ::mfw::core::searchpath operator""_sp(const ::mfw::stl::pchar_t *ptr, ::mfw::stl::size_t len) noexcept;
extern ::mfw::core::searchpath_view operator""_spv(const ::mfw::stl::pchar_t *ptr, ::mfw::stl::size_t len) noexcept;

MFW_VISIBILITY_LOCAL_POP()

#endif