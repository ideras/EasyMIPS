#include <iostream>
#include <algorithm>
#include "num_convert.h"
#include "mips32_runtime.h"

namespace Mips32
{
    inline void trim(std::string &s, const char *t = " \t\n\r\f\v")
    {
        s.erase(0, s.find_first_not_of(t));
        s.erase(s.find_last_not_of(t) + 1);
    }

    RuntimeContext::RuntimeContext()
    : RuntimeContext(nullptr, std::cout)
    {}

    RuntimeContext::RuntimeContext(MemoryManager *mm)
    : RuntimeContext(mm, std::cout)
    {}

    RuntimeContext::RuntimeContext(std::ostream &out)
    : RuntimeContext(nullptr, out)
    {}

    RuntimeContext::RuntimeContext(MemoryManager *mm, std::ostream &out)
    : mm(mm), out(out), ext_syscall_handler(nullptr), last_error()
    {
        if (mm)
        {
            reg_file.setReg(RegIndex::Sp, mm->memMap().stkEndAddr());
            reg_file.setReg(RegIndex::Gp, mm->memMap().gblStartAddr());
        }
        reg_file.setReg(RegIndex::Zero, 0);
    }

    ErrorCode RuntimeContext::syscallHandler(const EAsm::SrcInfo &src_info)
    {
        uint32_t v0 = reg_file[RegIndex::v0];

        switch (static_cast<Syscall>(v0))
        {
            case Syscall::PrintInt:
                out << static_cast<int32_t>(reg_file[RegIndex::a0]);
                break;
            case Syscall::PrintString:
            {
                VirtualAddr vaddr = reg_file[RegIndex::a0];
                if (!mm->isValidAddr(vaddr))
                {
                    last_error = EAsm::Error(src_info,
                                                "Virtual address ",
                                                Cvt::hexVal(vaddr),
                                                " is out of range\n");

                    return ErrorCode::VirtualAddrOutOfRange;
                }

                auto it = mm->memIter<char>(vaddr);
                auto mem_end = mm->memEnd<char>();
                while (it != mem_end)
                {
                    if (*it == '\0')
                        break;

                    out << *it++;
                }

                if (it == mem_end)
                {
                    last_error = EAsm::Error(src_info, "Virtual address ",
                                             Cvt::hexVal(vaddr + it.offset()),
                                             " is out of range\n");

                    return ErrorCode::VirtualAddrOutOfRange;
                }

                break;
            }
            case Syscall::PrintChar:
                out << static_cast<char>(reg_file[RegIndex::a0]);
                break;

            case Syscall::ReadInt:
            {
                std::string input;

                std::getline(std::cin, input);
                trim(input);

                try
                { reg_file.setReg(RegIndex::v0, std::stol(input, nullptr, 10)); }
                catch (...)
                { reg_file.setReg(RegIndex::v0, 0); }
                
                break;
            }
            case Syscall::ReadChar:
            {
                std::string input;

                std::getline(std::cin, input);
                trim(input);

                if (input.empty())
                    reg_file.setReg(RegIndex::v0, 0);
                else
                    reg_file.setReg(RegIndex::v0, input[0]);

                break;
            }
            case Syscall::ReadString:
            {
                size_t len = reg_file[RegIndex::a1];
                VirtualAddr vaddr = reg_file[RegIndex::a0];

                auto res = validateAddr(vaddr, len, WordSize::_8Bit);
                if (res.err_code != ErrorCode::Ok)
                {
                    last_error = EAsm::Error(src_info, std::move(res.err_info), '\n');
                    return ErrorCode::VirtualAddrOutOfRange;
                }

                if (len == 0) break;

                std::string input;
                std::getline(std::cin, input);

                auto it = mm->memIter<char>(vaddr);
                size_t copy_len = std::min(input.length(), len - 1);

                for (size_t i = 0; i < copy_len; ++i)
                    *it++ = input[i];

                *it = '\0';

                break;
            }
            case Syscall::ExitProgram:
                return ErrorCode::Stop;

            default:
                if (ext_syscall_handler != nullptr)
                {
                    ErrorCode ec = ext_syscall_handler(reg_file.getRegArray(),
                                                    nullptr,
                                                    std::addressof(mm->memMap()));

                    if (ec != ErrorCode::Ok)
                    {
                        last_error = EAsm::Error(src_info,
                                                 "Syscall handler failed with syscall number ",
                                                 colorText(fcolor::yellow, reg_file[RegIndex::v0]),
                                                 '\n');

                        return ec;
                    }
                }
                else
                {
                    last_error = EAsm::Error(src_info,
                                             "Syscall number ",
                                             colorText(fcolor::yellow, reg_file[RegIndex::v0]),
                                             " is not implemented\n");

                    return ErrorCode::SyscallNotImplemented;
                }
        }

        return ErrorCode::Ok;
    }

    EAsm::ErrorPair RuntimeContext::validateAddr(VirtualAddr vaddr, size_t wcount, WordSize ws)
    {
        size_t bsize = wcount * sizeOf(ws);

        if (bsize > 1)
        {
            VirtualAddr end_addr = vaddr + bsize - 1;

            if (!mm->isValidAddrRange(vaddr, end_addr))
            {
                return  { 
                            ErrorCode::VirtualAddrOutOfRange,
                            EAsm::makeErrorInfo("Invalid virtual address range ",
                                                colorText(fcolor::yellow, Cvt::hexVal(vaddr)), ":",
                                                colorText(fcolor::yellow, Cvt::hexVal(end_addr)))
                        };
            }
        }
        else
        {
            if (!mm->isValidAddr(vaddr))
            {
                return  {
                            ErrorCode::VirtualAddrOutOfRange,
                            EAsm::makeErrorInfo("Invalid virtual address ",
                                                colorText(fcolor::yellow, Cvt::hexVal(vaddr)))
                        };
            }
        }
        
        if (ws == WordSize::_32Bit && (vaddr % 4) != 0)
        {
            return  {
                        ErrorCode::VirtualAddrNotAligned,
                        EAsm::makeErrorInfo("Virtual address ",
                                            colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                            " is not aligned to word boundaries")
                    };
        }
        if (ws == WordSize::_16Bit && (vaddr % 2) != 0)
        {
            return  {
                        ErrorCode::VirtualAddrNotAligned,
                        EAsm::makeErrorInfo("Virtual address ",
                                            colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                            " is not aligned to half word boundaries")
                    };
        }

        return { ErrorCode::Ok, nullptr };
    }

}  // namespace Mips32