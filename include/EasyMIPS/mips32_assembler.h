#ifndef __MIPS32_ASSEMBLER_H__
#define __MIPS32_ASSEMBLER_H__

#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <optional>
#include <utility>
#include "mips32_runtime.h"

namespace Mips32
{
    enum class Opcode
    {
        Add, Sll, Nop, Addu, Srl, And, Sra, Break, Sllv, Div, Srlv, Divu, Srav, Jalr, Jr,
        Syscall, Mfhi, Mflo, Mthi, Mtlo, Mult, Multu, Nor, Or, Slt, Sltu, Sub, Subu, Xor,
        Bltz, Bgez, Beq, Beqz, Bne, Bnez, Blez, Bgtz, Slti, Lb, Sltiu, Lbu, Lh, Ori, Lhu,
        Addi, Addiu, Andi, Xori, Lui, Lw, Lwc1, Sb, Sh, Sw, Swc1, J, Jal, Move, Li, La,
    };

    enum class ArgType
    { Reg, Imm, BaseOfs, None };

    struct Signature
    {
        int arg_count;
        ArgType arg0;
        ArgType arg1;
        ArgType arg2;
    };

    struct InstInfo
    {
        const char *name;
        Opcode opcode;
        Signature sig;
    };

    namespace Assembler
    {
        class Arg
        {
        public:
            enum class Kind
            { Reg, Imm, StrLit, BaseOffset, MemAddrExpr, Array, None };

            struct Reg
            {
                Reg(size_t index): index_(index)
                {}

                size_t index() const
                { return index_; }

                uint32_t value(const RuntimeContext &ctx) const
                { return ctx.reg_file[index_]; }

            private:
                size_t index_;
            };

            struct BaseOffset
            {
                BaseOffset(uint32_t ofs, Reg reg)
                : basekind_(Kind::Reg), ofs(ofs), base_reg(reg)
                {}

                BaseOffset(uint32_t ofs, uint32_t imm)
                : basekind_(Kind::Imm), ofs(ofs), baseimm_(imm)
                {}

                uint32_t value(const RuntimeContext &ctx) const
                {
                    uint32_t base_val = (basekind_==Kind::Reg)? base_reg.value(ctx) : baseimm_;
                    return (ofs + base_val);
                }

                bool hasBaseReg() const
                { return (basekind_ == Kind::Reg); }

                bool hasBaseImm() const
                { return (basekind_ == Kind::Imm); }

                uint32_t offset() const
                { return ofs; }

                Reg baseReg() const
                { return base_reg; }

                uint32_t baseImm() const
                { return baseimm_; }

            private:
                Kind basekind_;
                uint32_t ofs;
                union {
                    uint32_t baseimm_;
                    Reg base_reg;
                };
            };

            struct MemAddrExpr
            {
                MemAddrExpr(WordSize ws, size_t wcount, const BaseOffset &addre)
                : wsize(ws), oword_count(wcount), addr_expr(addre)
                {}

                MemAddrExpr(WordSize ws, const BaseOffset &addre)
                : wsize(ws), oword_count(std::nullopt), addr_expr(addre)
                {}

                uint32_t value(const RuntimeContext &ctx) const
                { return addr_expr.value(ctx); }

                size_t wordCount() const
                { return (oword_count? *oword_count : 1); }

                size_t wordCount(size_t dcount) const
                { return (oword_count? *oword_count : dcount); }

                size_t byteCount() const
                { return wordCount() * sizeOf(wsize); }

                WordSize wordSize() const
                { return wsize; }

                BaseOffset addrExpr() const
                { return addr_expr; }

            private:
                WordSize wsize;
                std::optional<size_t> oword_count;
                BaseOffset addr_expr;
            };

            class Array
            {
            public:
                Array(): sz(0), args(nullptr)
                {}

                Array(unsigned size);

                Array(Array&& rhs)
                : sz(rhs.sz), args(std::exchange(rhs.args, nullptr))
                {}

                ~Array()
                { delete[] args; }

                Arg& operator[](size_t index)
                { return args[index]; }

                const Arg& operator[](size_t index) const
                { return args[index]; }

                unsigned size() const
                { return sz; }

            private:
                unsigned sz;
                Arg *args;
            };

            Arg(): kind_(Kind::None)
            {}

            Arg(const std::string& strl)
            : kind_(Kind::StrLit), str_lit(strl)
            {}

            Arg(uint32_t imm): kind_(Kind::Imm), imm_(imm)
            {}

            Arg(Reg reg): kind_(Kind::Reg), reg_(reg)
            {}

            Arg(const BaseOffset &bo): kind_(Kind::BaseOffset), base_ofs(bo)
            {}

            Arg(const MemAddrExpr &mae)
            : kind_(Kind::MemAddrExpr), mem_addr_expr(mae)
            {}

            Arg(Array&& arr): kind_(Kind::Array), array_(std::move(arr))
            {}

            Arg(Arg&& rhs) : kind_(rhs.kind_)
            {
                switch (kind_)
                {
                    case Kind::Reg:
                        reg_ = rhs.reg_;
                        break;
                    case Kind::Imm:
                        imm_ = rhs.imm_;
                        break;
                    case Kind::StrLit:
                        new (&str_lit) std::string(std::move(rhs.str_lit));
                        break;
                    case Kind::BaseOffset:
                        base_ofs = rhs.base_ofs;
                        break;
                    case Kind::MemAddrExpr:
                        mem_addr_expr = rhs.mem_addr_expr;
                        break;
                    case Kind::Array:
                        new (&array_) Array(std::move(rhs.array_));
                        break;
                    default:
                        throw std::runtime_error("Invalid call to Arg() move constructor\n");
                }
            }

            uint32_t value(const RuntimeContext& ctx) const
            {
                switch (kind_)
                {
                    case Kind::Reg: return reg_.value(ctx);
                    case Kind::Imm: return imm_;
                    case Kind::BaseOffset: return base_ofs.value(ctx);
                    case Kind::MemAddrExpr: return mem_addr_expr.value(ctx);
                    default:
                        throw std::runtime_error("Invalid call to Arg.value()\n");
                }
            }

            ~Arg()
            {
                if (kind_ == Kind::StrLit)
                    str_lit.~basic_string();
                else if (kind_ == Kind::Array)
                    array_.~Array();
            }

            bool isImm() const
            { return (kind_ == Kind::Imm); }

            bool isReg() const
            { return (kind_ == Kind::Reg); }

            bool isBaseOffset() const
            { return (kind_ == Kind::BaseOffset); }

            bool isMemAddrExpr() const
            { return (kind_ == Kind::MemAddrExpr); }

            bool isStrLit() const
            { return (kind_ == Kind::StrLit); }

            bool isArray() const
            { return (kind_ == Kind::Array); }

            uint32_t imm() const
            { return imm_; }

            Reg reg() const
            { return reg_; }

            const BaseOffset &baseOffset() const
            { return base_ofs; }

            const MemAddrExpr &memAddrExpr() const
            { return mem_addr_expr; }

            std::string strLit() const
            { return str_lit; }

            const Array &array() const
            { return array_; }

        private:
            Kind kind_;

            union
            {
                uint32_t imm_;
                Reg reg_;
                BaseOffset base_ofs;
                MemAddrExpr mem_addr_expr;
                std::string str_lit;
                Array array_;
            };
        };

        enum class Section
        { Data, Code, None };

        class GlobalData
        {
        public:
            GlobalData() = default;

            void init(size_t size)
            {
                if (size > 0)
                {
                    data.resize(size);
                    it = data.begin();
                }
            }

            void writeByte(uint8_t b)
            { *it++ = b; }

            void writeHWord(uint16_t b)
            {
                *it++ = (b >> 8) & 0xff;
                *it++ = b & 0xff;
            }

            void writeWord(uint32_t b)
            {
                *it++ = (b >> 24) & 0xff;
                *it++ = (b >> 16) & 0xff;
                *it++ = (b >> 8) & 0xff;
                *it++ = b & 0xff;
            }

            void align(size_t sz)
            {
                size_t count = std::distance(data.begin(), it);
                size_t new_count = ((count + (sz - 1)) / sz) * sz;
                size_t diff = new_count - count;
                std::advance(it, diff);
            }

            void copyTo(uint8_t *dst)
            { std::memcpy(dst, data.data(), data.size()); }

            const std::vector<uint8_t> &getData() const { return data; }

        private:
            std::vector<uint8_t> data;
            std::vector<uint8_t>::iterator it;
        };

        TaskFunction compileInst(Opcode opc, const std::vector<uint32_t> &argv, const EAsm::SrcInfo& src_info);
        int getRegIndex(const std::string &name);
        std::string getRegName(size_t idx);
        const InstInfo *getInstInfo(const std::string &name);
    }; // namespace Assembler

} // namespace Mips32

#endif
