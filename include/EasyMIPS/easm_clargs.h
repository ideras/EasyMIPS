#ifndef _EASM_CLARGS_H_
#define _EASM_CLARGS_H_

#include <cstddef>
#include <string>
#include <vector>

namespace EAsm
{
    struct ClArgs
    {
        ClArgs()
        : count(0),
          show_inst_count(false),
          show_exec_time(false),
          interactive(false),
          show_help(false),
          gbl_size(0),
          stk_size(0),
          entry_label(),
          vga_plugin_lib(),
          input_files()
        {
        }

        size_t count;
        bool show_inst_count;
        bool show_exec_time;
        bool interactive;
        bool show_help;
        size_t gbl_size;
        size_t stk_size;
        std::string entry_label;
        std::string vga_plugin_lib;
        std::string sc_plugin_lib;
        std::vector<std::string> input_files;
    };

    void usage(const char *prog_name);
    int parseClArgs(int argc, char *argv[], ClArgs& args);
    
} // namespace EAsm

#endif
