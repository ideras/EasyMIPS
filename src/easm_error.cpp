#include "easm_error.h"

namespace EAsm
{
    Error errorCodeDesc(const SrcInfo& src_info, ErrorCode ecode)
    {
        switch (ecode)
        {
            case ErrorCode::Overflow:
                return Error(src_info, "An onverflow has ocurred\n");

            case ErrorCode::VirtualAddrOutOfRange:
                return Error(src_info, "Virtual address out of range\n");

            case ErrorCode::VirtualAddrNotAligned:
                return Error(src_info, "Virtual address not aligned\n");

            case ErrorCode::DivisionByZero:
                return Error(src_info, "Division by zero\n");

            case ErrorCode::SyscallNotImplemented:
                return Error(src_info, "Syscall not implemented\n");

            case ErrorCode::Break:
                return Error(src_info, "Breakpoint exception\n");

            case ErrorCode::Bug:
                return Error(src_info, "Oops ... BUG in the machine\n");

            default:
                return Error(src_info, "Unknown error\n");
        }
    }

    Error arithOvfError(const SrcInfo& src_info, const std::string& inst, int32_t arg1, int32_t arg2)
    {
        return EAsm::Error(src_info, "Arithmetic overflow in ",
                           cboldText(fcolor::blue, inst), " instruction. ",
                           "The values that caused the overflow are: ",
                           colorText(fcolor::yellow, arg1),
                           " and ",
                           colorText(fcolor::yellow, arg2),
                           '\n');
    }
} // namespace EAsm