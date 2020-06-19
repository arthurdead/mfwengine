#include <public/mfw/core/debugging.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/core/application.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/pch_literals.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/typeinfo.hpp>

#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
	#include <DbgHelp.h>
	#ifdef _IMAGEHLP64
		#define IMAGEHLP_LINEW IMAGEHLP_LINEW64
	#endif
	#if MFW_COMPILER == MFW_COMPILER_CLANG
	struct _ThrowInfo;
	#endif
	#include <rttidata.h>
#elif MFW_LIBCPP_FLAGGED(UNIX)
	#ifdef __MFW_USE_IBERTY
		#include <libiberty/demangle.h>
	#endif
	#include <cxxabi.h>
	#if MFW_LIBCPP_IS(LLVM)
		#include <llvm-project/libcxxabi/src/private_typeinfo.h>
	#else
		#if MFW_COMPILER == MFW_COMPILER_CLANG
			MFW_WARNING_PUSH()
			MFW_WARNING_DISABLE("-Wnull-pointer-arithmetic")
		#endif
		#include <gcc/libsupc++/tinfo.h>
		#if MFW_COMPILER == MFW_COMPILER_CLANG
			MFW_WARNING_POP()
		#endif
	#endif
#endif

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_debugging, u8"core/debugging"_p)

	namespace __debugging_internal
	{
		class symbol_helper final : interfaces::global_initializer
		{
		public:
			static symbol_helper &instance();

		private:
			exit_status initialize() override
			{
			#if MFW_OS == MFW_OS_WINDOWS
				process = GetCurrentProcess();
				thread = GetCurrentThread();
				SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES
				#if MFW_PROCESSOR & MFW_PROCESSOR_32BITS_FLAG
					| SYMOPT_INCLUDE_32BIT_MODULES
				#endif
				);
				SymInitializeW(process, nullptr, true);

				/*
				const ucstring &_NT_SYMBOL_PATH{GetEnvironmentVariable(L"_NT_SYMBOL_PATH"s)};
				const ucstring &_NT_ALTERNATE_SYMBOL_PATH{GetEnvironmentVariable(L"_NT_ALTERNATE_SYMBOL_PATH"s)};
				const ucstring &SYSTEMROOT{GetEnvironmentVariable(L"SYSTEMROOT"s)};

				AddSymDir({L""p, L"EXECUTABLE_PATH"s});
				AddSymDir({_NT_SYMBOL_PATH});
				AddSymDir({_NT_ALTERNATE_SYMBOL_PATH});
				AddSymDir({SYSTEMROOT});
				AddSymDir({SYSTEMROOT+L"\\System32"s});
				*/
				//AddSymDir({home});

				//SymSetHomeDirectoryW(_process, home.c_str());

				symbuffer.assign(sizeof(SYMBOL_INFOW) + (MAX_SYM_NAME * sizeof(wchar_t)), 0);

				syminfo = reinterpret_cast<SYMBOL_INFOW *>(symbuffer.data());
				syminfo->SizeOfStruct = sizeof(SYMBOL_INFOW);
				syminfo->MaxNameLen = MAX_SYM_NAME;

				return {};
			#else
				MFW_MESSAGE("TODO!!!")
				return {};
			#endif
			}

			exit_status update() override { return {}; }

			exit_status shutdown() override
			{
			#if MFW_OS == MFW_OS_WINDOWS
				//SymRefreshModuleList(_process);
				SymCleanup(process);
			#endif
				return {};
			}

		public:
		#if MFW_OS == MFW_OS_WINDOWS
			HANDLE process{nullptr};
			HANDLE thread{nullptr};
			vector<byte> symbuffer{};
			SYMBOL_INFOW *syminfo{nullptr};
		#endif
		};

		MFW_DECLARE_GLOBAL_ALLOCATOR(symbolhelper, symbol_helper)

		symbol_helper &symbol_helper::instance() {
			return __symbolhelper_global_allocator.instance();
		}

		static uint32_t undflags_translate(undecorate_flags flags)
		{
			uint32_t ret{0};

		#if MFW_OS == MFW_OS_WINDOWS
			if(bool_cast(flags & undecorate_flags::complete)) { ret |= UNDNAME_COMPLETE; }
			if(bool_cast(flags & undecorate_flags::no_leading_under_scores)) { ret |= UNDNAME_NO_LEADING_UNDERSCORES; }
			if(bool_cast(flags & undecorate_flags::no_ms_keywords)) { ret |= UNDNAME_NO_MS_KEYWORDS; }
			if(bool_cast(flags & undecorate_flags::no_func_return)) { ret |= UNDNAME_NO_FUNCTION_RETURNS; }
			if(bool_cast(flags & undecorate_flags::no_allocation_model)) { ret |= UNDNAME_NO_ALLOCATION_MODEL; }
			if(bool_cast(flags & undecorate_flags::no_allocation_languague)) { ret |= UNDNAME_NO_ALLOCATION_LANGUAGE; }
			if(bool_cast(flags & undecorate_flags::no_ms_this_type)) { ret |= UNDNAME_NO_MS_THISTYPE; }
			if(bool_cast(flags & undecorate_flags::no_cv_this_type)) { ret |= UNDNAME_NO_CV_THISTYPE; }
			if(bool_cast(flags & undecorate_flags::no_this_type)) { ret |= UNDNAME_NO_THISTYPE; }
			if(bool_cast(flags & undecorate_flags::no_access_specifiers)) { ret |= UNDNAME_NO_ACCESS_SPECIFIERS; }
			if(bool_cast(flags & undecorate_flags::no_throw_signatures)) { ret |= UNDNAME_NO_THROW_SIGNATURES; }
			if(bool_cast(flags & undecorate_flags::no_memberType)) { ret |= UNDNAME_NO_MEMBER_TYPE; }
			if(bool_cast(flags & undecorate_flags::no_udt_return_model)) { ret |= UNDNAME_NO_RETURN_UDT_MODEL; }
			if(bool_cast(flags & undecorate_flags::undecorate_32bit)) { ret |= UNDNAME_32_BIT_DECODE; }
			if(bool_cast(flags & undecorate_flags::name_only)) { ret |= UNDNAME_NAME_ONLY; }
			if(bool_cast(flags & undecorate_flags::no_arguments)) { ret |= UNDNAME_NO_ARGUMENTS; }
			if(bool_cast(flags & undecorate_flags::no_special_syms)) { ret |= UNDNAME_NO_SPECIAL_SYMS; }
		#elif MFW_OS == MFW_OS_LINUX
			#ifdef __MFW_USE_IBERTY
			if(bool_cast(flags & undecorate_flags::name_only)) { ret |= DMGL_ANSI|DMGL_TYPES; }
			if(bool_cast(flags & undecorate_flags::complete)) { ret |= DMGL_PARAMS|DMGL_ANSI|DMGL_TYPES; }
			if(bool_cast(flags & undecorate_flags::no_arguments)) { ret &= ~DMGL_PARAMS; }
			if(bool_cast(flags & undecorate_flags::no_func_return)) { ret |= DMGL_RET_DROP; }
			#endif
		#else
			#error
		#endif

			return ret;
		}

	#if MFW_OS == MFW_OS_WINDOWS
		#if _RTTI_RELATIVE_TYPEINFO
			#define __MFW_RTTI_RELATIVE
		#endif
		using raw_type_info = ::TypeDescriptor;
		using base_class_info = ::_RTTIBaseClassDescriptor;
		using base_class_array = ::_RTTIBaseClassArray;
	#elif MFW_OS == MFW_OS_LINUX
		using raw_type_info = ::std::type_info;
		using base_class_info = ::abi::__base_class_type_info;
		using base_class_array = base_class_info[];
		#if MFW_LIBCPP_IS(GNU)
		using ::abi::adjust_pointer;
		using ::abi::vtable_prefix;
		#endif
		using ::abi::__cxa_demangle;
	#else
		#error
	#endif

	#if MFW_OS == MFW_OS_WINDOWS
		static const _RTTICompleteObjectLocator *get_complete_object_locator_from_object(const void *pointerToObject)
		{
			const _RTTICompleteObjectLocator *locator{reinterpret_cast<const _RTTICompleteObjectLocator **const *>(pointerToObject)[0][-1]};
			if(reinterpret_cast<uintptr_t>(locator) == 
			#if MFW_PROCESSOR_FLAGGED(64BITS)
				0xEDEDEDEDEDEDEDED
			#else
				0xEDEDEDED
			#endif
			) {
				return nullptr;
			}
			return locator;
		}

		#ifdef __MFW_RTTI_RELATIVE
		static uintptr_t get_image_base(const void *pCallerPC)
		{
			void *_ImageBase{RtlPcToFileHeader(const_cast<void *>(pCallerPC), &_ImageBase)};
			return reinterpret_cast<uintptr_t>(_ImageBase);
		}

		static uintptr_t get_image_base_from_complete_object_locator(const _RTTICompleteObjectLocator *pCompleteLocator)
		{
			if(pCompleteLocator->signature == COL_SIG_REV0) {
				return get_image_base(pCompleteLocator);
			}
			return (reinterpret_cast<uintptr_t>(pCompleteLocator) - (static_cast<uintptr_t>(pCompleteLocator->pSelf)));
		}
		#endif
	#elif MFW_OS == MFW_OS_LINUX
		static const ::abi::__class_type_info *get_class_info(const void *pointerToObject)
		{
			const void *vtable{*static_cast<const void *const *>(pointerToObject)};
			if(reinterpret_cast<uintptr_t>(vtable) == 0x5) {
				return nullptr;
			}

			#if MFW_LIBCPP_IS(GNU)
			const vtable_prefix *prefix{adjust_pointer<vtable_prefix>(vtable, -offsetof(vtable_prefix, origin))};
			const void *whole_ptr{adjust_pointer<void>(pointerToObject, prefix->whole_object)};

			const ::abi::__class_type_info *whole_type{prefix->whole_type};

			const void *whole_vtable{*static_cast<const void *const *>(whole_ptr)};
			const vtable_prefix *whole_prefix{adjust_pointer<vtable_prefix>(whole_vtable, -offsetof(vtable_prefix, origin))};
			if(whole_prefix->whole_type != whole_type) {
				return nullptr;
			}
			#else
			const ::abi::__class_type_info *whole_type{nullptr};
			__MFW_MESSAGE("TODO!!!")
			#endif

			return whole_type;
		}
	#endif

		static ucstring get_type_name(const raw_type_info *type)
		{
			ucstring name{reinterpret_cast<const char8_t *>(type->name
		#if MFW_OS == MFW_OS_LINUX
				()
		#endif
			)};
			ucstring str{};
			undecorate(name, str, undecorate_flags::name_only);
			return str;
		}
	}

	MFW_CORE_API bool MFW_CORE_CALL undecorate(const ucstring_view &decorated, ucstring &undecorated, undecorate_flags flags)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		undecorated.resize(MAX_SYM_NAME, u8'\0');

		uint32_t len{UnDecorateSymbolNameW(c_str(decorated), c_str(undecorated), static_cast<uint32_t>(undecorated.capacity()), __debugging_internal::undflags_translate(flags))};
		if(len == 0) {
			return false;
		}

		undecorated.resize(len, u8'\0');

		ucstring_view find{u8".?AV"_sv};
		size_t pos{undecorated.find(find.data(), 0, find.length())};
		if(pos != ucstring::npos) {
			undecorated.erase(pos, 4);
		}

		find = u8"@@"_sv;
		pos = undecorated.find(find.data(), 0, find.length());
		if(pos != ucstring::npos) {
			undecorated.erase(pos, 2);
		}

		return true;
	#elif MFW_OS == MFW_OS_LINUX
		#ifdef __MFW_USE_IBERTY
		cplus_demangle_set_style(gnu_v3_demangling);
		char *ret{cplus_demangle_v3(c_str(decorated), DMGL_GNU_V3|__debugging_internal::undflags_translate(flags))};
		if(!ret) {
			return false;
		}
		#else
		char *ret{__debugging_internal::__cxa_demangle(c_str(decorated), nullptr, 0, nullptr)};
		if(!ret) {
			return false;
		}
		#endif

		undecorated = uc_str(ret);

		free(ret);
		
		return true;
	#else
		#error
	#endif
	}

	MFW_CORE_API bool MFW_CORE_CALL symbols_from_address(const void *, symbol &)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		__symbol_helper &helper{__symbol_helper::instance()};

		if(!SymFromAddrW(helper.process, reinterpret_cast<uintptr_t>(ptr), nullptr, helper.syminfo)) {
			return false;
		}

		IMAGEHLP_LINEW line{};
		line.SizeOfStruct = sizeof(IMAGEHLP_LINEW);
		uint32_t displ{0};
		if(!SymGetLineFromAddrW(helper.process, reinterpret_cast<uintptr_t>(ptr), reinterpret_cast<unsigned long *>(&displ), &line)) {
			return false;
		}

		IMAGEHLP_MODULEW modul{};
		modul.SizeOfStruct = sizeof(IMAGEHLP_MODULEW);
		if(!SymGetModuleInfoW(helper.process, reinterpret_cast<uintptr_t>(ptr), &modul)) {
			return false;
		}

		sym.name = reinterpret_cast<const char16_t *>(helper.syminfo->Name);
		sym.line = static_cast<int32_t>(line.LineNumber);
		sym.filename = reinterpret_cast<const char16_t *>(line.FileName);
		//sym.dll = reinterpret_cast<HMODULE>(modul.BaseOfImage);
		sym.library = reinterpret_cast<const char16_t *>(modul.ModuleName);

		return true;
	#else
		MFW_DEBUGBREAK();
		MFW_MESSAGE("TODO!!!")
		return false;
	#endif
	}

	MFW_CORE_API bool MFW_CORE_CALL walk_stack(vector<symbol> &symbols, uint32_t)
	{
		symbols.clear();

	#if MFW_OS == MFW_OS_WINDOWS
		STACKFRAME frame{};
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrReturn.Mode = AddrModeFlat;
		frame.AddrBStore.Mode = AddrModeFlat;

		CONTEXT ctx{};
		ctx.ContextFlags = CONTEXT_FULL;
		RtlCaptureContext(&ctx);

		#if MFW_PROCESSOR == MFW_PROCESSOR_X86
		uint32_t machine{IMAGE_FILE_MACHINE_I386};
		frame.AddrPC.Offset = ctx.Eip;
		frame.AddrStack.Offset = ctx.Esp;
		frame.AddrFrame.Offset = ctx.Ebp;
		#elif MFW_PROCESSOR == MFW_PROCESSOR_X86_64
		uint32_t machine{IMAGE_FILE_MACHINE_AMD64};
		frame.AddrPC.Offset = ctx.Rip;
		frame.AddrStack.Offset = ctx.Rsp;
		frame.AddrFrame.Offset = ctx.Rdi;
		#endif

		__symbol_helper &helper{__symbol_helper::instance()};

		count++;

		uint32_t i{0};
		while(StackWalk(machine, helper.process, helper.thread, &frame, &ctx, nullptr, SymFunctionTableAccess, SymGetModuleBase, nullptr)) {
			if(i == 0) {
				i++;
				continue;
			}

			if(count > 1 && i > count) {
				break;
			}

			symbol sym{};
			if(!symbols_from_address(reinterpret_cast<const void *>(frame.AddrPC.Offset), sym)) {
				continue;
			}

			symbols.push_back(move(sym));
			i++;
		}
	#else
		MFW_DEBUGBREAK();
		MFW_MESSAGE("TODO!!!")
	#endif

		return !symbols.empty();
	}

	MFW_CORE_API void MFW_CORE_CALL print_stack(uint32_t count)
	{
		if(count > 0) {
			count++;
		}

		vector<symbol> symbols{};
		walk_stack(symbols, count);
		symbols.erase(symbols.begin());

		for(const symbol &it : symbols) {
			log_debugging().warning(u8"{}({}): {}"_sv, it.filename, it.line, it.name);
		}

		log_debugging().warning(u8"\n"_sv);
	}

	MFW_CORE_API bool MFW_CORE_CALL rtti::is_derived_of(const ucstring_view &name) const
	{
		for(const rtti &it : baselist) {
			if(it.name() == name)
				return true;
		}
		return false;
	}

	void rtti::initialize_internal(const void *ptr)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		const __debugging_internal::raw_type_info *typedesc{reinterpret_cast<const __debugging_internal::raw_type_info *>(ptr)};

		typeinfo = reinterpret_cast<const ::std::type_info *>(typedesc);
		name_ = __debugging_internal::get_type_name(typedesc);
	#elif MFW_OS == MFW_OS_LINUX
		const ::abi::__class_type_info *class_info{reinterpret_cast<const ::abi::__class_type_info *>(ptr)};

		typeinfo = class_info;
		name_ = __debugging_internal::get_type_name(class_info);

		const ::abi::__si_class_type_info *single{reinterpret_cast<const ::abi::__si_class_type_info *>(class_info)};
		const ::abi::__vmi_class_type_info *multiple{reinterpret_cast<const ::abi::__vmi_class_type_info *>(class_info)};
		if(multiple->__flags == 4159700632 || multiple->__base_count == 32767) {
			return;
		} else if(reinterpret_cast<uintptr_t>(single->__base_type) != 
	#if MFW_PROCESSOR_FLAGGED(64BITS)
			0x200000000
	#else
			0x20000
	#endif
		) {
			rtti &inf{baselist.emplace_front()};
			inf.initialize_internal(single->__base_type);
		} else if(multiple->__flags != 1432999672 && multiple->__base_count != 21845) {
			for(size_t i{0}; i < multiple->__base_count; i++) {
				const __debugging_internal::base_class_info *base{&multiple->__base_info[i]};
				rtti &inf{baselist.emplace_front()};
				inf.initialize_internal(base->__base_type);
			}
		}
	#else
		#error
	#endif
	}

	MFW_CORE_API bool MFW_CORE_CALL rtti::initialize(const void *ptr)
	{
		clear();

	#if MFW_OS == MFW_OS_WINDOWS
		const _RTTICompleteObjectLocator *locator{__debugging_internal::get_complete_object_locator_from_object(ptr)};
		if(!locator) {
			return false;
		}

		#ifdef __MFW_RTTI_RELATIVE
		uintptr_t baseaddr{__debugging_internal::get_image_base_from_complete_object_locator(locator)};
		const __debugging_internal::raw_type_info *typedesc{reinterpret_cast<const __debugging_internal::raw_type_info *>(baseaddr + static_cast<uintptr_t>(locator->pTypeDescriptor))};
		const _RTTIClassHierarchyDescriptor *classhier{reinterpret_cast<const _RTTIClassHierarchyDescriptor *>(baseaddr + static_cast<uintptr_t>(locator->pClassDescriptor))};
		const __debugging_internal::base_class_array *basearr{reinterpret_cast<const __debugging_internal::base_class_array *>(baseaddr + static_cast<uintptr_t>(classhier->pBaseClassArray))};
		#else
		const __debugging_internal::raw_type_info *typedesc{reinterpret_cast<const __debugging_internal::raw_type_info *>(locator->pTypeDescriptor)};
		const _RTTIClassHierarchyDescriptor *classhier{reinterpret_cast<const _RTTIClassHierarchyDescriptor *>(locator->pClassDescriptor)};
		const __debugging_internal::base_class_array *basearr{reinterpret_cast<const __debugging_internal::base_class_array *>(classhier->pBaseClassArray)};
		#endif

		initialize_internal(typedesc);

		for(size_t i{0}; i < classhier->numBaseClasses; i++) {
		#ifdef __MFW_RTTI_RELATIVE
			const __debugging_internal::base_class_info *classdesc{reinterpret_cast<const __debugging_internal::base_class_info *>(baseaddr + static_cast<uintptr_t>(basearr->arrayOfBaseClassDescriptors[i]))};
			const __debugging_internal::raw_type_info *classtype{reinterpret_cast<const __debugging_internal::raw_type_info *>(baseaddr + static_cast<uintptr_t>(classdesc->pTypeDescriptor))};
		#else
			const __debugging_internal::base_class_info *classdesc{reinterpret_cast<const __debugging_internal::base_class_info *>(basearr->arrayOfBaseClassDescriptors[i])};
			const __debugging_internal::raw_type_info *classtype{reinterpret_cast<const __debugging_internal::raw_type_info *>(classdesc->pTypeDescriptor)};
		#endif
			rtti &rtti{baselist.emplace_front()};
			rtti.initialize_internal(classtype);
		}
	#elif MFW_OS == MFW_OS_LINUX
		const ::abi::__class_type_info *class_info{__debugging_internal::get_class_info(ptr)};
		if(!class_info) {
			return false;
		}

		initialize_internal(class_info);
	#endif

		reverse();

		return true;
	}

	MFW_CORE_API void MFW_CORE_CALL rtti::clear()
	{
		baselist.clear();
		name_.clear();
	}
}