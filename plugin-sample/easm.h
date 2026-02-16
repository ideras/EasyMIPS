#ifndef __EASM_H__
#define __EASM_H__

#include <cstdint>
#include <cstddef>

using VirtualAddr = uint32_t;

enum class ErrorCode {
    Ok,
    Overflow,
    DivisionByZero,
    VirtualAddressOutOfRange,
    SyscallNotImplemented,
    SyscallsNotSupported,
    Break,
    Stop,
    Bug,
};

class Register
{
public:
    Register() = delete;

    enum {
        Zero, At, v0, v1, a0, a1, a2, a3,
        t0, t1, t2, t3, t4, t5, t6, t7,
        s0, s1, s2, s3, s4, s5, s6, s7,
        t8, t9, k0, k1, Gp, Sp, Fp, Ra,
        Lo, Hi, Pc
    };
};

struct MemoryMap
{
    long getOffset(VirtualAddr vaddr) const
    {
        if (vaddr >= gbl_start && vaddr < gbl_end) {
            return (vaddr - gbl_start);
        } else if (vaddr >= stk_start && vaddr < stk_end) {
            return ((vaddr - stk_start) + gblWordSize() * 4);
        } else {
            return -1;
        }
    }

    size_t gblWordSize() const { return (gbl_end - gbl_start)/4; }
    size_t stkWordSize() const { return (stk_end - stk_start)/4; }
    size_t wordSize() const { return gblWordSize() + stkWordSize(); }

    VirtualAddr gbl_start;
    VirtualAddr gbl_end;
    VirtualAddr stk_start;
    VirtualAddr stk_end;
};

#endif
