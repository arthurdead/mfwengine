cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

if(UNIX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_NDEBUG -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_NDEBUG -DNDEBUG")
endif(UNIX)

set(project_name "ide")
string(TOUPPER ${project_name} project_name_upper)
project(${project_name})

get_filename_component(base_dir "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(source_dir "${base_dir}/source")
set(source_public_dir "${source_dir}/public")
set(source_private_dir "${source_dir}/private")
set(thirdparty_dir "${base_dir}/thirdparty")

set(include_dirs ${source_dir} ${thirdparty_dir})

list(APPEND include_dirs
	"${source_dir}/private/thirdparty/glad/generated/debug/include"
)

if(UNIX)
	list(APPEND include_dirs
		"${thirdparty_dir}/libsupc++"
	)
endif(UNIX)

file(GLOB_RECURSE ${project_name}_files
	"${source_dir}/*/mfw/${project_name}/*.cpp"
	"${source_dir}/*/mfw/${project_name}/*.hpp"
	"${source_dir}/*/mfw/${project_name}/*.inl"
)

file(GLOB_RECURSE stl_files
	"${source_dir}/*/mfw/stl/*.cpp"
	"${source_dir}/*/mfw/stl/*.hpp"
	"${source_dir}/*/mfw/stl/*.inl"
)

file(GLOB_RECURSE core_files
	"${source_dir}/*/mfw/core/*.cpp"
	"${source_dir}/*/mfw/core/*.hpp"
	"${source_dir}/*/mfw/core/*.inl"
)

file(GLOB_RECURSE renderer_files
	"${source_dir}/*/mfw/renderer/*.cpp"
	"${source_dir}/*/mfw/renderer/*.hpp"
	"${source_dir}/*/mfw/renderer/*.inl"
)

file(GLOB_RECURSE glad_files
	"${source_dir}/private/thirdparty/glad/generated/debug/src/*.c"
)

file(GLOB_RECURSE pugixml_files
	"${thirdparty_dir}/pugixml/src/*.cpp"
	"${thirdparty_dir}/pugixml/src/*.hpp"
)
list(APPEND include_dirs "${thirdparty_dir}/pugixml/src")

file(GLOB_RECURSE rapidjson_files "${thirdparty_dir}/rapidjson/include/*.h")
list(APPEND include_dirs "${thirdparty_dir}/rapidjson/include")

if(UNIX)
	file(REMOVE_RECURSE core_files
		"${source_dir}/*/mfw/core/**/windows/**"
	)

	file(REMOVE_RECURSE ${project_name}_files
		"${source_dir}/*/mfw/${project_name}/**/windows/**"
	)
endif(UNIX)

file(REMOVE_RECURSE renderer_files
	"${source_dir}/*/mfw/renderer/**/vulkan/**"
	"${source_dir}/*/mfw/renderer/**/xcb/**"
)

source_group("stl" FILES ${stl_files})
source_group("core" FILES ${core_files})
source_group("renderer" FILES ${renderer_files})
source_group("glad" FILES ${glad_files})
source_group("pugixml" FILES ${pugixml_files})
source_group("rapidjson" FILES ${rapidjson_files})
source_group("${project_name}" FILES ${${project_name}_files})

#list(APPEND files ${${project_name}_files})
list(APPEND files ${stl_files})
list(APPEND files ${core_files})
#list(APPEND files ${renderer_files})
#list(APPEND files ${glad_files})
#list(APPEND files ${pugixml_files})
#list(APPEND files ${rapidjson_files})

if(UNIX)
	add_compile_options(-Wfatal-errors -Wall -Wextra -Wdeprecated)
	#add_compile_options(-Wpedantic -pedantic -pedantic-errors)
	add_compile_options("$<$<CONFIG:DEBUG>:-pg>")
	add_link_options("$<$<CONFIG:DEBUG>:-pg>")
endif(UNIX)

if(WIN32)
	add_compile_definitions(
		#_MFC_OVERRIDES_NEW
		PUGIXML_WCHAR_MODE
		__PLACEMENT_NEW_INLINE
		__PLACEMENT_VEC_NEW_INLINE
	)
endif(WIN32)

add_compile_definitions(
	"MFW_BUILDING_${project_name_upper}"
	"MFW_DEPENDS_ON_${project_name_upper}"
	MFW_${project_name_upper}_BUILD=MFW_BUILD_EXECUTABLE_CONSOLE

	MFW_BUILDING_STL
	MFW_STL_BUILD=MFW_BUILD_EMBEDDED
	MFW_DEPENDS_ON_STL

	MFW_BUILDING_CORE
	MFW_CORE_BUILD=MFW_BUILD_EMBEDDED
	MFW_DEPENDS_ON_CORE

	MFW_BUILDING_RENDERER
	MFW_RENDERER_BUILD=MFW_BUILD_EMBEDDED
	MFW_DEPENDS_ON_RENDERER

	RAPIDJSON_ERROR_CHARTYPE=char8_t
	RAPIDJSON_HAS_STDSTRING=1

	UNICODE _UNICODE

	_CONSOLE CONSOLE
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${base_dir}/build_tools/mfwide")

add_executable(${project_name} ${files})
#target_precompile_headers(${project_name} PRIVATE "${source_dir}/public/mfw/pch.hpp")

target_include_directories(${project_name} PUBLIC ${include_dirs})

set_target_properties(${project_name}
	PROPERTIES
	DEBUG_POSTFIX "_debug"
	RELEASE_POSTFIX "_release"
	RELWITHDEBINFO_POSTFIX "_release_debug"
)

if(WIN32)
	target_link_libraries(${project_name} Dbghelp.lib Rpcrt4.lib)
endif(WIN32)

if(UNIX)
	target_link_libraries(${project_name} dl)
	target_link_libraries(${project_name} X11 "X11-xcb" Xrandr xcb "xcb-glx" "xcb-randr" shaderc_shared GL)
	#target_link_libraries(${project_name} vulkan)
endif(UNIX)
