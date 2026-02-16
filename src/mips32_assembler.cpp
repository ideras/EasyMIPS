#include "mips32_assembler.h"
#include "easm_error.h"
#include "num_convert.h"
#include "colorizer.h"

namespace Mips32::Assembler
{
    struct RegName
    {
        const char *name1;
        const char *name2;
    };

    static const RegName reg_names[] = {
        {"$r0", "$zero"},
        {"$r1", "$at"},
        {"$r2", "$v0"},
        {"$r3", "$v1"},
        {"$r4", "$a0"},
        {"$r5", "$a1"},
        {"$r6", "$a2"},
        {"$r7", "$a3"},
        {"$r8", "$t0"},
        {"$r9", "$t1"},
        {"$r10", "$t2"},
        {"$r11", "$t3"},
        {"$r12", "$t4"},
        {"$r13", "$t5"},
        {"$r14", "$t6"},
        {"$r15", "$t7"},
        {"$r16", "$s0"},
        {"$r17", "$s1"},
        {"$r18", "$s2"},
        {"$r19", "$s3"},
        {"$r20", "$s4"},
        {"$r21", "$s5"},
        {"$r22", "$s6"},
        {"$r23", "$s7"},
        {"$r24", "$t8"},
        {"$r25", "$t9"},
        {"$r26", "$k0"},
        {"$r27", "$k1"},
        {"$r28", "$gp"},
        {"$r29", "$sp"},
        {"$r30", "$fp"},
        {"$r31", "$ra"},
        {"$r32", "$lo"},
        {"$r33", "$hi"},
        {"$r34", "$pc"},
    };
    static const int Reg_Count = sizeof(reg_names) / sizeof(reg_names[0]);

    static const std::unordered_map<std::string, InstInfo> inst_info = {
        {"add", {"add", Opcode::Add, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"sll", {"sll", Opcode::Sll, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"nop", {"nop", Opcode::Nop, {0, ArgType::None, ArgType::None, ArgType::None}}},
        {"addu", {"addu", Opcode::Addu, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"srl", {"srl", Opcode::Srl, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"and", {"and", Opcode::And, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"sra", {"sra", Opcode::Sra, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"break", {"break", Opcode::Break, {1, ArgType::Imm, ArgType::None, ArgType::None}}},
        {"sllv", {"sllv", Opcode::Sllv, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"div", {"div", Opcode::Div, {2, ArgType::Reg, ArgType::Reg, ArgType::None}}},
        {"srlv", {"srlv", Opcode::Srlv, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"divu", {"divu", Opcode::Divu, {2, ArgType::Reg, ArgType::Reg, ArgType::None}}},
        {"srav", {"srav", Opcode::Srav, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"jalr", {"jalr", Opcode::Jalr, {1, ArgType::Reg, ArgType::None, ArgType::None}}},
        {"jr", {"jr", Opcode::Jr, {1, ArgType::Reg, ArgType::None, ArgType::None}}},
        {"mfhi", {"mfhi", Opcode::Mfhi, {1, ArgType::Reg, ArgType::None, ArgType::None}}},
        {"syscall", {"syscall", Opcode::Syscall, {0, ArgType::None, ArgType::None, ArgType::None}}},
        {"mflo", {"mflo", Opcode::Mflo, {1, ArgType::Reg, ArgType::None, ArgType::None}}},
        {"mthi", {"mthi", Opcode::Mthi, {1, ArgType::Reg, ArgType::None, ArgType::None}}},
        {"mtlo", {"mtlo", Opcode::Mtlo, {1, ArgType::Reg, ArgType::None, ArgType::None}}},
        {"mult", {"mult", Opcode::Mult, {2, ArgType::Reg, ArgType::Reg, ArgType::None}}},
        {"multu", {"multu", Opcode::Multu, {2, ArgType::Reg, ArgType::Reg, ArgType::None}}},
        {"nor", {"nor", Opcode::Nor, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"or", {"or", Opcode::Or, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"slt", {"slt", Opcode::Slt, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"sltu", {"sltu", Opcode::Sltu, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"sub", {"sub", Opcode::Sub, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"subu", {"subu", Opcode::Subu, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"xor", {"xor", Opcode::Xor, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"addi", {"addi", Opcode::Addi, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"beq", {"beq", Opcode::Beq, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"bne", {"bne", Opcode::Bne, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"beqz", {"beqz", Opcode::Beqz, {2, ArgType::Reg, ArgType::Imm}}},
        {"bnez", {"bnez", Opcode::Bnez, {2, ArgType::Reg, ArgType::Imm}}},
        {"bgtz", {"bgtz", Opcode::Bgtz, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
        {"bltz", {"bltz", Opcode::Bltz, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
        {"bgez", {"bgez", Opcode::Bgez, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
        {"blez", {"blez", Opcode::Blez, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
        {"addiu", {"addiu", Opcode::Addiu, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"andi", {"andi", Opcode::Andi, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"slti", {"slti", Opcode::Slti, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"sltiu", {"sltiu", Opcode::Sltiu, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"ori", {"ori", Opcode::Ori, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"xori", {"xori", Opcode::Xori, {3, ArgType::Reg, ArgType::Reg, ArgType::Imm}}},
        {"lui", {"lui", Opcode::Lui, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
        {"lwc1", {"lwc1", Opcode::Lwc1, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"lw", {"lw", Opcode::Lw, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"lb", {"lb", Opcode::Lb, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"lbu", {"lbu", Opcode::Lbu, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"lh", {"lh", Opcode::Lh, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"lhu", {"lhu", Opcode::Lhu, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"sb", {"sb", Opcode::Sb, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"sh", {"sh", Opcode::Sh, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"sw", {"sw", Opcode::Sw, {2, ArgType::Reg, ArgType::BaseOfs, ArgType::None}}},
        {"swc1", {"swc1", Opcode::Swc1, {3, ArgType::Reg, ArgType::Reg, ArgType::Reg}}},
        {"j", {"j", Opcode::J, {1, ArgType::Imm, ArgType::None, ArgType::None}}},
        {"jal", {"jal", Opcode::Jal, {1, ArgType::Imm, ArgType::None, ArgType::None}}},
        {"move", {"move", Opcode::Move, {2, ArgType::Reg, ArgType::Reg, ArgType::None}}},
        {"la", {"la", Opcode::La, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
        {"li", {"li", Opcode::Li, {2, ArgType::Reg, ArgType::Imm, ArgType::None}}},
    };

    Arg::Array::Array(unsigned sz): sz(sz)
    { args = new Arg[sz]; }

    TaskFunction compileInst(Opcode opc, const std::vector<uint32_t>& argv, const EAsm::SrcInfo& src_info)
    {
        uint32_t arg1 = (argv.size()>0)? argv[0] : 0;
        uint32_t arg2 = (argv.size()>1)? argv[1] : 0;
        uint32_t arg3 = (argv.size()>2)? argv[2] : 0;

        switch (opc)
        {
            case Opcode::Add:
                return [arg1, arg2, arg3, si{src_info}](RuntimeContext& ctx)
                {
                    uint32_t rd1 = ctx.reg_file[arg2];
                    uint32_t rd2 = ctx.reg_file[arg3];
                    uint32_t sum = rd1 + rd2;

                    if (!((rd1 ^ rd2) & 0x80000000)  // same sign
                        && ((rd1 ^ sum) & 0x80000000)) // different result sign
                    {
                        ctx.last_error = EAsm::arithOvfError(si, "add",
                                                             static_cast<int32_t>(rd1),
                                                             static_cast<int32_t>(rd2));

                        return ErrorCode::Overflow;
                    }

                    ctx.reg_file.setReg(arg1, sum);
                    return ErrorCode::Ok;
                };
            case Opcode::Addu:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] + ctx.reg_file[arg3]);
                    return ErrorCode::Ok;
                };
            case Opcode::Addi:
                return [arg1, arg2, arg3, si{src_info}] (RuntimeContext& ctx)
                {
                    uint32_t rd1 = ctx.reg_file[arg2];
                    uint32_t rd2 = extend_cast<int16_t, uint32_t>(arg3);
                    uint32_t sum = rd1 + rd2;

                    if (!((rd1 ^ rd2) & 0x80000000) // same sign
                        && ((rd1 ^ sum) & 0x80000000)) // different result sign
                    {
                        ctx.last_error = EAsm::arithOvfError(si, "addi",
                                                             static_cast<int32_t>(rd1),
                                                             static_cast<int32_t>(rd2));

                        return ErrorCode::Overflow;
                    }

                    ctx.reg_file.setReg(arg1, sum);
                    return ErrorCode::Ok;
                };
            case Opcode::Addiu:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] + extend_cast<int16_t, uint32_t>(arg3));
                    return ErrorCode::Ok;
                };
            case Opcode::Sub:
                return [arg1, arg2, arg3, si{src_info}] (RuntimeContext& ctx)
                {
                    uint32_t rd1 = ctx.reg_file[arg2];
                    uint32_t rd2 = ctx.reg_file[arg3];
                    uint32_t sum = rd1 - rd2;

                    // Overflow if operands have different signs AND result has different sign than rd1
                    if (((rd1 ^ rd2) & (rd1 ^ sum) & 0x80000000))
                    {
                        ctx.last_error = EAsm::arithOvfError(si, "sub",
                                                             static_cast<int32_t>(rd1),
                                                             -static_cast<int32_t>(rd2));

                        return ErrorCode::Overflow;
                    }

                    ctx.reg_file.setReg(arg1, sum);
                    return ErrorCode::Ok;
                };
            case Opcode::Subu:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] - ctx.reg_file[arg3]);
                    return ErrorCode::Ok;
                };
            case Opcode::Mult:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    int64_t m = extend_cast<int32_t, int64_t>(ctx.reg_file[arg1])
                                * extend_cast<int32_t, int64_t>(ctx.reg_file[arg2]);

                    ctx.reg_file.setLoReg(m & 0xffffffff);
                    ctx.reg_file.setHiReg((m >> 32) & 0xffffffff);

                    return ErrorCode::Ok;
                };
            case Opcode::Multu:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    uint64_t m = static_cast<uint64_t>(ctx.reg_file[arg1])
                                * static_cast<uint64_t>(ctx.reg_file[arg2]);

                    ctx.reg_file.setLoReg(m & 0xffffffff);
                    ctx.reg_file.setHiReg(m >> 32);

                    return ErrorCode::Ok;
                };
            case Opcode::Nop:
                return [](RuntimeContext& ctx) { return ErrorCode::Ok; };
            case Opcode::Sll:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] << (arg3 & 0x1f));
                    return ErrorCode::Ok;
                };
            case Opcode::Srl:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] >> (arg3 & 0x1f));
                    return ErrorCode::Ok;
                };
            case Opcode::Sra:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    int32_t rd2 = static_cast<int32_t>(ctx.reg_file[arg2]);
                    int32_t shift = arg3 & 0x1f;

                    ctx.reg_file.setReg(arg1, rd2 >> shift);
                    return ErrorCode::Ok;
                };
            case Opcode::Sllv:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = ctx.reg_file[arg3];
                    
                    ctx.reg_file.setReg(arg1, rd2 << (rd3 & 0x1f));
                    return ErrorCode::Ok;
                };
            case Opcode::Srlv:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = ctx.reg_file[arg3];

                    ctx.reg_file.setReg(arg1, rd2 >> (rd3 & 0x1f));
                    return ErrorCode::Ok;
                };
            case Opcode::Srav:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    int32_t rd2 = static_cast<int32_t>(ctx.reg_file[arg2]);
                    int32_t shift = static_cast<int32_t>(ctx.reg_file[arg3]) & 0x1f;

                    ctx.reg_file.setReg(arg1, rd2 >> shift);
                    return ErrorCode::Ok;
                };
            case Opcode::And:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] & ctx.reg_file[arg3]);
                    return ErrorCode::Ok;
                };
            case Opcode::Break:
                return [arg1, si{src_info}](RuntimeContext& ctx)
                {
                    ctx.last_error = EAsm::Error(si, "Breakpoint exception #",
                                                   colorText(fcolor::yellow, arg1),
                                                   '\n');
                    return ErrorCode::Break;
                };
            case Opcode::Div:
                return [arg1, arg2, si{src_info}] (RuntimeContext& ctx)
                {
                    if (ctx.reg_file[arg2] != 0)
                    {
                        int32_t dividend = static_cast<int32_t>(ctx.reg_file[arg1]);
                        int32_t divisor = static_cast<int32_t>(ctx.reg_file[arg2]);

                        if (dividend == INT32_MIN && divisor == -1)
                        {
                            // Avoid SIGFPE on INT_MIN / -1
                            // Result is undefined in MIPS, safe to ignore or set random
                            ctx.reg_file.setLoReg(static_cast<uint32_t>(INT32_MIN));
                            ctx.reg_file.setHiReg(0);
                        }
                        else
                        {
                            int32_t quotient = dividend / divisor;
                            int32_t remainder = dividend % divisor;

                            ctx.reg_file.setLoReg(quotient);
                            ctx.reg_file.setHiReg(remainder);
                        }
                    }
                    return ErrorCode::Ok;
                };
            case Opcode::Divu:
                return [arg1, arg2, si{src_info}](RuntimeContext& ctx)
                {
                    if (ctx.reg_file[arg2] != 0)
                    {
                        uint32_t dividend = ctx.reg_file[arg1];
                        uint32_t divisor = ctx.reg_file[arg2];
                        uint32_t quotient = dividend / divisor;
                        uint32_t remainder = dividend % divisor;

                        ctx.reg_file.setLoReg(quotient);
                        ctx.reg_file.setHiReg(remainder);
                    }
                    return ErrorCode::Ok;
                };
            case Opcode::Jalr:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(RegIndex::Ra, ctx.pc);
                    ctx.pc = ctx.reg_file[arg1];
                    return ErrorCode::Ok;
                };
            case Opcode::Jr:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.pc = ctx.reg_file[arg1];
                    return ErrorCode::Ok;
                };
            case Opcode::J:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.pc = arg1;
                    return ErrorCode::Ok;
                };
            case Opcode::Jal:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(RegIndex::Ra, ctx.pc);
                    ctx.pc = arg1;
                    return ErrorCode::Ok;
                };
            case Opcode::Mfhi:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file.getHiReg());
                    return ErrorCode::Ok;
                };
            case Opcode::Syscall:
                return [si{src_info}](RuntimeContext& ctx)
                {
                    return ctx.syscallHandler(si);
                };
            case Opcode::Mflo:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file.getLoReg());
                    return ErrorCode::Ok;
                };
            case Opcode::Mthi:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.reg_file.setHiReg(ctx.reg_file[arg1]);
                    return ErrorCode::Ok;
                };
            case Opcode::Mtlo:
                return [arg1](RuntimeContext& ctx)
                {
                    ctx.reg_file.setLoReg(ctx.reg_file[arg1]);
                    return ErrorCode::Ok;
                };
            case Opcode::Nor:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ~(ctx.reg_file[arg2] | ctx.reg_file[arg3]));
                    return ErrorCode::Ok;
                };
            case Opcode::Or:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2] | ctx.reg_file[arg3]);
                    return ErrorCode::Ok;
                };
            case Opcode::Slt:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    int32_t rd2 = static_cast<int32_t>(ctx.reg_file[arg2]);
                    int32_t rd3 = static_cast<int32_t>(ctx.reg_file[arg3]);

                    ctx.reg_file.setReg(arg1, rd2 < rd3);
                    return ErrorCode::Ok;
                };
            case Opcode::Sltu:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = ctx.reg_file[arg3];

                    ctx.reg_file.setReg(arg1, rd2 < rd3);
                    return ErrorCode::Ok;
                };
            case Opcode::Xor:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = ctx.reg_file[arg3];
                    
                    ctx.reg_file.setReg(arg1, rd2 ^ rd3);
                    return ErrorCode::Ok;
                };
            case Opcode::Bltz:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    int32_t rd1 = static_cast<int32_t>(ctx.reg_file[arg1]);

                    if (rd1 < 0)
                        ctx.pc = arg2;

                    return ErrorCode::Ok;
                };
            case Opcode::Bgez:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    int32_t rd1 = static_cast<int32_t>(ctx.reg_file[arg1]);

                    if (rd1 >= 0)
                        ctx.pc = arg2;

                    return ErrorCode::Ok;
                };
            case Opcode::Blez:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    int32_t rd1 = static_cast<int32_t>(ctx.reg_file[arg1]);

                    if (rd1 <= 0)
                        ctx.pc = arg2;

                    return ErrorCode::Ok;
                };
            case Opcode::Bgtz:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    int32_t rd1 = static_cast<int32_t>(ctx.reg_file[arg1]);

                    if (rd1 > 0)
                        ctx.pc = arg2;

                    return ErrorCode::Ok;
                };
            case Opcode::Andi:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = arg3 & 0xffff;

                    ctx.reg_file.setReg(arg1, rd2 & rd3);
                    return ErrorCode::Ok;
                };
            case Opcode::Ori:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = arg3 & 0xffff;

                    ctx.reg_file.setReg(arg1, rd2 | rd3);
                    return ErrorCode::Ok;
                };
            case Opcode::Xori:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rd2 = ctx.reg_file[arg2];
                    uint32_t rd3 = arg3 & 0xffff;

                    ctx.reg_file.setReg(arg1, rd2 ^ rd3);
                    return ErrorCode::Ok;
                };
            case Opcode::Beq:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    if (ctx.reg_file[arg1] == ctx.reg_file[arg2])
                        ctx.pc = arg3;

                    return ErrorCode::Ok;
                };
            case Opcode::Beqz:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    if (ctx.reg_file[arg1] == 0)
                        ctx.pc = arg2;

                    return ErrorCode::Ok;
                };
            case Opcode::Bne:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    if (ctx.reg_file[arg1] != ctx.reg_file[arg2])
                        ctx.pc = arg3;

                    return ErrorCode::Ok;
                };
            case Opcode::Bnez:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    if (ctx.reg_file[arg1] != 0)
                        ctx.pc = arg2;

                    return ErrorCode::Ok;
                };
            case Opcode::Slti:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    int32_t rs = static_cast<int32_t>(ctx.reg_file[arg2]);
                    int32_t imm = extend_cast<int16_t, int32_t>(arg3);

                    ctx.reg_file.setReg(arg1, rs < imm);
                    return ErrorCode::Ok;
                };
            case Opcode::Sltiu:
                return [arg1, arg2, arg3](RuntimeContext& ctx)
                {
                    uint32_t rs = ctx.reg_file[arg2];
                    uint32_t imm = extend_cast<int16_t, uint32_t>(arg3);

                    ctx.reg_file.setReg(arg1, rs < imm);
                    return ErrorCode::Ok;
                };
            case Opcode::Sb:
                return [arg1, arg2, arg3, si{src_info}] (RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddr(vaddr))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "sb"),
                                                       '\n');

                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    MemIterator<uint8_t> it = ctx.mm->memIter<uint8_t>(vaddr);
                    *it = static_cast<uint8_t>(ctx.reg_file[arg1]);

                    return ErrorCode::Ok;
                };
            case Opcode::Sh:
                return [arg1, arg2, arg3, si{src_info}] (RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddrRange(vaddr, vaddr + 1))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "sh"),
                                                       '\n');
                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    if ((vaddr % 2) != 0)
                    {
                        ctx.last_error = EAsm::Error(si, "Virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " is not aligned to half word boundaries\n");

                        return ErrorCode::VirtualAddrNotAligned;
                    }
                    MemIterator<uint16_t> it = ctx.mm->memIter<uint16_t>(vaddr);
                    *it = static_cast<uint16_t>(ctx.reg_file[arg1]);

                    return ErrorCode::Ok;
                };
            case Opcode::Sw:
                return [arg1, arg2, arg3, si{src_info}] (RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddrRange(vaddr, vaddr + 3))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "sw"),
                                                       '\n');
                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    if ((vaddr % 4) != 0)
                    {
                        ctx.last_error = EAsm::Error(si, "Virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " is not aligned to word boundaries\n");

                        return ErrorCode::VirtualAddrNotAligned;
                    }
                    MemIterator<uint32_t> it = ctx.mm->memIter<uint32_t>(vaddr);
                    *it = ctx.reg_file[arg1];

                    return ErrorCode::Ok;
                };
            case Opcode::Lw:
                return [arg1, arg2, arg3, si{src_info}](RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddrRange(vaddr, vaddr + 3))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "lw"),
                                                       '\n');

                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    if ((vaddr % 4) != 0)
                    {
                        ctx.last_error = EAsm::Error(si, "Virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " is not aligned to word boundaries\n");

                        return ErrorCode::VirtualAddrNotAligned;
                    }
                    MemIterator<uint32_t> it = ctx.mm->memIter<uint32_t>(vaddr);
                    ctx.reg_file.setReg(arg1, *it);

                    return ErrorCode::Ok;
                };
            case Opcode::Lb:
                return [arg1, arg2, arg3, si{src_info}](RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddr(vaddr))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "lb"),
                                                       '\n');
                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    MemIterator<int8_t> it = ctx.mm->memIter<int8_t>(vaddr);
                    ctx.reg_file.setReg(arg1, static_cast<int32_t>(*it));

                    return ErrorCode::Ok;
                };
            case Opcode::Lbu:
                return [arg1, arg2, arg3, si{src_info}](RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddr(vaddr))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "lbu"),
                                                       '\n');
                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    MemIterator<uint8_t> it = ctx.mm->memIter<uint8_t>(vaddr);
                    ctx.reg_file.setReg(arg1, static_cast<uint32_t>(*it));

                    return ErrorCode::Ok;
                };
            case Opcode::Lh:
                return [arg1, arg2, arg3, si{src_info}](RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddrRange(vaddr, vaddr + 1))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "lh"),
                                                       '\n');
                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    if ((vaddr % 2) != 0)
                    {
                        ctx.last_error = EAsm::Error(si, "Virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " is not aligned to half word boundaries\n");

                        return ErrorCode::VirtualAddrNotAligned;
                    }
                    MemIterator<int16_t> it = ctx.mm->memIter<int16_t>(vaddr);
                    ctx.reg_file.setReg(arg1, static_cast<int32_t>(*it));

                    return ErrorCode::Ok;
                };
            case Opcode::Lhu:
                return [arg1, arg2, arg3, si{src_info}](RuntimeContext& ctx)
                {
                    VirtualAddr vaddr = extend_cast<int16_t, uint32_t>(arg2) + ctx.reg_file[arg3];
                    if (!ctx.mm->isValidAddrRange(vaddr, vaddr + 1))
                    {
                        ctx.last_error = EAsm::Error(si, "Invalid virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " in instruction ",
                                                       cboldText(fcolor::blue, "lhu"),
                                                       '\n');
                        return ErrorCode::VirtualAddrOutOfRange;
                    }
                    if ((vaddr % 2) != 0)
                    {
                        ctx.last_error = EAsm::Error(si, "Virtual address ",
                                                       colorText(fcolor::yellow, Cvt::hexVal(vaddr)),
                                                       " is not aligned to half word boundaries\n");

                        return ErrorCode::VirtualAddrNotAligned;
                    }
                    MemIterator<uint16_t> it = ctx.mm->memIter<uint16_t>(vaddr);
                    ctx.reg_file.setReg(arg1, static_cast<uint32_t>(*it));

                    return ErrorCode::Ok;
                };
            case Opcode::Lui:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, (arg2 & 0xffff) << 16);
                    return ErrorCode::Ok;
                };
            case Opcode::Lwc1:
            case Opcode::Swc1:
                return [] (RuntimeContext& ctx)
                {
                    ctx.last_error = EAsm::Error("Floating point coprocessor is not supported yet\n");
                    return ErrorCode::UnsupportedInst;
                };
            case Opcode::Move:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, ctx.reg_file[arg2]);
                    return ErrorCode::Ok;
                };
            case Opcode::La:
            case Opcode::Li:
                return [arg1, arg2](RuntimeContext& ctx)
                {
                    ctx.reg_file.setReg(arg1, arg2);
                    return ErrorCode::Ok;
                };
            default:
                throw std::runtime_error("Not implemented");
        }
    }

    int getRegIndex(const std::string& name)
    {
        for (int i = 0; i < Reg_Count; i++)
        {
            if ((name == reg_names[i].name1) || (name == reg_names[i].name2))
                return i;
        }
        return -1;
    }

    std::string getRegName(size_t idx)
    {
        return (idx < Reg_Count)? reg_names[idx].name2 : "";
    }

    const InstInfo *getInstInfo(const std::string& name)
    {
        auto it = inst_info.find(name);

        if (it != inst_info.end())
            return &(it->second);

        return nullptr;
    }

} // namespace Mips32::Assembler