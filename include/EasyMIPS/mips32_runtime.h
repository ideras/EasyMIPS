#ifndef __MIPS32_RUNTIME_H__
#define __MIPS32_RUNTIME_H__

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "mem_iterator.h"
#include "easm_error.h"
#include "sim_runtime.h"

namespace Mips32
{
    using ErrorCode = EAsm::ErrorCode;

    class RegIndex
    {
    public:
        enum
        {
            Zero, At, v0, v1, a0, a1, a2, a3, t0, t1, t2, t3, t4, t5, t6, t7,
            s0, s1, s2, s3, s4, s5, s6, s7, t8, t9, k0, k1, Gp, Sp, Fp, Ra,
            Lo, Hi, Pc
        };

        RegIndex() = delete;
    };

    // Taken from SPIM: http://students.cs.tamu.edu/tanzir/csce350/reference/syscalls.html
    enum class Syscall
    {
        PrintInt = 1,
        PrintString = 4,
        PrintChar = 11,
        ReadInt = 5,
        ReadString = 8,
        ReadChar = 12,
        ExitProgram = 10
    };

    struct MemoryMap
    {
        MemoryMap(VirtualAddr g_start, VirtualAddr s_start,
                  size_t g_size, size_t s_size)
        : gbl_start(g_start), stk_start(s_start),
          gbl_size(g_size), stk_size(s_size)
        {}

        long offsetOf(VirtualAddr vaddr) const
        {
            if (vaddr >= gbl_start && vaddr < (gbl_start + gbl_size))
                return (vaddr - gbl_start);
            else if (vaddr >= stk_start && vaddr < (stk_start + stk_size))
                return ((vaddr - stk_start) + gbl_size);
            else
                return -1;
        }

        VirtualAddr gblStartAddr() const
        { return gbl_start; }

        VirtualAddr gblEndAddr() const
        { return (gbl_start + gbl_size); }

        VirtualAddr stkStartAddr() const
        { return stk_start; }

        VirtualAddr stkEndAddr() const
        { return (stk_start + stk_size); }

        long maxOffset() const
        { return (gbl_size + stk_size - 1); }

        size_t gblSize() const
        { return gbl_size; }

        size_t stkSize() const
        { return stk_size; }

        size_t gblWordSize() const
        { return (gbl_size / 4); }

        size_t stkWordSize() const
        { return (stk_size / 4); }

        size_t wordSize() const
        { return gblWordSize() + stkWordSize(); }

    private:
        VirtualAddr gbl_start;
        VirtualAddr stk_start;
        size_t gbl_size;
        size_t stk_size;
    };

    class MemoryManager
    {
    public:
        MemoryManager(const MemoryMap& mmap)
        : mmap(mmap)
        {
            mem = new uint8_t[mmap.wordSize() * 4];
        }

        ~MemoryManager()
        {
            delete[] mem;
        }

        const MemoryMap& memMap() { return mmap; }

        template <typename T>
        MemIterator<T> memIter(VirtualAddr vaddr)
        {
            long ofs = mmap.offsetOf(vaddr);

            if (ofs < 0)
                return MemIterator<T>();

            return MemIterator<T>(mem, ByteOrder::BigEndian, ofs);
        }

        template <typename T>
        MemIterator<T> memEnd()
        { return MemIterator<T>(mem, ByteOrder::BigEndian, mmap.maxOffset() + 1); }

        bool isValidAddr(VirtualAddr vaddr)
        { return (mmap.offsetOf(vaddr) != -1); }

        bool isValidAddrRange(VirtualAddr vaddr1, VirtualAddr vaddr2)
        {
            return ((mmap.offsetOf(vaddr1) != -1)
                    && (mmap.offsetOf(vaddr2) != -1));
        }

    #ifdef _MIPS32_TESTING
        uint8_t* getMem()
        {
            return mem;
        }
    #endif

    private:
        uint8_t* mem = nullptr;
        MemoryMap mmap;
    };

    struct RuntimeContext;
    class RegFile;

    using TaskFunction = std::function<ErrorCode(RuntimeContext&)>;
    using SyscallHandler = ErrorCode (*)(uint32_t*, void*, const MemoryMap*);

    enum class Reg
    { High, Low };

    class RegFile
    {
    public:
        RegFile()
        {
            // Initialize all registers to 0
            // $zero (index 0) will always remain 0 since setReg prevents writes to it
            std::fill_n(regs, std::size(regs), 0);
        }

        void setHiReg(uint32_t val)
        { regs[RegIndex::Hi] = val; }

        void setLoReg(uint32_t val)
        { regs[RegIndex::Lo] = val; }

        uint32_t getHiReg() const
        { return regs[RegIndex::Hi]; }

        uint32_t getLoReg() const
        { return regs[RegIndex::Lo]; }

        uint32_t *getRegArray()
        { return regs; }

        void setReg(size_t index, uint32_t val)
        {
            if (index != 0)
                regs[index] = val;
        }

        uint32_t operator[] (size_t index) const
        { return regs[index]; }

    private:
        uint32_t regs[32 + 3]; // 32=LO, 33=HI, 34=PC
    };

    struct RuntimeContext
    {
        RuntimeContext();
        RuntimeContext(MemoryManager* mm);
        RuntimeContext(std::ostream& out);
        RuntimeContext(MemoryManager* mm, std::ostream& out);

        ErrorCode syscallHandler(const EAsm::SrcInfo& src_info);

        EAsm::ErrorPair validateAddr(VirtualAddr vaddr, size_t wcount, WordSize ws);

        VirtualAddr getPC() const
        { return reg_file[RegIndex::Pc]; }

        void setPC(VirtualAddr addr)
        { reg_file.setReg(RegIndex::Pc, addr); }

        RegFile reg_file;
        MemoryManager* mm;
        SyscallHandler ext_syscall_handler;
        std::ostream& out;
        EAsm::Error last_error;
    };

    struct VmOperation
    {
        VmOperation() = default;

        VmOperation(const std::string& fname, long line)
        : task(nullptr), src_info(fname, line)
        {}

        TaskFunction task;
        EAsm::SrcInfo src_info;  // For error reporting
    };

    using VmOperationVector = std::vector<VmOperation>;
    using OptVmOperation = std::optional<VmOperation>;
}  // namespace Mips32

#endif
