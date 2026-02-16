#include <replxx.hxx>
#include "easm_error.h"
#include "easm_clargs.h"
#include "colorizer.h"
#include "num_convert.h"
#include "native_lib.h"
#include "mips32_vm.h"
#include "mips32_completion.h"

static inline size_t WAlign(size_t size)
{
    return ((size + 3) / 4) * 4;
}

inline std::string& rtrim(std::string& s)
{
    static const char* t = " \t\v";
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

int main(int argc, char *argv[])
{
    EAsm::ClArgs args;

    int res = EAsm::parseClArgs(argc, argv, args);

    if (res > 0) return res;

    if (args.show_help || args.count == 0)
    {
        EAsm::usage(argv[0]);
        return 0;
    }

    Mips32::SyscallHandler ext_syscall_handler = nullptr;
    NativeLib nlib;
    if (!args.sc_plugin_lib.empty())
    {
        nlib.open(args.sc_plugin_lib);
        if (!nlib.isOpen())
        {
            std::cerr << "Cannot open library "
                      << colorText(fcolor::red, args.sc_plugin_lib)
                      << '\n';
            return 1;
        }

        ext_syscall_handler =
            reinterpret_cast<Mips32::SyscallHandler>(nlib.getFuncAddr("handleSyscall"));

        if (ext_syscall_handler == nullptr)
        {
            std::cerr << "Cannot find function "
                      << colorText(fcolor::green, "handleSyscall")
                      << " in library " << colorText(fcolor::magenta, args.sc_plugin_lib)
                      << '\n';
            return 1;
        }
    }

    size_t gbl_size = (args.gbl_size>0)? WAlign(args.gbl_size) : 4096;
    size_t stk_size = (args.stk_size>0)? WAlign(args.stk_size) : 4096;

    Mips32::MemoryMap mmap(0x10000000, (0x7fffeffc - stk_size), gbl_size, stk_size);
    Mips32::VirtualMachine vm(mmap, ext_syscall_handler);

    if (!args.input_files.empty())
    {
        int res = vm.exec(args.input_files, args.entry_label);
        if (res == 0)
        {
            if (args.show_inst_count)
            {
                std::cout << "Number of instructions: "
                        << colorText(fcolor::yellow, vm.getInstCount())
                        << '\n';
            }
            if (args.show_exec_time)
            {
                std::cout << "Execution time: "
                        << colorText(fcolor::yellow, vm.getExecTime())
                        << "us\n";
            }
        }
        else
        {
            std::cerr << vm.lastError();
            return res;
        }

        if (!args.interactive)
            return res;
    }
    
    std::cout << "--- EasyMIPS - " << boldText("MIPS32")
              << " (" << colorText(fcolor::cyan, "big endian")
              << ")\n\n";

    std::cout << "Global base address = "
              << colorText(rang::fg::yellow, Cvt::hexVal(mmap.gblStartAddr())) << '\n'
              << "Stack pointer address = "
              << colorText(rang::fg::yellow, Cvt::hexVal(mmap.stkEndAddr())) << '\n'
              << "Global memory size = "
              << colorText(rang::fg::yellow, mmap.gblWordSize()) << " words\n"
              << "Stack size         = "
              << colorText(rang::fg::yellow, mmap.stkWordSize()) << " words\n\n";

    // Register autocompletion callback with replxx
    replxx::Replxx rx;
    Mips32::registerCompletionCallback(rx);

    const std::string history_file = "history.txt";
    rx.history_load(history_file);

    while (1)
    {
        const char* input = rx.input("ASM> ");
        
        if (input == nullptr) break;

        std::string line = input;
        rtrim(line);

        if (line.empty()) continue;
            
        if (line == "exit" || line == "quit") break;

        rx.history_add(line);
        rx.history_save(history_file);

        if (vm.processCliInput(line) != 0)
        {   
            EAsm::Error last_err = vm.lastError();

            last_err.removeSrcInfo();
            std::cerr << last_err;
        }

        std::cout.flush();
    }

    return 1;
}
