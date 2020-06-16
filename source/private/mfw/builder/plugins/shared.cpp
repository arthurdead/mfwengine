#include <private/mfw/builder/plugins/shared.hpp>

namespace mfw::builder
{
	compiler_info_t get_compiler_info(const pstring &path)
	{
		if(path.empty()) {
			return {};
		}
		
		pstring filename{path.filename()};
		if(filename == u8"cl"_sv) {
			compiler_info_t info{};
			info.type = compiler_info_t::type_t::msvc;
			return info;
		}
		
		ucstring tmp{as_string<ucstring>(path)};
		return get_compiler_info(tmp);
	}
	
	linker_info_t get_linker_info(const pstring &path)
	{
		if(path.empty()) {
			return {};
		}
		
		ucstring tmp{as_string<ucstring>(path)};
		return get_linker_info(tmp);
	}
	
	compiler_info_t get_compiler_info(const ucstring &path)
	{
		compiler_info_t info{};
		
		if(!path.empty()) {
			if(path.find(u8"cl.exe"_sv) != ucstring::npos) {
				info.type = compiler_info_t::type_t::msvc;
			} else if(path.find(u8"gcc"_sv) != ucstring::npos) {
				if(path.find(u8"ar"_sv) == ucstring::npos) {
					info.type = compiler_info_t::type_t::gcc;
					info.flags |= compiler_info_t::flags_t::unix_;
					if(path.find(u8"musl"_sv) != ucstring::npos) {
						info.flags |= compiler_info_t::flags_t::musl;
					}
				}
			} else if(path.find(u8"clang"_sv) != ucstring::npos) {
				if(path.find(u8"-cl"_sv) != ucstring::npos) {
					info.type = compiler_info_t::type_t::msvc;
					//info.flags |= compiler_info_t::flags_t::unix_|compiler_info_t::flags_t::clang;
				} else if(path.find(u8"check"_sv) == ucstring::npos &&
					path.find(u8"ld"_sv) == ucstring::npos &&
					path.find(u8"doc"_sv) == ucstring::npos) {
					info.type = compiler_info_t::type_t::clang;
					info.flags |= compiler_info_t::flags_t::unix_|compiler_info_t::flags_t::clang;
					if(path.find(u8"musl"_sv) != ucstring::npos) {
						info.flags |= compiler_info_t::flags_t::musl;
					}
				}
			} else if(path.find(u8"g++"_sv) != ucstring::npos) {
				info.type = compiler_info_t::type_t::gcc;
				info.flags |= compiler_info_t::flags_t::unix_;
			} else if(path.find(u8"clang++"_sv) != ucstring::npos ||
						path.find(u8"zapcc++"_sv) != ucstring::npos ||
						path.find(u8"zapcc"_sv) != ucstring::npos ||
						path.find(u8"em++"_sv) != ucstring::npos ||
						path.find(u8"emcc"_sv) != ucstring::npos ||
						path.find(u8"include-what-you-use"_sv) != ucstring::npos) {
				info.type = compiler_info_t::type_t::clang;
				info.flags |= compiler_info_t::flags_t::unix_|compiler_info_t::flags_t::clang;
			}
			
			if(path.find(u8"ccache"_sv) != ucstring::npos) {
				info.flags |= compiler_info_t::flags_t::ccache;
			}
		}
		
		return info;
	}
	
	linker_info_t get_linker_info(const ucstring &path)
	{
		linker_info_t info{};
		
		if(!path.empty()) {
			if(path.find(u8"ld"_sv) != ucstring::npos) {
				if(path.find(u8"musl"_sv) != ucstring::npos) {
					info.flags |= linker_info_t::flags_t::musl;
				} else if(path.find(u8"gold"_sv) != ucstring::npos) {
					info.flags |= linker_info_t::flags_t::gold;
				} else if(path.find(u8"bfd"_sv) != ucstring::npos) {
					info.flags |= linker_info_t::flags_t::bfd;
				} else if(path.find(u8"lld"_sv) != ucstring::npos ||
							path.find(u8"wasm"_sv) != ucstring::npos) {
					info.flags |= linker_info_t::flags_t::lld;
				}
				info.flags |= linker_info_t::flags_t::unix_;
				info.type = linker_info_t::type_t::ld;
			} else if(path.find(u8"link"_sv) != ucstring::npos) {
				/*if(path.find(u8"lld"_sv) != ucstring::npos) {
					info.flags |= linker_info_t::flags_t::lld|linker_info_t::flags_t::unix_;
				}*/
				info.type = linker_info_t::type_t::link;
			}
		}
		
		return info;
	}
	
	const core::univalue *find_output_option(const core::serializable &options, const core::serializable &names)
	{
		for(const core::serializable &child : names) {
			const ucstring &name{child.get_name()};
			const core::serializable *option{options.get_child(name)};
			if(option) {
				return &option->get_value();
			}
		}
		return nullptr;
	}
	
	pstring get_output_path(const tool_reference &tool, const core::serializable &options)
	{
		pstring path{};

		const core::serializable *args{tool.output_args()};
		if(args) {
			const core::univalue *output{find_output_option(options, *args)};
			if(output) {
				const ucstring &str{output->get_string()};
				path = as_string<pstring>(str);
			}
		}
		
		if(path.empty()) {
			path = tool.output_default_path();
		}
		
		return path;
	}
}