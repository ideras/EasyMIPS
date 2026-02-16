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
#include "mips32_vm.h"
#include "rang.hpp"

namespace Ast = Mips32::Ast;
namespace Asm = Mips32::Assembler;

namespace fs = std::filesystem;

std::string inc_folder;

const VirtualAddr gbl_start = 0x10000000;
const VirtualAddr stk_end = 0x7fffeffc;
const size_t gbl_size = 1024;
const size_t stk_size = 1024;
const VirtualAddr stk_start = stk_end - stk_size;

const Mips32::MemoryMap mmap(gbl_start, stk_start, gbl_size, stk_size);

std::string readAllFile(const std::string& file_path)
{
    std::ifstream in(file_path, std::ios::in);

    if (!in.is_open())
        throw std::runtime_error("Cannot open file: " + file_path + "\n");

    in.seekg(0, std::ios::end);
    const size_t fsize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> file_data(fsize);
    in.read(file_data.data(), fsize);

    if (in.gcount() != fsize)
        throw std::runtime_error("Cannot read file: " + file_path + "\n");

    return std::string(file_data.data(), file_data.size());
}

void testFile(const std::string& src_file, const std::string& exp_file)
{
    std::string file_content;
    try
    {
        file_content = readAllFile(exp_file);
    }
    catch (std::runtime_error& err)
    {
        std::cout << err.what();
        REQUIRE(false);
    }
    
    std::ostringstream oss;
    Mips32::VirtualMachine vm(mmap, oss);

    rang::setControlMode(rang::control::Off);
    int res = vm.exec({src_file});
    rang::setControlMode(rang::control::Auto);

    if (res != 0)
        std::cerr << vm.lastError();

    REQUIRE( res == 0 );

    INFO(src_file);
    CHECK(oss.str() == file_content);
}

void checkOverflow(const std::string& src_file)
{
    std::ostringstream oss;

    try
    {
        Mips32::VirtualMachine vm(mmap);

        int res = vm.exec({src_file});
        REQUIRE( res != 0 );
        REQUIRE( !vm.lastError().empty() );

        oss << vm.lastError();
        CHECK(oss.str().find("overflow") != std::string::npos);
    }
    catch (EAsm::Error& err)
    {
        std::cerr << err;
        CHECK(false);
    }
}

TEST_CASE("MIPS32 virtual machine single file test")
{
    fs::path srcfolder_path(inc_folder);
    fs::path expfolder_path(inc_folder);

    srcfolder_path /= "asm";
    expfolder_path /= "expected";

    REQUIRE(fs::exists(srcfolder_path));
    REQUIRE(fs::exists(expfolder_path));

    REQUIRE(fs::is_directory(srcfolder_path));
    REQUIRE(fs::is_directory(expfolder_path));

    for (const auto& entry : fs::directory_iterator(srcfolder_path))
    {
        fs::path sfilepath = entry.path();
        std::string src_filename = sfilepath.filename().string();

        if (src_filename.compare(0, 3, "ovf") == 0)
        {
            std::cout << "Testing overflow: " << colorText(fcolor::yellow, sfilepath.string()) << '\n';
            checkOverflow(sfilepath.string());
            continue;
        }

        INFO(sfilepath.string());
        if (fs::is_regular_file(entry.status()))
        {
            fs::path efile = sfilepath.filename();

            efile.replace_extension(".txt");
            fs::path efilepath(expfolder_path / efile);

            std::cout << "Testing file " << cboldText(rang::fg::magenta, sfilepath.string()) << '\n';
            testFile(sfilepath.string(), efilepath.string());
        }
    }
}

std::string vmRun(const std::vector<std::string>& files)
{
    std::ostringstream oss;
    Mips32::VirtualMachine vm(mmap, oss);

    rang::setControlMode(rang::control::Off);
    int res = vm.exec(files, "start");
    rang::setControlMode(rang::control::Auto);
    
    if (res != 0)
        std::cerr << vm.lastError();

    REQUIRE( res == 0 );
    return oss.str();
}

TEST_CASE("MIPS32 virtual machine multiple file: test 1")
{
    fs::path ifolder_path(inc_folder);
    fs::path srcfolder_path(ifolder_path / "asm" / "multiple1");
    fs::path expfolder_path(ifolder_path / "expected");

    REQUIRE(fs::exists(srcfolder_path));
    REQUIRE(fs::exists(expfolder_path));

    REQUIRE(fs::is_directory(srcfolder_path));
    REQUIRE(fs::is_directory(expfolder_path));

    const char *src_filenames[] = {"start.asm", "file1.asm", "file2.asm", "file3.asm"};
    const size_t SRC_COUNT = sizeof(src_filenames) / sizeof(src_filenames[0]);
    std::vector<std::string> files;

    for (int i = 0; i < SRC_COUNT; i++)
    {
        fs::path srcfile_path(srcfolder_path / src_filenames[i]);
        REQUIRE(fs::exists(srcfile_path));

        files.push_back(srcfile_path.string());
    }

    fs::path expfile_path(expfolder_path / "multiple1.txt");
    REQUIRE(fs::exists(expfile_path));

    std::string file_content;
    
    REQUIRE_NOTHROW( file_content = readAllFile(expfile_path.string()) );

    std::string output = vmRun(files);

    CHECK( output == file_content );
}

TEST_CASE("MIPS32 virtual machine multiple file: test 2")
{
    fs::path ifolder_path(inc_folder);
    fs::path srcfolder_path(ifolder_path / "asm" / "multiple2");
    fs::path expfolder_path(ifolder_path / "expected");

    REQUIRE( fs::exists(srcfolder_path) );
    REQUIRE( fs::exists(expfolder_path) );

    REQUIRE( fs::is_directory(srcfolder_path) );
    REQUIRE( fs::is_directory(expfolder_path) );

    const char *src_filenames[] = {"start.asm", "file1.asm", "file2.asm"};
    const size_t SRC_COUNT = sizeof(src_filenames) / sizeof(src_filenames[0]);
    std::vector<std::string> files;

    for (int i = 0; i < SRC_COUNT; i++)
    {
        fs::path srcfile_path(srcfolder_path / src_filenames[i]);
        REQUIRE( fs::exists(srcfile_path) );

        files.push_back(srcfile_path.string());
    }

    fs::path expfile_path(expfolder_path / "multiple2.txt");
    REQUIRE(fs::exists(expfile_path));

    std::string file_content;
    
    REQUIRE_NOTHROW( file_content = readAllFile(expfile_path.string()) );

    std::string output = vmRun(files);
    
    CHECK(output == file_content);
}

int main(int argc, char **argv)
{
    doctest::Context context;

    char **p = argv + 1;
    for (int i = 0; i < argc - 1; i++)
    {
        if (strncmp(p[i], "--include", 9) == 0)
        {
            std::string opt = p[i];
            size_t pos = opt.find("=");
            if (pos == std::string::npos)
            {
                std::cerr << "Error in include argument. "
                          << "Usage " << cboldText(rang::fg::cyan, argv[0])
                          << " " << boldText("--include")
                          << "=" << colorText(rang::fg::magenta, "<folder>") << '\n';
                return 1;
            }
            inc_folder = opt.substr(pos + 1);
            break;
        }
    }
    if (inc_folder.empty())
        inc_folder = ".";

    std::cout << "Using include directory " << cboldText(rang::fg::magenta, inc_folder) << '\n';

    context.applyCommandLine(argc, argv);

    return context.run();
}