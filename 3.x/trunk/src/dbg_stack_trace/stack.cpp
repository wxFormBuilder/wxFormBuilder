// Copyright 2007 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iomanip>
#include <sstream>
#include <ostream>
#include <iostream>
#include <cassert>

#include "stack.hpp"

#if defined(_WIN32)
    #include <windows.h>
    #include <imagehlp.h> // link against imagehlp.lib

    #if defined(__MINGW32__)
        #include <bfd.h> // link against libbfd and libiberty
        #include <psapi.h> // link against psapi
        #include <cxxabi.h>
    #endif

#elif defined(__GNUC__)
    #include <dlfcn.h>
    #include <cxxabi.h>
#endif

namespace
{

#if defined(__GNUC__)
    std::string demangle(const char *name)
    {
        int status = 0;
        char *d = 0;
        std::string ret = name;
        try { if ((d = abi::__cxa_demangle(name, 0, 0, &status))) ret = d; }
        catch(...) {  }
        std::free(d);
        return ret;
    }
#endif

#if defined(_WIN32)

    class uncopyable
    {
        public:
            uncopyable() { }

        private:
            uncopyable(const uncopyable &); // remains undefined
            uncopyable &operator= (const uncopyable &); // remains undefined
    };

    #if defined(__MINGW32__)

    class bfd_context : uncopyable
    {
        private:
            struct find_data
            {
                std::string func;
                asymbol **symbol_table;
                bfd_vma counter;
            };

        public:
            bfd_context() :
                abfd_(0),
                sec_(0),
                symbol_table_(0)
            {
                char procname[MAX_PATH];
                GetModuleFileNameExA(GetCurrentProcess(), NULL, procname, sizeof procname);

                bfd_init();
                abfd_ = bfd_openr(procname, 0);
                if (!abfd_)
                    throw dbg::stack_error("Failed to parse object data for the executable");

                char **formats = 0;
                bool b1 = bfd_check_format(abfd_, bfd_object);
                bool b2 = bfd_check_format_matches(abfd_, bfd_object, &formats);
                bool b3 = bfd_get_file_flags(abfd_) & HAS_SYMS;

                if (!(b1 && b2 && b3))
                {
                    bfd_close(abfd_);
                    free(formats);
                    throw dbg::stack_error("Failed to parse object data for the executable");
                }
                free(formats);

                // Load symbol table
                unsigned dummy = 0;
                if (bfd_read_minisymbols(abfd_, FALSE, (void **)&symbol_table_, &dummy) == 0 &&
                    bfd_read_minisymbols(abfd_, TRUE, (void **)&symbol_table_, &dummy) < 0)
                {
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

            std::string get_function_name(DWORD offset)
            {
                find_data data;
                data.symbol_table = symbol_table_;
                data.counter = offset;

                bfd_map_over_sections(abfd_, &find_function_name_in_section, &data);

                return data.func;
            }

        private:
            static void find_function_name_in_section(bfd *abfd, asection *sec, void *opaque_data)
            {
                assert(sec);
                assert(opaque_data);
                find_data &data = *static_cast<find_data *>(opaque_data);

                if (!data.func.empty()) return; // already found it

                if (!(bfd_get_section_flags(abfd, sec) & SEC_ALLOC)) return;

                bfd_vma vma = bfd_get_section_vma(abfd, sec);
                if (data.counter < vma || vma + bfd_get_section_size(sec) <= data.counter) return;

                const char *func = 0;
                const char *file = 0;
                unsigned line = 0;

                if (bfd_find_nearest_line(abfd, sec, data.symbol_table, data.counter - vma, &file, &func, &line) && func)
                    data.func = demangle(func);
            }

        private:
            bfd *abfd_;
            asection *sec_;
            asymbol **symbol_table_;
    };

    #endif // __MINGW32__

    class auto_cast_function_ptr
    {
        public:
            template<typename FuncPtr>
            explicit auto_cast_function_ptr(FuncPtr f) : fptr_(reinterpret_cast<void (*)(void)>(f)) { }

            template<typename FuncPtr>
            operator FuncPtr() const { return reinterpret_cast<FuncPtr>(fptr_); }

        private:
            void (*fptr_)(void);
    };

    class windows_dll : uncopyable
    {
        public:
            explicit windows_dll(const std::string &libname) :
                name_(libname),
                lib_(LoadLibraryA(name_.c_str()))
            {
                if (!lib_) throw dbg::stack_error("Failed to load dll " + name_);
            }

            ~windows_dll() { FreeLibrary(lib_); }

            const std::string &name() const { return name_; }

            auto_cast_function_ptr function(const std::string &func_name) const
            {
                FARPROC proc = GetProcAddress(lib_, func_name.c_str());
                if (!proc) throw dbg::stack_error("failed to load function " + func_name + " from library " + name_);

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
            scoped_lock(mutex &m) : m_(m) { m_.lock(); }
            ~scoped_lock() { m_.unlock(); }
        private:
            mutex &m_;
    };

    mutex fill_frames_mtx_;


    void fill_frames(std::list<dbg::stack_frame> &frames, dbg::stack::depth_type limit, CONTEXT* fromContext = 0)
    {
        scoped_lock lk(fill_frames_mtx_);

        #if defined(__MINGW32__)
            static bfd_context bfdc;
        #endif

        symbol_context sc;

        STACKFRAME frame; std::memset(&frame, 0, sizeof frame);
        CONTEXT context;

        if ( 0 == fromContext )
        {
			std::memset(&context, 0, sizeof(CONTEXT));
			context.ContextFlags = CONTEXT_FULL;
			windows_dll kernel32("kernel32.dll");

			void (WINAPI *RtlCaptureContext_) (CONTEXT*) = kernel32.function("RtlCaptureContext");
			RtlCaptureContext_(&context);
        }
        else
        {
        	context = *fromContext;
        }

        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;

        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();

        dbg::stack::depth_type skip = 0;
        bool has_limit = limit != 0;
        char symbol_buffer[sizeof(IMAGEHLP_SYMBOL) + 255];
        char module_name_raw[MAX_PATH];

        while(StackWalk(IMAGE_FILE_MACHINE_I386, process, thread,
                        &frame, &context, 0, SymFunctionTableAccess, SymGetModuleBase, 0    ))
        {
            if (skip < 1)
            {
                ++skip;
                continue;
            }

            if (has_limit && limit-- == 0) break;

            IMAGEHLP_SYMBOL *symbol = reinterpret_cast<IMAGEHLP_SYMBOL *>(symbol_buffer);
            symbol->SizeOfStruct = (sizeof *symbol) + 255;
            symbol->MaxNameLength = 254;

            DWORD module_base = SymGetModuleBase(process, frame.AddrPC.Offset);
            std::string module_name = "[unknown module]";
            if(   module_base && GetModuleFileNameA(reinterpret_cast<HINSTANCE>(module_base), module_name_raw, MAX_PATH))
                module_name = module_name_raw;

            #if defined(__MINGW32__)
                std::string func = bfdc.get_function_name(frame.AddrPC.Offset);

                if (func.empty())
                {
                    DWORD displacement = 0; // dummy variable
                    BOOL got_symbol = SymGetSymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol);
                    func = got_symbol ? symbol->Name : "[unknown function]";
                }
            #else
                DWORD displacement = 0; // dummy variable
                BOOL got_symbol = SymGetSymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol);
                std::string func = got_symbol ? symbol->Name : "[unknown function]";
            #endif

            dbg::stack_frame f(reinterpret_cast<const void *>(frame.AddrPC.Offset), func + " in " + module_name);
            frames.push_back(f);
        }
    }
#elif defined(__GNUC__)
    #if defined(__i386__)

    void fill_frames(std::list<dbg::stack_frame> &frames, dbg::stack::depth_type limit)
    {
        // Based on code found at:
        // http://www.tlug.org.za/wiki/index.php/Obtaining_a_stack_trace_in_C_upon_SIGSEGV

        Dl_info info;
        void **frame = static_cast<void **>(__builtin_frame_address(0));
        void **bp = static_cast<void **>(*frame);
        void *ip = frame[1];

        bool has_limit = limit != 0;
        dbg::stack::depth_type skip = 0;

        while(bp && ip && dladdr(ip, &info))
        {
            if (skip < 1)
                ++skip;
            else
            {
                if (has_limit && limit-- == 0) break;
                frames.push_back(dbg::stack_frame(ip, demangle(info.dli_sname) + " in " + info.dli_fname));

                if(info.dli_sname && !strcmp(info.dli_sname, "main")) break;
            }

            ip = bp[1];
            bp = static_cast<void**>(bp[0]);
        }
    }

    #elif defined(__ppc__)

    void fill_frames(std::list<dbg::stack_frame> &frames, dbg::stack::depth_type limit)
    {
        // Based on code found at:
        // http://www.informit.com/articles/article.aspx?p=606582&seqNum=4&rl=1

        void *ip = __builtin_return_address(0);
        void **frame = static_cast<void **>(__builtin_frame_address(1));
        bool has_limit = limit != 0;
        Dl_info info;

        do
        {
            if (has_limit && limit-- == 0) break;

            if (dladdr(ip, &info))
                frames.push_back(dbg::stack_frame(ip, demangle(info.dli_sname) + " in " + info.dli_fname));

            if (frame && (frame = static_cast<void**>(*frame))) ip = *(frame + 2);
        }
        while (frame && ip);
    }

    #else
        // GNU, but neither x86 or PPC
        #error "Sorry but dbg::stack is not supported on this architecture"
    #endif
#else
    // Unsupported compiler
    #error "Sorry but dbg::stack is not supported on this compiler"
#endif
}



namespace dbg
{
    stack_error::stack_error(const std::string &what) :
        what_(what)
    {
    }

    stack_error::~stack_error() throw()
    {
    }

    const char *stack_error::what() const throw()
    {
        return what_.c_str();
    }


    stack_frame::stack_frame(const void *instruction, const std::string &function) :
        instruction_(instruction),
        function_(function)
    {
    }

    const void *stack_frame::instruction() const
    {
        return instruction_;
    }

    const std::string &stack_frame::function() const
    {
        return function_;
    }

    std::ostream &operator<< (std::ostream &out, const stack_frame &frame)
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

} // close namespace dbg

