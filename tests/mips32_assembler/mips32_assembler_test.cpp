#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#define _MIPS32_TESTING

#include <sstream>
#include <string>
#include <vector>
#include "doctest.h"
#include "easm_error.h"
#include "mips32_ast.h"
#include "mips32_assembler.h"

#define _AsmPrg node_pool.AsmProgramCreate
#define _Global(lbl) node_pool.GlobalDirCreate(lbl)
#define _ArgList node_pool.ArgListCreate
#define _Inst  node_pool.InstCreate
#define _Set   node_pool.SetCmdCreate
#define _Show  node_pool.ShowCmdCreate
#define _Stop  node_pool.StopCmdCreate
#define _Label node_pool.LabelEntryCreate
#define _EmptyStmt node_pool.EmptyStmtCreate()
#define _BaseOffset node_pool.BaseOffsetCreate
#define _Ident node_pool.IdentCreate
#define _MemRef node_pool.MemRefCreate
#define _Str node_pool.StringCreate
#define _StrLiteral node_pool.StrLiteralCreate
#define _Byte Ast::SS_Byte
#define _HWord Ast::SS_HWord
#define _Word Ast::SS_Word
#define _EmptySS Ast::SS_Empty
#define _EmptyArg node_pool.EmptyArgCreate()
#define _Reg(r) node_pool.RegNameCreate(r)
#define _Dec(v) node_pool.DecConstCreate(v)
#define _Hex(v) node_pool.HexConstCreate(v)
#define _Bin(v) node_pool.BinConstCreate(v)
#define _Char(v) node_pool.CharLiteralCreate(v)
#define _SectionData node_pool.SectionDataCreate()
#define _SectionText node_pool.SectionTextCreate()
#define _ByteData node_pool.ByteDataCreate
#define _HWordData node_pool.HWordDataCreate
#define _WordData node_pool.WordDataCreate
#define _DataArgList node_pool.DataArgListCreate
#define _DecDataArg(v) node_pool.DecConstDataArgCreate(v)
#define _HexDataArg(v) node_pool.HexConstDataArgCreate(v)
#define _BinDataArg(v) node_pool.BinConstDataArgCreate(v)
#define _CharDataArg(v) node_pool.CharLiteralDataArgCreate(v)
#define _StrLiteralDataArg(v) node_pool.StrLiteralDataArgCreate(v)
#define _FillDataArg(v, r) node_pool.FillDataArgCreate(v, r)

namespace Ast = Mips32::Ast;
namespace Asm = Mips32::Assembler;
using opc = Mips32::Opcode;
using Reg = Mips32::RegIndex;
using ErrorCode = EAsm::ErrorCode;

namespace EAsm
{
    std::ostream& operator<<(std::ostream& out, ErrorCode ecode)
    {
        switch (ecode)
        {
            case ErrorCode::Overflow:
                out << "Overflow";
                break;

            case ErrorCode::VirtualAddrOutOfRange:
                out << "VirtualAddrOutOfRange";
                break;

            case ErrorCode::VirtualAddrNotAligned:
                out << "VirtualAddrNotAligned";
                break;

            case ErrorCode::DivisionByZero:
                out << "DivisionByZero";
                break;

            case ErrorCode::SyscallNotImplemented:
                out << "SyscallNotImplemented";
                break;

            case ErrorCode::Break:
                out << "Break";
                break;

            case ErrorCode::Bug:
                out << "Bug";
                break;

            case ErrorCode::Ok:
                out << "Ok";
                break;

            default:
                out << "Unknown";
        }
        return out;
    }
}

class Mips32Regs
{
public:
    Mips32Regs(Mips32::RuntimeContext& ctx)
        : ctx(ctx),
        zero(ctx.reg_file.getRegArray()[Reg::Zero]),
        at(ctx.reg_file.getRegArray()[Reg::At]),
        v0(ctx.reg_file.getRegArray()[Reg::v0]),
        v1(ctx.reg_file.getRegArray()[Reg::v1]),
        a0(ctx.reg_file.getRegArray()[Reg::a0]),
        a1(ctx.reg_file.getRegArray()[Reg::a1]),
        a2(ctx.reg_file.getRegArray()[Reg::a2]),
        a3(ctx.reg_file.getRegArray()[Reg::a3]),
        t0(ctx.reg_file.getRegArray()[Reg::t0]),
        t1(ctx.reg_file.getRegArray()[Reg::t1]),
        t2(ctx.reg_file.getRegArray()[Reg::t2]),
        t3(ctx.reg_file.getRegArray()[Reg::t3]),
        t4(ctx.reg_file.getRegArray()[Reg::t4]),
        t5(ctx.reg_file.getRegArray()[Reg::t5]),
        t6(ctx.reg_file.getRegArray()[Reg::t6]),
        t7(ctx.reg_file.getRegArray()[Reg::t7]),
        s0(ctx.reg_file.getRegArray()[Reg::s0]),
        s1(ctx.reg_file.getRegArray()[Reg::s1]),
        s2(ctx.reg_file.getRegArray()[Reg::s2]),
        s3(ctx.reg_file.getRegArray()[Reg::s3]),
        s4(ctx.reg_file.getRegArray()[Reg::s4]),
        s5(ctx.reg_file.getRegArray()[Reg::s5]),
        s6(ctx.reg_file.getRegArray()[Reg::s6]),
        s7(ctx.reg_file.getRegArray()[Reg::s7]),
        t8(ctx.reg_file.getRegArray()[Reg::t8]),
        t9(ctx.reg_file.getRegArray()[Reg::t9]),
        k0(ctx.reg_file.getRegArray()[Reg::k0]),
        k1(ctx.reg_file.getRegArray()[Reg::k1]),
        gp(ctx.reg_file.getRegArray()[Reg::Gp]),
        sp(ctx.reg_file.getRegArray()[Reg::Sp]),
        fp(ctx.reg_file.getRegArray()[Reg::Fp]),
        ra(ctx.reg_file.getRegArray()[Reg::Ra]),
        hi(ctx.reg_file.getRegArray()[Reg::Hi]),
        lo(ctx.reg_file.getRegArray()[Reg::Lo]),
        pc(ctx.pc)
        {}

public:
    uint32_t& zero;
    uint32_t& at;
    uint32_t& v0;
    uint32_t& v1;
    uint32_t& a0;
    uint32_t& a1;
    uint32_t& a2;
    uint32_t& a3;
    uint32_t& t0;
    uint32_t& t1;
    uint32_t& t2;
    uint32_t& t3;
    uint32_t& t4;
    uint32_t& t5;
    uint32_t& t6;
    uint32_t& t7;
    uint32_t& s0;
    uint32_t& s1;
    uint32_t& s2;
    uint32_t& s3;
    uint32_t& s4;
    uint32_t& s5;
    uint32_t& s6;
    uint32_t& s7;
    uint32_t& t8;
    uint32_t& t9;
    uint32_t& k0;
    uint32_t& k1;
    uint32_t& gp;
    uint32_t& sp;
    uint32_t& fp;
    uint32_t& ra;
    uint32_t& hi;
    uint32_t& lo;
    uint32_t& pc;

private:
    Mips32::RuntimeContext& ctx;
};

TEST_CASE("AST Test")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);

    Ast::AsmProgram *prg = 
        _AsmPrg({
            _Set(_Reg("$t0"), _Dec("10")),
            _Set(_Reg("$t1"), _Dec("10")),
            _Inst("add", _ArgList({ _Reg("$t2"), _Reg("$t0"), _Reg("$t1") })),
            _Label("loop:"),
            _Inst("beq", _ArgList({ _Reg("$t0"), _Reg("$zero"), _Ident("end_loop") })),
            _Inst("addi", _ArgList({ _Reg("$t0"), _Reg("$t0"), _Dec("-1") })),
            _Inst("j", _ArgList({ _Ident("loop") })),
            _Label("end_loop:"),
            _Stop(),
        });

    CHECK(prg->toString() == "#set $t0=10\n"
                             "#set $t1=10\n"
                             "add $t2,$t0,$t1\n"
                             "loop:\n"
                             "beq $t0,$zero,end_loop\n"
                             "addi $t0,$t0,-1\n"
                             "j loop\n"
                             "end_loop:\n"
                             "#stop");
}

TEST_CASE("Resolve labels in code")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);

    Ast::AsmProgram *prg =
        _AsmPrg({ 
            _Global("label1"),
            _Inst("add", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })),
            _Inst("addi", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })),
            _Set(_Reg("$t0"), _Dec("10")),
            _Show(_Reg("$s0"), _EmptyArg, Ast::Fmt_Hex),
            _Inst("sub", _ArgList({ _Reg("$a0"), _Reg("$a0"), _Reg("$zero") })),
            _Label("label1:"),
            _Label("label2:"),
            _Inst("lw", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("0"), _Reg("$s0")) })),
            _Inst("lh", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("0"), _Reg("$s0")) })),
            _Label("label3:"),
            _Inst("mult", _ArgList({ _Reg("$a0"), _Reg("$v0") })),
        });

    Ast::CompileState cst(0x400000, 0x0);

    bool success = true;
    try {
        prg->resolveLabels(cst);
    } catch (EAsm::Error& err) {
        std::cerr << err;
        success = false;
    }
    REQUIRE(success);

    CHECK( cst.global_lbl.size() == 1 );
    REQUIRE( prg->local_lbl.size() == 3 );
    
    VirtualAddr vaddrs[] = { 0x0, 0x400000, 0x400004, 0x400008, 0x40000c,
                             0x400010, 0x400014, 0x400014, 0x400014, 0x400018,
                             0x40001c, 0x40001c };

    REQUIRE( prg->asm_entries.size() == sizeof(vaddrs)/sizeof(vaddrs[0]) );

    auto it1 = prg->local_lbl.find("label1");
    auto it2 = prg->local_lbl.find("label2");
    auto it3 = prg->local_lbl.find("label3");

    REQUIRE(it1 != prg->local_lbl.end());
    REQUIRE(it2 != prg->local_lbl.end());
    REQUIRE(it3 != prg->local_lbl.end());

    CHECK(it1->second->virtual_addr == 0x400014);
    CHECK(it2->second->virtual_addr == 0x400014);
    CHECK(it3->second->virtual_addr == 0x40001c);

    auto it4 = cst.global_lbl.find("label1");
    REQUIRE( it4 != cst.global_lbl.end() );

    CHECK( it4->second->virtual_addr == 0x400014 );

    const Ast::AsmEntryVector& asm_entries = prg->asm_entries;
    for (int i = 0; i < asm_entries.size(); i++)
    {
        INFO("Stmt: " << asm_entries[i]->toString());
        CHECK( asm_entries[i]->virtual_addr == vaddrs[i] );
    }
}

TEST_CASE("Resolve labels in data")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Ast::AsmProgram *prg =
        _AsmPrg({ 
            _SectionData,
            _Label("$LC1:"),
            _ByteData( _DataArgList({
                            _StrLiteralDataArg("Sorted array: "),
                            _DecDataArg("0")
                        })
            ),
            _Label("$LC0:"),
            _WordData( _DataArgList({
                            _DecDataArg("74"),
                            _DecDataArg("34"),
                            _DecDataArg("227")
                        })
            ),
        });

    bool success = true;
    Ast::CompileState cst(0x0, 0x10000000);

    try {
        prg->resolveLabels(cst);
    }
    catch (EAsm::Error& err)
    {
        std::cerr << err;
        success = false;
    }
    REQUIRE(success);

    REQUIRE( prg->local_lbl.size() == 2 );
    
    VirtualAddr vaddrs[] = { 0x10000000, 0x10000010 };
    auto it1 = prg->local_lbl.find("$LC1");
    auto it2 = prg->local_lbl.find("$LC0");

    REQUIRE(it1 != prg->local_lbl.end());
    REQUIRE(it2 != prg->local_lbl.end());

    CHECK(it1->second->virtual_addr == 0x10000000);
    CHECK(it2->second->virtual_addr == 0x10000010);
}

TEST_CASE("Label duplicated")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);

    Ast::AsmProgram *prg =
        _AsmPrg({
            _Inst("li", _ArgList({ _Reg("$t0"), _Dec("10") })),
            _Inst("li", _ArgList({ _Reg("$t1"), _Dec("20") })),
            _Label("loop1:"),
            _Inst("add", _ArgList({ _Reg("$t0"), _Reg("$t0"), _Reg("$t1") })),
            _Inst("j", _ArgList({ _Ident("loop1") })),
            _Label("loop1:"),
            _Inst("addi", _ArgList({ _Reg("$a0"), _Reg("$t0"), _Dec("100") })),
            _Inst("beq", _ArgList({ _Reg("$a0"), _Reg("$zero"), _Ident("loop2") })),
        });

    REQUIRE( prg->toString() == "li $t0,10\n"
                                "li $t1,20\n"
                                "loop1:\n"
                                "add $t0,$t0,$t1\n"
                                "j loop1\n"
                                "loop1:\n"
                                "addi $a0,$t0,100\n"
                                "beq $a0,$zero,loop2" );
 
    Ast::CompileState cst(0x400000, 0x0);

    REQUIRE_THROWS( prg->resolveLabels(cst) );
}

TEST_CASE("Mips32 ISA API")
{
    CHECK(Asm::getRegIndex("$zero") == 0);
    CHECK(Asm::getRegIndex("$ra") == 31);

    CHECK(Asm::getRegName(2) == "$v0");
    CHECK(Asm::getRegName(3) == "$v1");

    const Mips32::InstInfo *ii = Asm::getInstInfo("lw");
    REQUIRE(ii);
    CHECK(ii->sig.arg_count == 2);
}

void testInst(Mips32::RuntimeContext& ctx, Mips32::Opcode opc,
              const std::vector<uint32_t>& args, ErrorCode ecode = ErrorCode::Ok)
{
    Mips32::VmOperation act;

    Mips32::TaskFunction task = Asm::compileInst(opc, args, EAsm::SrcInfo("Test case", 0));
    
    REQUIRE(task != nullptr); 
    ErrorCode err = task(ctx);
    REQUIRE(err == ecode);
}

TEST_CASE("Memory Manager Test")
{
    Mips32::MemoryMap mmap(0x1000, 0xf000, 256, 256);
    Mips32::MemoryManager mm(mmap);
    VirtualAddr vaddrs32[] = {0x1000, 0x1004, 0x10fc, 0xf000, 0xf004, 0xf0fc};
    long offsets[] = {0, 1, 63, 64, 65, 127};
    const int ADDR_COUNT = sizeof(vaddrs32) / sizeof(vaddrs32[0]);

    // Offsets
    REQUIRE( mmap.gblWordSize() == 64 );
    REQUIRE( mmap.stkWordSize() == 64 );

    for (int i = 0; i < ADDR_COUNT; i++)
    {
        INFO("Offset for address: " << std::hex << vaddrs32[i] << std::dec);
        long ofs1 = mmap.offsetOf(vaddrs32[i]);
        long ofs2 = offsets[i] * 4;
        REQUIRE( ofs1 == ofs2 );
    }

    // Word
    uint32_t values32[] = {0xdeadbeef, 0x0badcaca, 0x0badface, 0x1badcafe, 0xbadb1ade, 0xdec0de};
    uint32_t *pword = reinterpret_cast<uint32_t *>(mm.getMem());

    for (int i = 0; i < ADDR_COUNT; i++)
    {
        INFO("[32 bits] Writing address: " << std::hex << vaddrs32[i] << std::dec);
        REQUIRE( mm.isValidAddr(vaddrs32[i]) );
        
        auto it = mm.memIter<uint32_t>(vaddrs32[i]);
        *it = values32[i];
        REQUIRE( pword[offsets[i]] == values32[i] );
    }

    for (int i = 0; i < ADDR_COUNT; i++)
    {
        INFO("[32 bits] Reading address: " << std::hex << vaddrs32[i] << std::dec);
        REQUIRE( mm.isValidAddr(vaddrs32[i]) );
        
        auto it = mm.memIter<uint32_t>(vaddrs32[i]);
        CHECK( *it == values32[i] );
    }

    // Half Word
    VirtualAddr vaddrs16[] = {0x1002, 0x1004, 0x10fc, 0xf002, 0xf004, 0xf0fe};
    uint16_t values16[] = {0xcaca, 0xface, 0xc0de, 0xcafe, 0xbaca, 0x0bad};
    uint32_t evalues32[] = {0xdeadcaca, 0xfacecaca, 0xc0deface, 0x1badcafe, 0xbaca1ade, 0xde0bad};

    for (int i = 0; i < ADDR_COUNT; i++)
    {
        INFO("[16 bits] Writing address: " << std::hex << vaddrs32[i] << std::dec);

        REQUIRE( mm.isValidAddr(vaddrs16[i]) );
        auto it = mm.memIter<uint16_t>(vaddrs16[i]);
        *it = values16[i];
        REQUIRE( pword[offsets[i]] == evalues32[i] );
    }

    for (int i = 0; i < ADDR_COUNT; i++)
    {
        INFO("[16 bits] Reading address: " << std::hex << vaddrs32[i] << std::dec);
        
        REQUIRE( mm.isValidAddr(vaddrs16[i]) );
        auto it = mm.memIter<uint16_t>(vaddrs16[i]);
        CHECK( *it == values16[i] );
    }

    // Byte
    VirtualAddr vaddrs8[] = { 0x1001, 0x1003, 0x1004, 0x1006, 0xf001, 0xf003, 0xf004, 0xf006 };
    uint8_t values8[] = { 0xca, 0xca, 0xfa, 0xce, 0xc0, 0xde, 0xca, 0xfe };
    uint32_t evals32[] = { 0xdecabeef, 0xdecabeca, 0xfaadcaca, 0xfaadceca, 0x1bc0cafe, 0x1bc0cade, 0xcadb1ade, 0xcadbfede };
    long offsets8[] = {0, 0, 1, 1, 64, 64, 65, 65};
    const int BYTE_ADDR_COUNT = sizeof(vaddrs8) / sizeof(vaddrs8[0]);

    // Restore data
    for (int i = 0; i < ADDR_COUNT; i++)
        pword[offsets[i]] = values32[i];
    
    for (int i = 0; i < BYTE_ADDR_COUNT; i++)
    {
        INFO("[8 bits] Writing address: " << std::hex << vaddrs8[i] << std::dec);
        
        REQUIRE( mm.isValidAddr(vaddrs8[i]) );
        auto it = mm.memIter<uint8_t>(vaddrs8[i]);
        *it = values8[i];
        REQUIRE( pword[offsets8[i]] == evals32[i] );
    }

    for (int i = 0; i < BYTE_ADDR_COUNT; i++)
    {
        INFO("[8 bits] Reading address: " << std::hex << vaddrs16[i] << std::dec);

        REQUIRE( mm.isValidAddr(vaddrs8[i]) );
        auto it = mm.memIter<uint8_t>(vaddrs8[i]);
        CHECK( *it == values8[i] );
    }
}

TEST_CASE("Mips32 runtime: Test arith instructions")
{
    Mips32::RuntimeContext ctx;
    Mips32Regs reg(ctx);

    reg.t1 = 20;
    reg.t2 = 10;
    testInst(ctx, opc::Add, {Reg::t0, Reg::t1, Reg::t2});
    CHECK( reg.t0 == 30 );
    testInst(ctx, opc::Addu, {Reg::t0, Reg::t1, Reg::t2});
    CHECK( reg.t0 == 30 );
    testInst(ctx, opc::Sub, {Reg::t0, Reg::t1, Reg::t2});
    CHECK( reg.t0 == 10 );
    testInst(ctx, opc::Subu, {Reg::t0, Reg::t1, Reg::t2});
    CHECK( reg.t0 == 10 );

    reg.t0 = 0xfeaa;
    reg.t1 = 0x70000000;
    reg.t2 = 0x70000000;
    testInst(ctx, opc::Add, {Reg::t0, Reg::t1, Reg::t2}, ErrorCode::Overflow);
    CHECK( reg.t0 == 0xfeaa );
    testInst(ctx, opc::Addu, {Reg::t0, Reg::t1, Reg::t2});
    CHECK( reg.t0 == 0xe0000000 );

    reg.t0 = 0x0;
    reg.t1 = 0x80000000;
    testInst(ctx, opc::Addi, {Reg::t0, Reg::t1, 0x00000fff});
    CHECK(reg.t0 == 0x80000fff);

    reg.t0 = 0xcacafeaa;
    reg.t1 = 0x80000000;
    testInst(ctx, opc::Addi, {Reg::t0, Reg::t1, 0x0000ffff}, ErrorCode::Overflow);
    CHECK(reg.t0 == 0xcacafeaa);

    reg.t1 = 0x80000000;
    testInst(ctx, opc::Addiu, {Reg::t0, Reg::t1, 0x00000fff});
    CHECK(reg.t0 == 0x80000fff);

    reg.t0 = 0xcacafeaa;
    reg.t1 = 0x80000000;
    testInst(ctx, opc::Addiu, {Reg::t0, Reg::t1, 0x0000ffff});
    CHECK(reg.t0 == 0x7fffffff);
    
    reg.t0 = 0xdeadbeef;
    reg.t1 = 0x80000000;
    reg.t2 = 1;
    testInst(ctx, opc::Sub, {Reg::t0, Reg::t1, Reg::t2}, ErrorCode::Overflow);
    CHECK(reg.t0 == 0xdeadbeef);
    testInst(ctx, opc::Subu, {Reg::t0, Reg::t1, Reg::t2});
    CHECK(reg.t0 == 0x7fffffff);

    reg.t0 = 45;
    reg.t1 = 0xffffffff;
    testInst(ctx, opc::Mult, {Reg::t0, Reg::t1});
    CHECK(reg.lo == 0xffffffd3);
    CHECK(reg.hi == 0xffffffff);

    reg.t0 = 45;
    reg.t1 = 0xffffffff;
    testInst(ctx, opc::Multu, {Reg::t0, Reg::t1});
    CHECK( reg.hi == 0x2c );
    CHECK( reg.lo == 0xffffffd3 );

    reg.t0 = 0xffffffd3;
    reg.t1 = 7;
    testInst(ctx, opc::Div, {Reg::t0, Reg::t1});
    CHECK( reg.lo == 0xfffffffa );
    CHECK( reg.hi == 0xfffffffd) ;

    reg.v0 = 0xffffffd3;
    reg.v1 = 7;
    testInst(ctx, opc::Divu, {Reg::v0, Reg::v1});
    CHECK( reg.lo == 613566750u );
    CHECK( reg.hi == 1u );

    // Mfhi
    reg.v0 = 0xaaaabbcc;
    reg.hi = 0xdeadbeef;
    testInst(ctx, opc::Mfhi, {Reg::v0});
    CHECK( reg.v0 == 0xdeadbeef );

    // Mflo
    reg.v0 = 0xaaaabbcc;
    reg.lo = 0xdeadbeef;
    testInst(ctx, opc::Mflo, {Reg::v0});
    CHECK( reg.v0 == 0xdeadbeef );

    // Mthi
    reg.v0 = 0xdeadbeef;
    reg.hi = 0xaaaabbcc;
    testInst(ctx, opc::Mthi, {Reg::v0});
    CHECK( reg.hi == 0xdeadbeef );

    // Mtlo
    reg.v0 = 0xdeadbeef;
    reg.lo = 0xaaaabbcc;
    testInst(ctx, opc::Mtlo, {Reg::v0});
    CHECK( reg.lo == 0xdeadbeef );
}

TEST_CASE("Mips32 runtime: Test bit instructions")
{
    Mips32::RuntimeContext ctx;
    Mips32Regs reg(ctx);

    //And
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0xffff;

    testInst(ctx, opc::And, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0x0000beef );
    
    //Nor
    reg.v0 = 0xdade;
    reg.v1 = 0xdead9449;
    reg.t0 = 0x2aa6;

    testInst(ctx, opc::Nor, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0x21524110 );

    //Or
    reg.v0 = 0xdade;
    reg.v1 = 0xdead9449;
    reg.t0 = 0x2aa6;

    testInst(ctx, opc::Or, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0xdeadbeef );

    //Xor
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0x2aa6;
    testInst(ctx, opc::Xor, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0xdead9449 );

    // Andi
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Andi, {Reg::v0, Reg::v1, 0xffff});
    CHECK( reg.v0 == 0x0000beef );

    // Ori
    reg.v0 = 0xdade;
    reg.v1 = 0xdead9449;
    testInst(ctx, opc::Ori, {Reg::v0, Reg::v1, 0x2aa6});
    CHECK( reg.v0 == 0xdeadbeef );

    // Xori
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Xori, {Reg::v0, Reg::v1, 0x2aa6});
    CHECK( reg.v0 == 0xdead9449 );

    // Sll
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Sll, {Reg::v0, Reg::v1, 16});
    CHECK( reg.v0 == 0xbeef0000 );

    // Srl
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Srl, {Reg::v0, Reg::v1, 16});
    CHECK( reg.v0 == 0xdead );

    // Sra
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Sra, {Reg::v0, Reg::v1, 16});
    CHECK( reg.v0 == 0xffffdead );

    // Sllv
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 16;
    testInst(ctx, opc::Sllv, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0xbeef0000 );

    // Srlv
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 16;
    testInst(ctx, opc::Srlv, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0xdead );

    // Srav
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 16;
    testInst(ctx, opc::Srav, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0xffffdead );
}

TEST_CASE("Mips32 runtime: Test control flow instructions")
{
    Mips32::RuntimeContext ctx;
    Mips32Regs reg(ctx);

    // Jalr
    reg.v0 = 0x400dead;
    reg.pc = 0x400004;
    reg.ra = 0xcaca;
    testInst(ctx, opc::Jalr, {Reg::v0});
    CHECK( reg.ra == 0x400004 );
    CHECK( reg.pc == 0x400dead );
    
    // Jr
    reg.v0 = 0x400dead;
    reg.pc = 0x400000;
    testInst(ctx, opc::Jr, {Reg::v0});
    CHECK( reg.pc == 0x400dead );

    // J
    reg.pc = 0x400000;
    testInst(ctx, opc::J, {0x400dead});
    CHECK( reg.pc == 0x400dead );

    // Jal
    reg.ra = 0xcaca;
    reg.pc = 0x400004;
    testInst(ctx, opc::Jal, {0x400dead});
    CHECK( reg.ra == 0x400004 );
    CHECK( reg.pc == 0x400dead );

    // Beq
    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xdeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Beq, {Reg::v0, Reg::v1, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xeeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Beq, {Reg::v0, Reg::v1, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    // Bne
    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xdeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bne, {Reg::v0, Reg::v1, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xeeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bne, {Reg::v0, Reg::v1, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    // Bltz
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bltz, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bltz, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgtz, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    // Bgez
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgez, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgez, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgez, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    // Blez
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Blez, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Blez, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Blez, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    // Bgtz
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgtz, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgtz, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    testInst(ctx, opc::Bgtz, {Reg::v0, 0x400dead});
    CHECK( reg.pc == 0xcaca );
}

TEST_CASE("Mips32 runtime: Test comparison instructions")
{
    Mips32::RuntimeContext ctx;
    Mips32Regs reg(ctx);

    // Slt
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0;
    testInst(ctx, opc::Slt, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 1 );

    reg.v0 = 0xdade;
    reg.v1 = 0x7eadbeef;
    reg.t0 = 0;
    testInst(ctx, opc::Slt, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0 );

    // Sltu
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0;
    testInst(ctx, opc::Sltu, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 0 );

    reg.v0 = 0xdade;
    reg.v1 = 15;
    reg.t0 = 40958;
    testInst(ctx, opc::Sltu, {Reg::v0, Reg::v1, Reg::t0});
    CHECK( reg.v0 == 1 );

    // Slti
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Slti, {Reg::v0, Reg::v1, 0x0});
    CHECK( reg.v0 == 1 );

    reg.v0 = 0xdade;
    reg.v1 = 0x7eadbeef;
    testInst(ctx, opc::Slti, {Reg::v0, Reg::v1, 0x0});
    CHECK( reg.v0 == 0 );

    // Sltiu
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Sltiu, {Reg::v0, Reg::v1, 0x0});
    CHECK( reg.v0 == 0 );

    reg.v0 = 0xdade;
    reg.v1 = 0x7eadbeef;
    testInst(ctx, opc::Sltiu, {Reg::v0, Reg::v1, 0xbeef});
    CHECK( reg.v0 == 1 );
}

TEST_CASE("Mips32 runtime: Memory instructions")
{
    Mips32::MemoryMap mmap(0x1000, 0xf000, 256, 256);
    Mips32::MemoryManager mm(mmap);
    Mips32::RuntimeContext ctx;

    ctx.mm = &mm;
    Mips32Regs reg(ctx);
    uint32_t *pword = reinterpret_cast<uint32_t *>(mm.getMem());

    // Sw
    reg.t0 = 0xcacafeaa;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Sw, {Reg::t0, 4, Reg::s0});
    CHECK( pword[1] == 0xcacafeaa );
    
    reg.t0 = 0xdeadbeef;
    reg.s0 = 0x1008;
    testInst(ctx, opc::Sw, {Reg::t0, 0xfff8, Reg::s0});
    CHECK( pword[0] == 0xdeadbeef );

    reg.t0 = 0xbacafeaa;
    reg.s0 = 0xf000;
    testInst(ctx, opc::Sw, {Reg::t0, 4, Reg::s0});
    CHECK( pword[65] == 0xbacafeaa );

    // Sh
    pword[1] = 0xbacadead;
    reg.t0 = 0xaabbc0de;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Sh, {Reg::t0, 6, Reg::s0});
    CHECK( pword[1] == 0xbacac0de );

    pword[65] = 0x0000dead;
    reg.t0 = 0xaabbbaca;
    reg.s0 = 0xf000;
    testInst(ctx, opc::Sh, {Reg::t0, 4, Reg::s0});
    CHECK( pword[65] == 0xbacadead );

    // Sb
    pword[1] = 0xbacadead;
    reg.t0 = 0x33;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Sb, {Reg::t0, 6, Reg::s0});
    CHECK( pword[1] == 0xbaca33ad );
    reg.t0 = 0x11;
    testInst(ctx, opc::Sb, {Reg::t0, 4, Reg::s0});
    CHECK( pword[1] == 0x11ca33ad );
    reg.t0 = 0x44;
    testInst(ctx, opc::Sb, {Reg::t0, 7, Reg::s0});
    CHECK( pword[1] == 0x11ca3344 );
    reg.t0 = 0x22;
    testInst(ctx, opc::Sb, {Reg::t0, 5, Reg::s0});
    CHECK( pword[1] == 0x11223344 );

    // Lw
    pword[1] = 0xdec0de;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Lw, {Reg::t0, 4, Reg::s0});
    CHECK( reg.t0 == 0xdec0de );

    // Lh
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Lh, {Reg::t0, 4, Reg::s0});
    CHECK( reg.t0 == 0xffffaa22 );
    testInst(ctx, opc::Lh, {Reg::t0, 6, Reg::s0});
    CHECK( reg.t0 == 0x11bb );

    // Lhu
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Lhu, {Reg::t0, 4, Reg::s0});
    CHECK( reg.t0 == 0xaa22 );
    testInst(ctx, opc::Lhu, {Reg::t0, 6, Reg::s0});
    CHECK( reg.t0 == 0x11bb );

    // Lb
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Lb, {Reg::t0, 4, Reg::s0});
    CHECK( reg.t0 == 0xffffffaa );
    testInst(ctx, opc::Lb, {Reg::t0, 5, Reg::s0});
    CHECK( reg.t0 == 0x22 );
    testInst(ctx, opc::Lb, {Reg::t0, 6, Reg::s0});
    CHECK( reg.t0 == 0x11 );
    testInst(ctx, opc::Lb, {Reg::t0, 7, Reg::s0});
    CHECK( reg.t0 == 0xffffffbb );

    // Lbu
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    testInst(ctx, opc::Lbu, {Reg::t0, 4, Reg::s0});
    CHECK( reg.t0 == 0xaa );
    testInst(ctx, opc::Lbu, {Reg::t0, 5, Reg::s0});
    CHECK( reg.t0 == 0x22 );
    testInst(ctx, opc::Lbu, {Reg::t0, 6, Reg::s0});
    CHECK( reg.t0 == 0x11 );
    testInst(ctx, opc::Lbu, {Reg::t0, 7, Reg::s0});
    CHECK( reg.t0 == 0xbb );
}

TEST_CASE("Mips32 runtime: Test misc instructions")
{
    Mips32::RuntimeContext ctx;
    Mips32Regs reg(ctx);

    // Lui
    reg.v0 = 0xcaca2fea;
    testInst(ctx, opc::Lui, {Reg::v0, 0xdead});
    CHECK( reg.v0 == 0xdead0000 );

    // Move
    reg.v0 = 0xba5474;
    reg.v1 = 0xdeadbeef;
    testInst(ctx, opc::Move, {Reg::v0, Reg::v1});
    CHECK( reg.v0 == 0xdeadbeef );

    // La
    reg.v0 = 0xba5474;
    testInst(ctx, opc::La, {Reg::v0, 0xdeadbeef});
    CHECK( reg.v0 == 0xdeadbeef );

    // Li
    reg.v0 = 0xba5474;
    testInst(ctx, opc::Li, {Reg::v0, 0xdeadbeef});
    CHECK( reg.v0 == 0xdeadbeef );
}

void compileAndRun(Mips32::RuntimeContext& rctx, Ast::Stmt *stmt,
                   ErrorCode ecode = ErrorCode::Ok, bool req_success = true)
{
    std::vector<Mips32::VmOperation> actions;
    bool success;

    INFO(stmt->toString());
    try {
        Ast::CompileState cst(0x400, 0x100);
        auto vm_oper = Ast::compileEntry(stmt, nullptr, cst, EAsm::SrcInfo("Test case", 0));

        REQUIRE(vm_oper != std::nullopt);

        ErrorCode ec = vm_oper->task(rctx);

        if (ec != ErrorCode::Ok)
            std::cerr << rctx.last_error;

        REQUIRE( ec == ecode );

        success = true;
    } catch (EAsm::Error& err) {
        std::cerr << err;
        success = false;
    }

    CHECK( success == req_success );
}

TEST_CASE("Mips32 assembler: Test arith instructions")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

    reg.t1 = 20;
    reg.t2 = 10;
    compileAndRun( rctx, _Inst("add", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })) );
    CHECK( reg.t0 == 30 );
    compileAndRun( rctx, _Inst("addu", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })) );
    CHECK( reg.t0 == 30 );
    compileAndRun( rctx, _Inst("sub", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })) );
    CHECK( reg.t0 == 10 );
    compileAndRun( rctx, _Inst("subu", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })) );
    CHECK( reg.t0 == 10 );

    reg.t0 = 0xfeaa;
    reg.t1 = 0x70000000;
    reg.t2 = 0x70000000;
    compileAndRun( rctx, _Inst("add", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })), ErrorCode::Overflow );
    CHECK( reg.t0 == 0xfeaa );
    compileAndRun( rctx, _Inst("addu", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })) );
    CHECK( reg.t0 == 0xe0000000 );

    reg.t1 = 0x80000000;
    compileAndRun( rctx, _Inst("addi", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Hex("0x00000fff") })) );
    CHECK(reg.t0 == 0x80000fff);

    reg.t0 = 0xcacafeaa;
    reg.t1 = 0x80000000;
    compileAndRun( rctx, _Inst("addi", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Hex("0x0000ffff") })), ErrorCode::Overflow);
    CHECK(reg.t0 == 0xcacafeaa);

    reg.t1 = 0x80000000;
    compileAndRun( rctx, _Inst("addiu", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Hex("0x00000fff") })) );
    CHECK(reg.t0 == 0x80000fff);

    reg.t0 = 0xcacafeaa;
    reg.t1 = 0x80000000;
    compileAndRun( rctx, _Inst("addiu", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Hex("0x0000ffff") })) );
    CHECK(reg.t0 == 0x7fffffff);
    
    reg.t0 = 0xdeadbeef;
    reg.t1 = 0x80000000;
    reg.t2 = 1;
    compileAndRun( rctx, _Inst("sub", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })), ErrorCode::Overflow );
    CHECK(reg.t0 == 0xdeadbeef);
    compileAndRun( rctx, _Inst("subu", _ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") })) );
    CHECK(reg.t0 == 0x7fffffff);

    reg.t0 = 45;
    reg.t1 = 0xffffffff;
    compileAndRun( rctx, _Inst("mult", _ArgList({ _Reg("$t0"), _Reg("$t1") })) );
    CHECK(reg.lo == 0xffffffd3);
    CHECK(reg.hi == 0xffffffff);

    reg.t0 = 45;
    reg.t1 = 0xffffffff;
    compileAndRun( rctx, _Inst("multu", _ArgList({ _Reg("$t0"), _Reg("$t1") })) );
    CHECK( reg.hi == 0x2c );
    CHECK( reg.lo == 0xffffffd3 );

    reg.t0 = 0xffffffd3;
    reg.t1 = 7;
    compileAndRun( rctx, _Inst("div", _ArgList({ _Reg("$t0"), _Reg("$t1") })) );
    CHECK( reg.lo == 0xfffffffa );
    CHECK( reg.hi == 0xfffffffd) ;

    reg.v0 = 0xffffffd3;
    reg.v1 = 7;
    compileAndRun( rctx, _Inst("divu", _ArgList({ _Reg("$t0"), _Reg("$t1") })) );
    CHECK( reg.lo == 613566750u );
    CHECK( reg.hi == 1u );

    // Mfhi
    reg.v0 = 0xaaaabbcc;
    reg.hi = 0xdeadbeef;
    compileAndRun( rctx, _Inst("mfhi", _ArgList({ _Reg("$v0") })) );
    CHECK( reg.v0 == 0xdeadbeef );

    // Mflo
    reg.v0 = 0xaaaabbcc;
    reg.lo = 0xdeadbeef;
    compileAndRun( rctx, _Inst("mflo", _ArgList({ _Reg("$v0") })) );
    CHECK( reg.v0 == 0xdeadbeef );

    // Mthi
    reg.v0 = 0xdeadbeef;
    reg.hi = 0xaaaabbcc;
    compileAndRun( rctx, _Inst("mthi", _ArgList({ _Reg("$v0") })) );
    CHECK( reg.hi == 0xdeadbeef );

    // Mtlo
    reg.v0 = 0xdeadbeef;
    reg.lo = 0xaaaabbcc;
    compileAndRun( rctx, _Inst("mtlo", _ArgList({ _Reg("$v0") })) );
    CHECK( reg.lo == 0xdeadbeef );
}

TEST_CASE("Mips32 assembler: Test bit instructions")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

    //And
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0xffff;
    compileAndRun( rctx, _Inst("and", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0x0000beef );
    
    //Nor
    reg.v0 = 0xdade;
    reg.v1 = 0xdead9449;
    reg.t0 = 0x2aa6;
    compileAndRun( rctx, _Inst("nor", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0x21524110 );

    //Or
    reg.v0 = 0xdade;
    reg.v1 = 0xdead9449;
    reg.t0 = 0x2aa6;
    compileAndRun( rctx, _Inst("or", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0xdeadbeef );

    //Xor
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0x2aa6;
    compileAndRun( rctx, _Inst("xor", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0xdead9449 );

    // Andi
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("andi", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0xffff") })) );
    CHECK( reg.v0 == 0x0000beef );

    // Ori
    reg.v0 = 0xdade;
    reg.v1 = 0xdead9449;
    compileAndRun( rctx, _Inst("ori", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0x2aa6") })) );
    CHECK( reg.v0 == 0xdeadbeef );

    // Xori
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("xori", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0x2aa6") })) );
    CHECK( reg.v0 == 0xdead9449 );

    // Sll
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("sll", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Dec("16") })) );
    CHECK( reg.v0 == 0xbeef0000 );

    // Srl
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("srl", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Dec("16") })) );
    CHECK( reg.v0 == 0xdead );

    // Sra
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("sra", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Dec("16") })) );
    CHECK( reg.v0 == 0xffffdead );

    // Sllv
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 16;
    compileAndRun( rctx, _Inst("sllv", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0xbeef0000 );

    // Srlv
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 16;
    compileAndRun( rctx, _Inst("srlv", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0xdead );

    // Srav
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 16;
    compileAndRun( rctx, _Inst("srav", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0xffffdead );
}

TEST_CASE("Mips32 assembler: Test control flow instructions")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

    // Jalr
    reg.v0 = 0x400dead;
    reg.pc = 0x400004;
    reg.ra = 0xcaca;
    compileAndRun( rctx, _Inst("jalr", _ArgList({ _Reg("$v0") })) );
    CHECK( reg.ra == 0x400004 );
    CHECK( reg.pc == 0x400dead );
    
    // Jr
    reg.v0 = 0x400dead;
    reg.pc = 0x400000;
    compileAndRun( rctx, _Inst("jr", _ArgList({ _Reg("$v0") })) );
    CHECK( reg.pc == 0x400dead );

    // J
    reg.pc = 0x400000;
    compileAndRun( rctx, _Inst("j", _ArgList({ _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    // Jal
    reg.ra = 0xcaca;
    reg.pc = 0x400004;
    compileAndRun( rctx, _Inst("jal", _ArgList({ _Hex("0x400dead") })) );
    CHECK( reg.ra == 0x400004 );
    CHECK( reg.pc == 0x400dead );

    // Beq
    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("beq", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xeeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("beq", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    // Beqz
    reg.v0 = 0;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("beqz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("beqz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    // Bne
    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bne", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0xdeadbeef;
    reg.v1 = 0xeeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bne", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    // Bnez
    reg.v0 = 0;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bnez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bnez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    // Bltz
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bltz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bltz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bltz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    // Bgez
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bgez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bgez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bgez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    // Blez
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("blez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("blez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("blez", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    // Bgtz
    reg.v0 = 0xdeadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bgtz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );

    reg.v0 = 0x7eadbeef;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bgtz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0x400dead );

    reg.v0 = 0;
    reg.pc = 0xcaca;
    compileAndRun( rctx, _Inst("bgtz", _ArgList({ _Reg("$v0"), _Hex("0x400dead") })) );
    CHECK( reg.pc == 0xcaca );
}

TEST_CASE("Mips32 assembler: Test comparison instructions")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

    // Slt
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0;
    compileAndRun( rctx, _Inst("slt", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 1 );

    reg.v0 = 0xdade;
    reg.v1 = 0x7eadbeef;
    reg.t0 = 0;
    compileAndRun( rctx, _Inst("slt", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0 );

    // Sltu
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    reg.t0 = 0;
    compileAndRun( rctx, _Inst("sltu", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 0 );

    reg.v0 = 0xdade;
    reg.v1 = 15;
    reg.t0 = 40958;
    compileAndRun( rctx, _Inst("sltu", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Reg("$t0") })) );
    CHECK( reg.v0 == 1 );

    // Slti
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("slti", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Dec("0") })) );
    CHECK( reg.v0 == 1 );

    reg.v0 = 0xdade;
    reg.v1 = 0x7eadbeef;
    compileAndRun( rctx, _Inst("slti", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Dec("0") })) );
    CHECK( reg.v0 == 0 );

    // Sltiu
    reg.v0 = 0xdade;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("sltiu", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Dec("0") })) );
    CHECK( reg.v0 == 0 );

    reg.v0 = 0xdade;
    reg.v1 = 0x7eadbeef;
    compileAndRun( rctx, _Inst("sltiu", _ArgList({ _Reg("$v0"), _Reg("$v1"), _Hex("0xbeef") })) );
    CHECK( reg.v0 == 1 );
}

TEST_CASE("Mips32 assembler: Memory instructions")
{
    Mips32::MemoryMap mmap(0x1000, 0xf000, 256, 256);
    Mips32::MemoryManager mm(mmap);
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

    rctx.mm = &mm;
    uint32_t *pword = reinterpret_cast<uint32_t *>(mm.getMem());

    // Sw
    reg.t0 = 0xcacafeaa;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("sw", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( pword[1] == 0xcacafeaa );
    
    reg.t0 = 0xdeadbeef;
    reg.s0 = 0x1008;
    compileAndRun( rctx, _Inst("sw", _ArgList({ _Reg("$t0"), _BaseOffset(_Hex("0xfff8"), _Reg("$s0")) })) );
    CHECK( pword[0] == 0xdeadbeef );

    reg.t0 = 0xbacafeaa;
    reg.s0 = 0xf000;
    compileAndRun( rctx, _Inst("sw", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( pword[65] == 0xbacafeaa );

    // Sh
    pword[1] = 0xbacadead;
    reg.t0 = 0xaabbc0de;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("sh", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("6"), _Reg("$s0")) })) );
    CHECK( pword[1] == 0xbacac0de );

    pword[65] = 0x0000dead;
    reg.t0 = 0xaabbbaca;
    reg.s0 = 0xf000;
    compileAndRun( rctx, _Inst("sh", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( pword[65] == 0xbacadead );

    // Sb
    pword[1] = 0xbacadead;
    reg.t0 = 0x33;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("sb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("6"), _Reg("$s0")) })) );
    CHECK( pword[1] == 0xbaca33ad );
    reg.t0 = 0x11;
    compileAndRun( rctx, _Inst("sb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( pword[1] == 0x11ca33ad );
    reg.t0 = 0x44;
    compileAndRun( rctx, _Inst("sb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("7"), _Reg("$s0")) })) );
    CHECK( pword[1] == 0x11ca3344 );
    reg.t0 = 0x22;
    compileAndRun( rctx, _Inst("sb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("5"), _Reg("$s0")) })) );
    CHECK( pword[1] == 0x11223344 );

    // Lw
    pword[1] = 0xdec0de;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("lw", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xdec0de );

    // Lh
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("lh", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xffffaa22 );
    compileAndRun( rctx, _Inst("lh", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("6"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0x11bb );

    // Lhu
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("lhu", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xaa22 );
    compileAndRun( rctx, _Inst("lhu", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("6"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0x11bb );

    // Lb
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("lb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xffffffaa );
    compileAndRun( rctx, _Inst("lb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("5"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0x22 );
    compileAndRun( rctx, _Inst("lb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("6"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0x11 );
    compileAndRun( rctx, _Inst("lb", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("7"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xffffffbb );

    // Lbu
    pword[1] = 0xaa2211bb;
    reg.s0 = 0x1000;
    compileAndRun( rctx, _Inst("lbu", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("4"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xaa );
    compileAndRun( rctx, _Inst("lbu", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("5"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0x22 );
    compileAndRun( rctx, _Inst("lbu", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("6"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0x11 );
    compileAndRun( rctx, _Inst("lbu", _ArgList({ _Reg("$t0"), _BaseOffset(_Dec("7"), _Reg("$s0")) })) );
    CHECK( reg.t0 == 0xbb );
}

TEST_CASE("Mips32 assembler: Test misc instructions")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

    // Lui
    reg.v0 = 0xcaca2fea;
    compileAndRun( rctx, _Inst("lui", _ArgList({ _Reg("$v0"), _Hex("0xdead") })) );
    CHECK( reg.v0 == 0xdead0000 );

    // Move
    reg.v0 = 0xba5474;
    reg.v1 = 0xdeadbeef;
    compileAndRun( rctx, _Inst("move", _ArgList({ _Reg("$v0"), _Reg("$v1") })) );
    CHECK( reg.v0 == 0xdeadbeef );

    // La
    reg.v0 = 0xba5474;
    compileAndRun( rctx, _Inst("la", _ArgList({ _Reg("$v0"), _Hex("0xdeadbeef") })) );
    CHECK( reg.v0 == 0xdeadbeef );

    // Li
    reg.v0 = 0xba5474;
    compileAndRun( rctx, _Inst("li", _ArgList({ _Reg("$v0"), _Hex("0xdeadbeef") })) );
    CHECK( reg.v0 == 0xdeadbeef );
    
    reg.v0 = 0xba5474;
    compileAndRun( rctx, _Inst("li", _ArgList({ _Reg("$v0"), _Bin("0b111") })) );
    CHECK( reg.v0 == 0x7 );
}

TEST_CASE("Mips32 assembler: Syscalls")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    std::ostringstream out;
    Mips32::RuntimeContext rctx(out);
    Mips32Regs reg(rctx);

    // Print integer
    out.str("");
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::PrintInt);
    reg.a0 = 94786;
    compileAndRun( rctx, _Inst("syscall", _EmptyArg) );
    CHECK( out.str() == "94786" );

    // Print char
    out.str("");
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::PrintChar);
    reg.a0 = 'H';
    compileAndRun( rctx, _Inst("syscall", _EmptyArg) );
    CHECK( out.str() == "H" );

    Mips32::MemoryMap mmap(0x1000, 0xf000, 256, 256);
    Mips32::MemoryManager mm(mmap);
    static const char *str = "Hello World";

    VirtualAddr vaddr = 0x1000;
    auto it = mm.memIter<char>(vaddr);
    const char *p = str;
    while (*p != '\0')
        *it++ = *p++;

    *it = '\0';
    
    rctx.mm = &mm;

    // Print string
    out.str("");
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::PrintString);
    reg.a0 = 0x1000;
    compileAndRun( rctx, _Inst("syscall", _EmptyArg) );
    CHECK( out.str() == "Hello World" );

    // Read integer
    std::istringstream in("56789\n");
    std::streambuf *cin_buff = std::cin.rdbuf();

    std::cin.rdbuf(in.rdbuf());
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::ReadInt);
    compileAndRun( rctx, _Inst("syscall", _EmptyArg) );
    CHECK( reg.v0 == 56789 );
    std::cin.rdbuf(cin_buff);

    // Read char
    in.str("CAB\n");

    std::cin.rdbuf(in.rdbuf());
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::ReadChar);
    compileAndRun( rctx, _Inst("syscall", _EmptyArg) );
    CHECK( reg.v0 == 'C' );
    std::cin.rdbuf(cin_buff);

    // Read String
    in.str("Hello World\n");

    std::cin.rdbuf(in.rdbuf());
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::ReadString);
    reg.a0 = 0x1000;
    reg.a1 = 12;
    compileAndRun( rctx, _Inst("syscall", _EmptyArg) );
    
    char *pmem = reinterpret_cast<char *>(mm.getMem());
    char estr[] = {'l', 'l', 'e', 'H', 'o', 'W', ' ', 'o', '\0', 'd', 'l', 'r'};

    CHECK( memcmp(pmem, estr, sizeof(estr)) == 0 );
    std::cin.rdbuf(cin_buff);

    // Exit program
    reg.v0 = static_cast<unsigned>(Mips32::Syscall::ExitProgram);
    compileAndRun( rctx, _Inst("syscall", _EmptyArg), ErrorCode::Stop, true);
}

TEST_CASE("Mips32 assembler: show command")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    std::ostringstream out;
    Mips32::RuntimeContext rctx(out);
    Mips32Regs reg(rctx);

    reg.v0 = 0xcaca2fea;
    out.str("");
    compileAndRun( rctx, _Show(_Reg("$v0"), _EmptyArg, Ast::Fmt_Auto) );
    CHECK( out.str() == "$v0 = 3402248170\n" );
    
    out.str("");
    compileAndRun( rctx, _Show(_Reg("$v0"), _EmptyArg, Ast::Fmt_UDec) );
    CHECK( out.str() == "$v0 = 3402248170\n" );

    out.str("");
	compileAndRun( rctx, _Show(_Reg("$v0"), _EmptyArg, Ast::Fmt_SDec) );
    CHECK( out.str() == "$v0 = -892719126\n");

    out.str("");
	compileAndRun( rctx, _Show(_Reg("$v0"), _EmptyArg, Ast::Fmt_Hex) );
    CHECK( out.str() == "$v0 = 0xcaca2fea\n" );

    out.str("");
	compileAndRun( rctx, _Show(_Reg("$v0"), _EmptyArg, Ast::Fmt_Bin) );
    CHECK( out.str() == "$v0 = 0b11001010110010100010111111101010\n" );

    reg.t0 = 'H';
    out.str("");
	compileAndRun( rctx, _Show(_Reg("$t0"), _EmptyArg, Ast::Fmt_Ascii) );
    CHECK( out.str() == "$t0 = H\n" );

    // #show 0xcaca2fea
    out.str("");
	compileAndRun( rctx, _Show(_Hex("0xcaca2fea"), _EmptyArg, Ast::Fmt_Auto) );
    CHECK( out.str() == "3402248170\n");

    // #show "Hello World"
    out.str("");
	compileAndRun( rctx, _Show(_StrLiteral("Hello World"), _EmptyArg, Ast::Fmt_Auto) );
    CHECK( out.str() == "Hello World\n");

    // #show "Hello World" unsigned decimal
    out.str("");
	compileAndRun( rctx, _Show(_StrLiteral("Hello World"), _EmptyArg, Ast::Fmt_UDec) );
    CHECK( out.str() == "Hello World\n");

    // #show [$t0, $t1, $t2] hex
    reg.t0 = 0xdeadbeef;
    reg.t1 = 0x00dec0de;
    reg.t2 = 0x0badface;
    out.str("");
	compileAndRun( rctx, _Show(_ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") }), _StrLiteral("\n"), Ast::Fmt_Hex) );
    CHECK( out.str() == "$t0 = 0xdeadbeef\n$t1 = 0x00dec0de\n$t2 = 0x0badface\n");

    // #show [$t0, $t1, $t2] hex sep = ","
    reg.t0 = 0xdeadbeef;
    reg.t1 = 0x00dec0de;
    reg.t2 = 0x0badface;
    out.str("");
	compileAndRun( rctx, _Show(_ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") }), _StrLiteral(","), Ast::Fmt_Hex) );
    CHECK( out.str() == "$t0 = 0xdeadbeef,$t1 = 0x00dec0de,$t2 = 0x0badface\n");

    // #show [$t0, $t1, $t2] hex sep = 0x23 ('#')
    reg.t0 = 0xdeadbeef;
    reg.t1 = 0x00dec0de;
    reg.t2 = 0x0badface;
    out.str("");
	compileAndRun( rctx, _Show(_ArgList({ _Reg("$t0"), _Reg("$t1"), _Reg("$t2") }), _Hex("0x23"), Ast::Fmt_Hex) );
    CHECK( out.str() == "$t0 = 0xdeadbeef#$t1 = 0x00dec0de#$t2 = 0x0badface\n");

    Mips32::MemoryMap mmap(0x1000, 0xf000, 256, 256);
    Mips32::MemoryManager mm(mmap);
    uint32_t *pword = reinterpret_cast<uint32_t *>(mm.getMem());

    rctx.mm = &mm;
    
    // #show 4($s0)
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                    _Show(_ArgList({ _MemRef(_EmptySS, _EmptyArg, _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                          _EmptyArg,   
                          Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "word(0x00001004) = 0x0badface\n");

    // #show byte 4($s0)
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                    _Show(_ArgList({ _MemRef(_Byte, _EmptyArg, _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                          _EmptyArg,  
                          Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "byte(0x00001004) = 0x0b\n");

    // #show hword 4($s0)
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun(  rctx, 
                    _Show(_ArgList({ _MemRef(_HWord, _EmptyArg, _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                    _EmptyArg,
                    Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "hword(0x00001004) = 0x0bad\n");

    // #show word 4($s0)
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun(  rctx, 
                    _Show(_ArgList({ _MemRef(_Word, _EmptyArg, _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                          _EmptyArg,  
                          Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "word(0x00001004) = 0x0badface\n");

    // #show byte 4($s0):8
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    pword[2] = 0x11223344;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                   _Show(_ArgList({ _MemRef(_Byte, _Dec("8"), _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                                    _EmptyArg,
                                    Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "byte(0x00001004):8 = 0x0b 0xad 0xfa 0xce 0x11 0x22 0x33 0x44\n");

    // #show hword 4($s0):4
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    pword[2] = 0x11223344;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                   _Show(_ArgList({ _MemRef(_HWord, _Dec("4"), _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                                    _EmptyArg,
                                    Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "hword(0x00001004):4 = 0x0bad 0xface 0x1122 0x3344\n");

    // #show word 4($s0):2
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    pword[2] = 0x11223344;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                   _Show(_ArgList({ _MemRef(_Word, _Dec("2"), _BaseOffset(_Dec("4"), _Reg("$s0"))) }),
                                    _EmptyArg,
                                    Ast::Fmt_Hex) 
                );
    CHECK( out.str() == "word(0x00001004):2 = 0x0badface 0x11223344\n");

    // #show word (0x1004):2
    reg.s0 = 0x1000;
    pword[1] = 0x0badface;
    pword[2] = 0x11223344;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                   _Show(_ArgList({ _MemRef(_Word, _Dec("2"),
                                               _BaseOffset(_Dec("0"), _Hex("0x1004")))
                                    }),
                         _EmptyArg, 
                         Ast::Fmt_Hex) 
                );
   CHECK( out.str() == "word(0x00001004):2 = 0x0badface 0x11223344\n");

    // #show word ($v0):2 hex
    reg.s0 = 0x1004;
    pword[1] = 0x0badface;
    pword[2] = 0x11223344;
    out.str("");
    node_pool.setCurrLinenum(__LINE__);
	compileAndRun( rctx, 
                   _Show(_ArgList({ _MemRef(_Word, _Dec("2"),
                                               _BaseOffset(_Dec("0"), _Reg("$s0")))
                                     }),
                         _EmptyArg,   
                         Ast::Fmt_Hex) 
                );
   CHECK( out.str() == "word(0x00001004):2 = 0x0badface 0x11223344\n");
}

TEST_CASE("Mips32 assembler: set command")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    Mips32::RuntimeContext rctx;
    Mips32Regs reg(rctx);

     // #set $v0 = 10
    reg.v0 = 0xcaca2fea;
    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, _Set(_Reg("$v0"), _Dec("10")) );
    CHECK( reg.v0 == 10 );
    
    // #set $v0 = 0xdec0de
    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, _Set(_Reg("$v0"), _Hex("0xdec0de")) );
    CHECK( reg.v0 == 0xdec0de );

    // #set $v0 = -125
    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, _Set(_Reg("$v0"), _Dec("-125")) );
    CHECK( static_cast<int32_t>(reg.v0) == -125 );

    // #set $v0 = 0b11001101
    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, _Set(_Reg("$v0"), _Bin("0b11001101")) );
    CHECK( reg.v0 == 0b11001101 );

    // #set $v0 = $t0
    reg.t0 = 0xbadface;
    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, _Set(_Reg("$v0"), _Reg("$t0")) );
    CHECK( reg.v0 == 0xbadface );

    Mips32::MemoryMap mmap(0x1000, 0xf000, 256, 256);
    Mips32::MemoryManager mm(mmap);
    uint32_t *pword = reinterpret_cast<uint32_t *>(mm.getMem());

    rctx.mm = &mm;

    // #set $v0 = 4($s0)
    reg.s0 = 0x1000;
    reg.v0 = 0xcacafea;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_Reg("$v0"), 
                         _MemRef(_EmptySS, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( reg.v0 == 0x11223344 );

    // #set $v0 = hword 4($s0)
    reg.s0 = 0x1000;
    reg.v0 = 0xcacafea;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_Reg("$v0"), 
                         _MemRef(_HWord, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( reg.v0 == 0x00001122 );


    // #set $v0 = hword 6($s0)
    reg.s0 = 0x1000;
    reg.v0 = 0xdeadbeef;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_Reg("$v0"), 
                         _MemRef(_HWord, _EmptyArg,
                                  _BaseOffset(_Dec("6"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( reg.v0 == 0x00003344 );

    // #set $v0 = byte 5($s0)
    reg.s0 = 0x1000;
    reg.v0 = 0xdeadbeef;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_Reg("$v0"), 
                         _MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("5"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( reg.v0 == 0x000022 );

    // #set 4($s0) = 0xbadface
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_EmptySS, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))),
                   _Hex("0xbadface"))
                );
    CHECK( pword[1] == 0xbadface );

    // #set byte 5($s0) = 0x11223344
    pword[1] = 0xaabbccdd;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("5"), _Reg("$s0"))),
                   _Hex("0x11223344"))
                 );
    CHECK( pword[1] == 0xaa44ccdd );

    // #set hword 6($s0) = 0x11223344
    pword[1] = 0xaabbccdd;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_HWord, _EmptyArg,
                                  _BaseOffset(_Dec("6"), _Reg("$s0"))),
                   _Hex("0x11223344"))
                );
    CHECK( pword[1] == 0xaabb3344 );

    // #set word 4($s0) = 0x11223344
    pword[1] = 0xaabbccdd;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_Word, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))),
                   _Hex("0x11223344"))
                );
    CHECK( pword[1] == 0x11223344 );

    // #set byte 4($s0) = [10, 0xaa, 'H', -115, 0x11001101]
    pword[1] = 0xaabbccdd;
    pword[2] = 0xaabbccdd;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))),
                        _ArgList({_Dec("10"), _Hex("0xaa"), _Char("H"),
                                   _Dec("-115"), _Bin("0b11001101")})
                        )
                );
    CHECK( pword[1] == 0x0aaa488d );
    CHECK( pword[2] == 0xcdbbccdd );

    // #set byte 4($s0) = "Hello World"
    pword[1] = 0xaabbccdd;
    pword[2] = 0xaabbccdd;
    pword[3] = 0x00000000;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))),
                        _StrLiteral("Hello World"))
                );
    CHECK( pword[1] == 0x48656c6c );
    CHECK( pword[2] == 0x6f20576f );
    CHECK( pword[3] == 0x726c6400 );

    // #set byte 4($s0) = ["Hello World", 0xaa, 0xbb]
    pword[1] = 0xaabbccdd;
    pword[2] = 0xaabbccdd;
    pword[3] = 0x00000000;
    pword[4] = 0x11223344;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))),
                        _ArgList({ _StrLiteral("Hello World"),
                                    _Hex("0xaa"), _Hex("0xbb") 
                                  })
                        )
                );
    CHECK( pword[1] == 0x48656c6c );
    CHECK( pword[2] == 0x6f20576f );
    CHECK( pword[3] == 0x726c64aa );
    CHECK( pword[4] == 0xbb223344 );

    // #set byte 4($s0):10 = ["Hello World", 0xaa, 0xbb]
    pword[1] = 0xaabbccdd;
    pword[2] = 0xaabbccdd;
    pword[3] = 0x0badface;
    pword[4] = 0x11223344;
    reg.s0 = 0x1000;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx,
                   _Set(_MemRef(_Byte, _Dec("10"),
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))),
                        _ArgList({ _StrLiteral("Hello World"),
                                    _Hex("0xaa"), _Hex("0xbb") 
                                  })
                        ),
                    ErrorCode::Ok,
                    false
                );
    CHECK( pword[1] == 0xaabbccdd );
    CHECK( pword[2] == 0xaabbccdd );
    CHECK( pword[3] == 0x0badface );
    CHECK( pword[4] == 0x11223344 );

    // #set 0($s0) = 4($s0)
    reg.s0 = 0x1000;
    pword[0] = 0xaabbccdd;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_MemRef(_EmptySS, _EmptyArg,
                                  _BaseOffset(_Dec("0"), _Reg("$s0"))
                                 ), 
                         _MemRef(_EmptySS, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))
                                 )
                    ),
                    ErrorCode::Ok,
                    false
                );
    CHECK( pword[0] == 0xaabbccdd );

    // #set 0($s0) = hword 4($s0)
    reg.s0 = 0x1000;
    pword[0] = 0x0cacafea;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_MemRef(_EmptySS, _EmptyArg,
                                  _BaseOffset(_Dec("0"), _Reg("$s0"))
                                 ),
                         _MemRef(_HWord, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( pword[0] == 0x1122afea );

    // #set hword 2($s0) = hword 4($s0)
    reg.s0 = 0x1000;
    pword[0] = 0x0cacafea;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_MemRef(_HWord, _EmptyArg,
                                  _BaseOffset(_Dec("2"), _Reg("$s0"))
                                 ),
                         _MemRef(_HWord, _EmptyArg,
                                  _BaseOffset(_Dec("4"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( pword[0] == 0x0cac1122 );

    // #set byte 2($s0) = byte 5($s0)
    reg.s0 = 0x1000;
    pword[0] = 0xdeadbeef;
    pword[1] = 0x11223344;

    node_pool.setCurrLinenum(__LINE__);
    compileAndRun( rctx, 
                   _Set(_MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("2"), _Reg("$s0"))
                                 ), 
                         _MemRef(_Byte, _EmptyArg,
                                  _BaseOffset(_Dec("5"), _Reg("$s0"))
                                 )
                    )
                );
    CHECK( pword[0] == 0xdead22ef );
}

TEST_CASE("Data Definition 1")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    
    Ast::AsmProgram *prg = 
        _AsmPrg({
            _SectionData,
            _ByteData( _DataArgList({
                            _StrLiteralDataArg("Hello World"),
                            _HexDataArg("0xde"), _HexDataArg("0xad"),
                            _HexDataArg("0xbe"), _BinDataArg("0b11101111"),
                            _FillDataArg(_CharDataArg("*"), _DecDataArg("13")),
                            _DecDataArg("10"), _DecDataArg("0")
                        })
                    ),
            _WordData( _DataArgList({
                            _FillDataArg(_HexDataArg("0x0"), _DecDataArg("10")),
                            _HexDataArg("0xdeadbeef"), _HexDataArg("0x00dec0de"),
                            _StrLiteralDataArg("Hello")
                        })
                    ),
            _ByteData( _DataArgList({ _HexDataArg("0xde") }) ),
            _HWordData( _DataArgList({
                            _HexDataArg("0xdead"),  _HexDataArg("0xbeef"),
                            _StrLiteralDataArg("Hello"), _HexDataArg("0x00de"),
                            _HexDataArg("0xc0de"),
                            _FillDataArg(_HexDataArg("0xaabb"), _DecDataArg("20")),
                        })
                    )
        });

    uint8_t bytes[] = {
        // Byte data
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd',
        0xde, 0xad, 0xbe, 0xef, '*', '*', '*', '*', '*', '*',
        '*', '*', '*', '*', '*', '*', '*', 10, 0, 0x00, 0x00,
        
        // Word data
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xde, 0xad, 0xbe, 0xef, 0x00, 0xde, 0xc0, 0xde, 
        0x00, 0x00, 0x00, 'H', 0x00, 0x00, 0x00, 'e',
        0x00, 0x00, 0x00, 'l',  0x00, 0x00, 0x00, 'l', 
        0x00, 0x00, 0x00, 'o',
        
        // Byte data
        0xde, 0x00, 

        // Half word data
        0xde, 0xad, 0xbe, 0xef, 0x00, 'H', 0x00, 'e', 0x00, 'l',
        0x00, 'l', 0x00, 'o', 0x00, 0xde, 0xc0, 0xde,
        0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb,
        0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb,
        0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb,
        0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa, 0xbb,
    };
    size_t BYTE_COUNT = sizeof(bytes)/sizeof(bytes[0]);
    Ast::CompileState cst(0x0, 0x1000);

    bool success = true;
    try {
        prg->resolveLabels(cst);
        REQUIRE(cst.data_size == 160);

        std::vector<Mips32::VmOperation> actions;
        prg->compile(cst, actions);

        std::vector<uint8_t> data(cst.data_size);
        prg->gdata.copyTo(data.data());

        CHECK( std::equal(bytes, bytes + BYTE_COUNT, data.begin()) );
    }
    catch (EAsm::Error& err)
    {
        std::cerr << err;
        success = false;
    }

    CHECK( success );
}

TEST_CASE("Data Definition 2")
{
    Ast::NodePool node_pool(__FILE__, __LINE__);
    
    Ast::AsmProgram *prg = 
        _AsmPrg({
            _SectionData,
            _ByteData( _DataArgList({
                            _StrLiteralDataArg("Sorted array: "),
                            _DecDataArg("0")
                        })
                    ),
            _WordData( _DataArgList({
                            _DecDataArg("74"),
                            _DecDataArg("34"),
                            _DecDataArg("227")
                        })
                    )
        });

    uint8_t bytes[] = {
        // Byte data
        'S', 'o', 'r', 't', 'e', 'd', ' ', 'a', 'r', 'r', 'a', 'y', ':', ' ',
        0x00, 0x00,
        
        // Word data
        0x00, 0x00, 0x00, 0x4A, // 74
        0x00, 0x00, 0x00, 0x22, // 34
        0x00, 0x00, 0x00, 0xE3, // 227
    };
    size_t BYTE_COUNT = sizeof(bytes)/sizeof(bytes[0]);
    Ast::CompileState cst(0x0, 0x1000);

    bool success = true;
    try {
        prg->resolveLabels(cst);
        REQUIRE(cst.data_size == 28);

        std::vector<Mips32::VmOperation> actions;
        prg->compile(cst, actions);

        std::vector<uint8_t> data(cst.data_size);
        prg->gdata.copyTo(data.data());

        CHECK( std::equal(bytes, bytes + BYTE_COUNT, data.begin()) );
    }
    catch (EAsm::Error& err)
    {
        std::cerr << err;
        success = false;
    }

    CHECK( success );
}