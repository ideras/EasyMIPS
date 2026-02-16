#ifndef __MIPS32_ENV_H__
#define __MIPS32_ENV_H__

#include <string>
#include <vector>
#include <memory>
#include <iosfwd>
#include "mips32_runtime.h"

namespace Mips32
{

class VirtualMachine
{
public:
    VirtualMachine(const MemoryMap& mmap)
    : VirtualMachine(mmap, nullptr, std::cout)
    {}

    VirtualMachine(const MemoryMap& mmap, SyscallHandler esch)
    : VirtualMachine(mmap, esch, std::cout)
    {}

    VirtualMachine(const MemoryMap& mmap, std::ostream& out)
    : VirtualMachine(mmap, nullptr, out)
    {}

    VirtualMachine(const MemoryMap& mmap, SyscallHandler esch, std::ostream& out)
    : mem_map(mmap), out(out), ext_sc_handler(esch)
    { init(); }

    const MemoryMap& memoryMap() { return mem_map; }

    void init();
    
    size_t getInstCount() { return inst_count; }
    size_t getExecTime() { return exec_time_us; }

    int processCliInput(const std::string& input);

    int exec(const std::vector<std::string>& input_files,
             const std::string& entry_label = "");

    const EAsm::Error& lastError()
    { return last_error; }

private:
    int exec(const VmOperationVector& action_v, VirtualAddr entry_point, VirtualAddr initial_ra);

private:
    MemoryMap mem_map;
    std::unique_ptr<MemoryManager> mem_mgr;
    std::unique_ptr<RuntimeContext> rt_ctx;
    SyscallHandler ext_sc_handler;
    std::ostream& out;
    EAsm::Error last_error;
    size_t inst_count;
    size_t exec_time_us;
};

} // namespace Mips32

#endif
