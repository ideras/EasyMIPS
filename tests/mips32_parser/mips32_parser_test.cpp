#define DOCTEST_CONFIG_IMPLEMENT

#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include "doctest.h"
#include "easm_error.h"
#include "mips32_parser.h"
#include "mips32_ast.h"
#include "mips32_assembler.h"

#define __AsmProgram ctx.AsmProgramCreate
#define __NodeList ctx.NodeListCreate
#define __ArgList ctx.ArgListCreate
#define __Inst  ctx.InstCreate
#define __Set   ctx.SetCmdCreate
#define __Show  ctx.ShowCmdCreate
#define __Stop  ctx.StopCmdCreate
#define __LStmt ctx.LabelledStmtCreate
#define __BaseOffset ctx.BaseOffsetCreate
#define __Ident ctx.IdentCreate
#define __Str ctx.StringCreate
#define __Empty ctx.EmptyCreate
#define __OutFmtHex ctx.OutFmtHexCreate
#define __Byte ctx.ByteSizeDirCreate
#define __HWord ctx.HWordSizeDirCreate
#define __Word ctx.WordSizeDirCreate
#define __Reg(r) ctx.RegCreate(r)
#define __Dec(v) ctx.DecConstCreate(v)
#define __Hex(v) ctx.HexConstCreate(v)
#define __Bin(v) ctx.BinConstCreate(v)
#define __Char(v) ctx.CharConstCreate(v)

namespace Ast = Mips32::Ast;
namespace Asm = Mips32::Assembler;

namespace fs = std::filesystem;

std::string inc_folder;

const std::string lbl_dup_code("li $t0,10\n"
                               "li $t1,20\n"
                               "loop1: add $t0,$t0,$t1\n"
                               "j loop1\n"
                               "loop1:\n"
                               "addi $a0,$t0,100\n"
                               "beq $a0,$zero,loop2");

std::string readAllFile(const std::string& file_path)
{
    std::ifstream in(file_path, std::ios::in);

    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path + "\n");
    }

    in.seekg(0, std::ios::end);
    const size_t fsize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> file_data(fsize);
    in.read(file_data.data(), fsize);
    if (in.gcount() != fsize) {
        throw std::runtime_error("Cannot open file: " + file_path + "\n");
    }

    return std::string(file_data.data(), file_data.size());
}

void testFile(const std::string& src_file, const std::string& exp_file)
{
    std::string file_content;
    try {
        file_content = readAllFile(exp_file);
    } catch (std::runtime_error& err) {
        std::cout << err.what();
        REQUIRE( false );
    }
    std::ifstream in(src_file, std::ios::in|std::ios::binary);

    REQUIRE(in.is_open());

    Ast::NodePool node_pool;
    Mips32::Lexer lexer(in);
    Mips32::Parser parser(lexer, node_pool);

    try {
        node_pool.setCurrFilename(src_file.c_str());
        Ast::Node *prg = parser.parse();
        REQUIRE( prg );
        CHECK( prg->toString() == file_content );
    } catch (EAsm::Error& err) {
        std::cerr << err;
        CHECK( false );
    }
}

TEST_CASE("Parser + AST") 
{
    fs::path srcfolder_path(inc_folder);
    fs::path expfolder_path(inc_folder);

    srcfolder_path /= "asm";
    expfolder_path /= "expected";

    REQUIRE( fs::exists(srcfolder_path) );
    REQUIRE( fs::exists(expfolder_path) );

    REQUIRE( fs::is_directory(srcfolder_path) );
    REQUIRE( fs::is_directory(expfolder_path) );

    SUBCASE("AST") {
        for (const auto& entry : fs::directory_iterator(srcfolder_path)) {
            fs::path sfilepath = entry.path();
            std::string src_file = sfilepath.string();

            INFO(src_file);
            if (fs::is_regular_file(entry.status())) {
                fs::path efilepath(expfolder_path / sfilepath.filename());
            
                std::cout << "Testing file " << cboldText(rang::fg::yellow, sfilepath.string()) << '\n';
                testFile(sfilepath.string(), efilepath.string());
            }
        }
    }

    SUBCASE("Line numbers in node") {
        fs::path filepath(srcfolder_path / "lines.asm");

        REQUIRE( fs::is_regular_file(filepath) );

        std::string src_file = filepath.string();
        std::ifstream in(src_file, std::ios::in|std::ios::binary);

        REQUIRE(in.is_open());

        Ast::NodePool node_pool;
        Mips32::Lexer lexer(in);
        Mips32::Parser parser(lexer, node_pool);

        try {
            node_pool.setCurrFilename(src_file.c_str());
            Ast::AsmProgram *prg = parser.parse();
            REQUIRE( prg );
            
            const Ast::AsmEntryVector& asm_entries = prg->asm_entries;

            int lines[] = {3, 4, 6, 8, 9, 10, 10, 10, 12, 14, 16};
            REQUIRE( asm_entries.size() == sizeof(lines)/sizeof(lines[0]) );
            for (int i = 0; i < asm_entries.size(); i++) {
                INFO("Instruction: " << asm_entries[i]->toString());
                CHECK( asm_entries[i]->getLinenum() == lines[i] );
            }            
        } catch (EAsm::Error& err) {
            std::cerr << err;
            REQUIRE( false );
        }
    }
}

int main(int argc, char** argv) 
{
    doctest::Context context;

    char **p = argv+1;
    for (int i = 0; i < argc -1; i++) {
        if (strncmp(p[i], "--include", 9) == 0) {
            std::string opt = p[i];
            size_t pos = opt.find("=");
            if (pos == std::string::npos) {
                std::cerr << "Error in include argument. "
                          << "Usage " << cboldText(rang::fg::cyan, argv[0])
                          << " " << boldText("--include") 
                          << "=" << colorText(rang::fg::magenta, "<folder>") << '\n';
                return 1;
            }
            inc_folder = opt.substr(pos + 1);
            std::cout << "Using include directory " << cboldText(rang::fg::magenta, inc_folder) << '\n';
            break;
        }
    }
    if (inc_folder.empty()) {
        inc_folder = ".";
    }
    context.applyCommandLine(argc, argv);

    return context.run();
}