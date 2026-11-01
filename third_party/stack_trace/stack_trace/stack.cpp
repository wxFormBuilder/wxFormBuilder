// Copyright 2007 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "stack.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

#if defined(_WIN32)
#include <imagehlp.h>  // link against imagehlp.lib
#include <psapi.h>     // link against psapi

#if defined(__MINGW32__)
#if !defined(_M_ARM64) && !defined(__aarch64__)
// bfd.h requires PACKAGE (normally set by config.h) to be defined
#define PACKAGE
#include <bfd.h>  // link against libbfd and libiberty
#endif
#include <cxxabi.h>
#endif
#elif defined(__GNUC__)
#include <cxxabi.h>
#include <dlfcn.h>
#if defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
#include <execinfo.h>

#include <vector>
#endif
#endif


namespace
{

#if defined(__GNUC__)
std::string demangle(const char* name)
{
    int status = 0;
    char* d = 0;
    std::string ret = name;
    try {
        d = abi::__cxa_demangle(name, 0, 0, &status);
        // Some MinGW symbol names lack the leading underscore of the Itanium mangling prefix
        if (!d && name[0] == 'Z') {
            const std::string prefixed = std::string("_") + name;
            d = abi::__cxa_demangle(prefixed.c_str(), 0, 0, &status);
        }
        if (d)
            ret = d;
    } catch (...) {
    }
    std::free(d);
    return ret;
}
#endif

#if defined(_WIN32)
class uncopyable
{
public:
    uncopyable() {}

private:
    uncopyable(const uncopyable&);             // remains undefined
    uncopyable& operator=(const uncopyable&);  // remains undefined
};

#if defined(__MINGW32__) && !defined(_M_ARM64) && !defined(__aarch64__)
// bfd addresses sections relative to the preferred image base, not the address the module was loaded at
DWORD64 preferred_address(DWORD64 address, DWORD64 module_base)
{
    if (!module_base)
        return address;
    const IMAGE_NT_HEADERS* nt = ImageNtHeader(reinterpret_cast<PVOID>(static_cast<UINT_PTR>(module_base)));
    if (!nt)
        return address;
    return address - module_base + nt->OptionalHeader.ImageBase;
}

class bfd_context : uncopyable
{
private:
    struct find_data {
        std::string func;
        asymbol** symbol_table;
        bfd_vma counter;
    };

public:
    bfd_context() : abfd_(0), sec_(0), symbol_table_(0)
    {
        char procname[MAX_PATH];
        GetModuleFileNameExA(GetCurrentProcess(), NULL, procname, sizeof procname);

        bfd_init();
        abfd_ = bfd_openr(procname, 0);
        if (!abfd_)
            throw dbg::stack_error("Failed to parse object data for the executable");

        char** formats = 0;
        bool b1 = bfd_check_format(abfd_, bfd_object);
        bool b2 = bfd_check_format_matches(abfd_, bfd_object, &formats);
        bool b3 = bfd_get_file_flags(abfd_) & HAS_SYMS;

        if (!(b1 && b2 && b3)) {
            bfd_close(abfd_);
            free(formats);
            throw dbg::stack_error("Failed to parse object data for the executable");
        }
        free(formats);

        // Load symbol table
        unsigned dummy = 0;
        if (
          bfd_read_minisymbols(abfd_, FALSE, (void**)&symbol_table_, &dummy) == 0 &&
          bfd_read_minisymbols(abfd_, TRUE, (void**)&symbol_table_, &dummy) < 0) {
            free(symbol_table_);
            bfd_close(abfd_);
            throw dbg::stack_error("Failed to parse object data for the executable");
        }
    }

    ~bfd_context()
    {
        free(symbol_table_);
        bfd_close(abfd_);
    }

    std::string get_function_name(DWORD64 offset)
    {
        find_data data;
        data.symbol_table = symbol_table_;
        data.counter = static_cast<bfd_vma>(offset);

        bfd_map_over_sections(abfd_, &find_function_name_in_section, &data);

        return data.func;
    }

private:
    static void find_function_name_in_section(bfd* abfd, asection* sec, void* opaque_data)
    {
        assert(sec);
        assert(opaque_data);
        find_data& data = *static_cast<find_data*>(opaque_data);

        if (!data.func.empty())
            return;  // already found it

        auto flags =
#ifdef bfd_get_section_flags
          bfd_get_section_flags(abfd, sec);
#else
          bfd_section_flags(sec);
#endif
        if (!(flags & SEC_ALLOC))
            return;

        auto vma =
#ifdef bfd_get_section_vma
          bfd_get_section_vma(abfd, sec);
#else
          bfd_section_vma(sec);
#endif
        auto size =
#ifdef bfd_get_section_size
          bfd_get_section_size(sec);
#else
          bfd_section_size(sec);
#endif
        if (data.counter < vma || vma + size <= data.counter)
            return;

        const char* func = 0;
        const char* file = 0;
        unsigned line = 0;

        if (bfd_find_nearest_line(abfd, sec, data.symbol_table, data.counter - vma, &file, &func, &line) && func)
            data.func = demangle(func);
    }

private:
    bfd* abfd_;
    asection* sec_;
    asymbol** symbol_table_;
};
#endif  // __MINGW32__

class auto_cast_function_ptr
{
public:
    template <typename FuncPtr>
    explicit auto_cast_function_ptr(FuncPtr f) : fptr_(reinterpret_cast<void (*)(void)>(f))
    {
    }

    template <typename FuncPtr>
    operator FuncPtr() const
    {
        return reinterpret_cast<FuncPtr>(fptr_);
    }

private:
    void (*fptr_)(void);
};

class windows_dll : uncopyable
{
public:
    explicit windows_dll(const std::string& libname) : name_(libname), lib_(LoadLibraryA(name_.c_str()))
    {
        if (!lib_)
            throw dbg::stack_error("Failed to load dll " + name_);
    }

    ~windows_dll() { FreeLibrary(lib_); }

    const std::string& name() const { return name_; }

    auto_cast_function_ptr function(const std::string& func_name) const
    {
        FARPROC proc = GetProcAddress(lib_, func_name.c_str());
        if (!proc)
            throw dbg::stack_error("failed to load function " + func_name + " from library " + name_);

        return auto_cast_function_ptr(proc);
    }

private:
    std::string name_;
    HMODULE lib_;
};

class symbol_context : uncopyable
{
public:
    symbol_context() { SymInitialize(GetCurrentProcess(), 0, true); }
    ~symbol_context() { SymCleanup(GetCurrentProcess()); }
};


class mutex : uncopyable
{
public:
    mutex() { InitializeCriticalSection(&cs_); }
    ~mutex() { DeleteCriticalSection(&cs_); }
    void lock() { EnterCriticalSection(&cs_); }
    void unlock() { LeaveCriticalSection(&cs_); }

private:
    CRITICAL_SECTION cs_;
};

class scoped_lock : uncopyable
{
public:
    scoped_lock(mutex& m) : m_(m) { m_.lock(); }
    ~scoped_lock() { m_.unlock(); }

private:
    mutex& m_;
};

mutex fill_frames_mtx_;


void fill_frames(std::list<dbg::stack_frame>& frames, dbg::stack::depth_type limit, CONTEXT* fromContext = 0)
{
    scoped_lock lk(fill_frames_mtx_);

#if defined(__MINGW32__) && !defined(_M_ARM64) && !defined(__aarch64__)
    static bfd_context bfdc;
#endif

    symbol_context sc;

    STACKFRAME64 frame;
    std::memset(&frame, 0, sizeof frame);
    CONTEXT context;

    if (0 == fromContext) {
        std::memset(&context, 0, sizeof(CONTEXT));
        context.ContextFlags = CONTEXT_FULL;
        windows_dll kernel32("kernel32.dll");

        void(WINAPI * RtlCaptureContext_)(CONTEXT*) = kernel32.function("RtlCaptureContext");
        RtlCaptureContext_(&context);
    } else {
        context = *fromContext;
    }

    DWORD machine_type = IMAGE_FILE_MACHINE_I386;

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_AMD64)  // 64-bit Intel/AMD
    machine_type = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset = context.Rip;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrFrame.Offset = context.Rbp;
#elif defined(__aarch64__) || defined(_M_ARM64)                     // 64-bit ARM64
    machine_type = IMAGE_FILE_MACHINE_ARM64;
    frame.AddrPC.Offset = context.Pc;
    frame.AddrStack.Offset = context.Sp;
    frame.AddrFrame.Offset = context.Fp;
#elif defined(__i386__) || defined(_X86_) || defined(_M_IX86)       // 32-bit Intel
    machine_type = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset = context.Eip;
    frame.AddrStack.Offset = context.Esp;
    frame.AddrFrame.Offset = context.Ebp;
#endif

    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    dbg::stack::depth_type skip = 0;
    bool has_limit = limit != 0;
    char symbol_buffer[sizeof(IMAGEHLP_SYMBOL64) + 255];
    char module_name_raw[MAX_PATH];

    while (StackWalk64(
      machine_type, process, thread, &frame, &context, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0)) {
        if (skip < 1) {
            ++skip;
            continue;
        }

        if (has_limit && limit-- == 0)
            break;

        IMAGEHLP_SYMBOL64* symbol = reinterpret_cast<IMAGEHLP_SYMBOL64*>(symbol_buffer);
        symbol->SizeOfStruct = (sizeof *symbol) + 255;
        symbol->MaxNameLength = 254;

        DWORD64 module_base = SymGetModuleBase64(process, frame.AddrPC.Offset);

        std::string module_name = "[unknown module]";
        if (
          module_base &&
          GetModuleFileNameA(reinterpret_cast<HMODULE>(static_cast<UINT_PTR>(module_base)), module_name_raw, MAX_PATH))
            module_name = module_name_raw;

#if defined(__MINGW32__) && !defined(_M_ARM64) && !defined(__aarch64__)
        std::string func = bfdc.get_function_name(preferred_address(frame.AddrPC.Offset, module_base));

        if (func.empty()) {
            DWORD64 displacement = 0;
            BOOL got_symbol = SymGetSymFromAddr64(process, frame.AddrPC.Offset, &displacement, symbol);
            func = got_symbol ? demangle(symbol->Name) : "[unknown function]";
        }
#else
        DWORD64 displacement = 0;
        BOOL got_symbol = SymGetSymFromAddr64(process, frame.AddrPC.Offset, &displacement, symbol);
#if defined(__GNUC__)
        std::string func = got_symbol ? demangle(symbol->Name) : "[unknown function]";
#else
        std::string func = got_symbol ? symbol->Name : "[unknown function]";
#endif
#endif

        dbg::stack_frame f(reinterpret_cast<const void*>(frame.AddrPC.Offset), func + " in " + module_name);
        frames.push_back(f);
    }
}
#elif defined(__GNUC__)
#if defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
// The unwinder used by backtrace() does not rely on frame pointers, which are usually omitted in optimized builds
__attribute__((noinline)) void fill_frames(std::list<dbg::stack_frame>& frames, dbg::stack::depth_type limit)
{
    // Skip this function and the stack constructor calling it
    const std::size_t skip = 2;
    const bool has_limit = limit != 0;
    std::size_t capacity = has_limit ? limit + skip : 128;

    std::vector<void*> ips;
    std::size_t count = 0;
    for (;;) {
        ips.resize(capacity);
        const int n = backtrace(ips.data(), static_cast<int>(capacity));
        count = n > 0 ? static_cast<std::size_t>(n) : 0;
        if (has_limit || count < capacity)
            break;
        capacity *= 2;
    }

    for (std::size_t i = skip; i < count; ++i) {
        Dl_info info;
        std::string function = "[unknown function]";
        std::string module = "[unknown module]";
        if (dladdr(ips[i], &info)) {
            if (info.dli_sname)
                function = demangle(info.dli_sname);
            if (info.dli_fname)
                module = info.dli_fname;
        }
        frames.push_back(dbg::stack_frame(ips[i], function + " in " + module));
    }
}
#else
// GNU, but neither x86, x86_64 or arm64
// #error "Sorry but dbg::stack is not supported on this architecture"
void fill_frames(std::list<dbg::stack_frame>&, dbg::stack::depth_type)
{
}
#endif
#else
// Unsupported compiler
// #error "Sorry but dbg::stack is not supported on this compiler"
void fill_frames(std::list<dbg::stack_frame>&, dbg::stack::depth_type)
{
}
#endif
}  // namespace


namespace dbg
{
stack_error::stack_error(const std::string& what) : what_(what)
{
}

stack_error::~stack_error() throw()
{
}

const char* stack_error::what() const throw()
{
    return what_.c_str();
}


stack_frame::stack_frame(const void* instruction, const std::string& function) :
    instruction_(instruction), function_(function)
{
}

const void* stack_frame::instruction() const
{
    return instruction_;
}

const std::string& stack_frame::function() const
{
    return function_;
}

std::ostream& operator<<(std::ostream& out, const stack_frame& frame)
{
    return out << frame.instruction() << ": " << frame.function();
}

#if defined(_WIN32)
stack::stack(depth_type limit, CONTEXT* fromContext)
{
    fill_frames(frames_, limit, fromContext);
}
#endif

stack::stack(depth_type limit)
{
    fill_frames(frames_, limit);
}

stack::const_iterator stack::begin() const
{
    return frames_.begin();
}

stack::const_iterator stack::end() const
{
    return frames_.end();
}

stack::depth_type stack::depth() const
{
    return frames_.size();
}

}  // namespace dbg
