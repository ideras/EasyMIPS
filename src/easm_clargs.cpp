#include <iostream>
#include "colorizer.h"
#include "easm_clargs.h"

namespace EAsm
{
    void usage(const char *prog_name)
    {
        std::cerr << boldText("Usage: ") << cboldText(fcolor::blue, prog_name) 
                  << " [options]\n"
                  << "Where options can be:\n"
                  << "  " << colorText(fcolor::magenta, "--run")
                  << " " << colorText(fcolor::yellow, "<file_1>")
                  << " " << colorText(fcolor::yellow, "<file_2>")
                  << " ... " 
                  << colorText(fcolor::yellow, "<file_N>\n")
                  << "    Run the assembler program from files file_1, file_2, ..., file_N\n"
                  << "  " << colorText(fcolor::magenta, "--entry") << " "
                  << colorText(fcolor::yellow, "<function>\n")
                  << "    Start the program at the specified function\n"
                  << "  " << colorText(fcolor::magenta, "--interactive")
                  << " or "
                  << colorText(fcolor::magenta, "-i")
                  << "\n    Start EasyMIPS in interactive mode\n"
                  << "  " << colorText(fcolor::magenta, "--sc-handler ")
                  << colorText(fcolor::yellow, "<library>\n")
                  << "    Specifies a library to handle syscalls\n"
                  << "  " << colorText(fcolor::magenta, "--gbl-size ")
                  << colorText(fcolor::yellow, "<size>\n")
                  << "    Defines the size in bytes of the global memory\n"
                  << "  " << colorText(fcolor::magenta, "--stk-size ")
                  << colorText(fcolor::yellow, "<size>\n")
                  << "    Defines the size in bytes of the stack\n"
                  << "  " << colorText(fcolor::magenta, "--inst-count\n")
                  << "    Shows the number of instruction used when running a program\n"
                  << "  " << colorText(fcolor::magenta, "--exec-time\n")
                  << "    Shows the execution time used when running a program\n"
                  << "  " << colorText(fcolor::magenta, "--help\n")
                  << "    Shows this information\n";
    }

    int parseClArgs(int argc, char *argv[], ClArgs& args)
    {
        if (argc <= 1) return 0;
        
        const char *prg = argv[0];

        ++argv, --argc;
        args.count = argc;
        
        size_t i = 0;
        while (i < argc)
        {
            if (strcmp(argv[i], "--run") == 0)
            {
                i++;
                while (i < argc)
                {
                    if (argv[i][0] == '-') break;

                    args.input_files.emplace_back(argv[i]);
                    i++;
                }
                if (args.input_files.empty())
                {
                    std::cerr << "Missing file name for "
                              << cboldText(fcolor::red, "--run")
                              << " option\n";

                    usage(prg);
                    return 2;
                }
                continue;
            }
            if (strcmp(argv[i], "--inst-count") == 0)
                args.show_inst_count = true;
            else if (strcmp(argv[i], "--exec-time") == 0)
                args.show_exec_time = true;
            else if (strcmp(argv[i], "--interactive") == 0
                     || strcmp(argv[i], "-i") == 0)
                args.interactive = true;
            else if (strcmp(argv[i], "--help") == 0)
                args.show_help = true;
            else if (strcmp(argv[i], "--stk-size") == 0)
            {
                i++;
                if (i >= argc)
                {
                    std::cerr << "Missing size argument in option "
                              << cboldText(fcolor::red, "--stk-size")
                              << '\n';
                    usage(prg);
                    return 2;
                }

                char *endptr;
                args.stk_size = std::strtoul(argv[i], &endptr, 10);

                if (*endptr != '\0')
                {
                    std::cerr << "Invalid size argument in option "
                              << cboldText(fcolor::red, "--stk-size")
                              << '\n';
                    usage(prg);
                    return 2;
                }
            }
            else if (strcmp(argv[i], "--gbl-size") == 0)
            {
                i++;
                if (i >= argc)
                {
                    std::cerr << "Missing size argument in option "
                              << cboldText(fcolor::red, "--gbl-size")
                              << '\n';
                    usage(prg);
                    return 2;
                }

                char *endptr;
                args.gbl_size = std::strtoul(argv[i], &endptr, 10);

                if (*endptr != '\0')
                {
                    std::cerr << "Invalid size argument in option "
                              << cboldText(fcolor::red, "--gbl-size")
                              << '\n';
                    usage(prg);
                    return 2;
                }
            }
            else if (strcmp(argv[i], "--vga-plugin") == 0)
            {
                i++;
                if (i >= argc)
                {
                    std::cerr << "Missing plugin file for "
                            << cboldText(fcolor::red, "--vga-plugin")
                            << " option\n";
                    usage(prg);
                    return 2;
                }
                args.vga_plugin_lib = argv[i];
            }
            else if (strcmp(argv[i], "--sc-handler") == 0)
            {
                i++;
                if (i >= argc)
                {
                    std::cerr << "Missing plugin file for "
                            << cboldText(fcolor::red, "--sc-handler")
                            << " option\n";
                    usage(prg);
                    return 2;
                }
                args.sc_plugin_lib = argv[i];
            }
            else if (strcmp(argv[i], "--entry") == 0)
            {
                i++;
                if (i == argc)
                { 
                    std::cerr << "Missing function name for "
                            << cboldText(fcolor::red, "--entry")
                            << " option\n";
                    usage(prg);
                    return 2;
                }
                args.entry_label = argv[i];
            }
            else
            {
                std::cerr << "Invalid option " << cboldText(fcolor::red, argv[i]) << '\n';
                usage(prg);
                return 2;
            }
            i++;
        }

        return 0;
    }    
} // namespace EAsm
