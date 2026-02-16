#include <chrono>
#include "mips32_vm.h"
#include "mips32_lexer.h"
#include "mips32_parser.h"
#include "easm_error.h"
#include "num_convert.h"
#include "colorizer.h"

using sys_clk = std::chrono::system_clock;

namespace Mips32
{
    void VirtualMachine::init()
    {
        mem_mgr = std::make_unique<MemoryManager>(mem_map);
        rt_ctx = std::make_unique<RuntimeContext>(mem_mgr.get(), out);
        rt_ctx->ext_syscall_handler = ext_sc_handler;
    }

    int VirtualMachine::processCliInput(const std::string& input)
    {
        std::stringstream in(input);
    
        Mips32::Ast::NodePool node_pool;
        Mips32::Lexer lexer(in);
        Mips32::Parser parser(lexer, node_pool);
        
        node_pool.setCurrFilename("");

        try {
            Mips32::Ast::AsmProgram *n_prg = parser.parse();

            if (n_prg->asm_entries.size() > 1)
            {

                last_error = EAsm::Error("Only one instruction is allowed in interactive mode. "
                                         "Input has ",
                                         colorText(fcolor::yellow, n_prg->asm_entries.size()),
                                         " instructions\n");
                return 1;
            }
            Ast::AsmEntry *n_entry = n_prg->asm_entries[0];

            if (n_entry->isA(Ast::ExecCmd_kind))
            {
                Ast::ExecCmd *n_cmd = Ast::node_cast<Ast::ExecCmd>(n_entry);
                std::string filename = Ast::node_cast<Ast::String>(n_cmd->n_str)->s_val;

                return exec({filename}, "");    
            }
            else if (n_entry->isA(Ast::ResetCmd_kind))
            {
                init();
            }
            else if (n_entry->isA(Ast::Inst_kind)
                     || n_entry->isA(Ast::Cmd_kind))
            {
                Ast::CompileState cst(0x400000, 0x10000000);
                VmOperationVector action_v;
                n_prg->compile(cst, action_v);

                return exec(action_v, 0x400000, 0);
            }
            else
            {
                last_error = EAsm::Error(colorText(fcolor::red, n_entry->toString()),
                                         " is not allowed in interactive mode\n");
                return 1;
            }
        }
        catch (EAsm::Error& err)
        {
            last_error = EAsm::Error(std::move(err));
            return 1;
        }

        return 0;
    }

    int VirtualMachine::exec(const std::vector<std::string>& input_files,
                             const std::string& entry_label)
    {
        Ast::NodePool node_pool;
        std::vector<Ast::AsmProgram *> prg_v;
        Ast::CompileState cst(0x400000, 0x10000000);

        for (const auto& file : input_files)
        {
            std::ifstream in(file, std::ios::in);
            if (!in.is_open())
            {
                last_error = EAsm::Error("Cannot open file ", cboldText(fcolor::red, file), '\n');
                return 1;
            }

            node_pool.setCurrFilename(file.c_str());
            node_pool.setCurrLinenum(1);

            Lexer lexer(in);
            Parser parser(lexer, node_pool);
            
            try
            {
                cst.vd_addr = ((cst.vd_addr + 3) / 4) * 4;
                Ast::AsmProgram *prg = parser.parse();
                prg->resolveLabels(cst);
                prg_v.push_back(prg);
            }
            catch (EAsm::Error& err)
            {
                last_error = EAsm::Error(std::move(err));
                return 2;
            }
        }

        Ast::AsmEntry *entry_point = nullptr;
        bool needs_entry_point = false;

        if (!entry_label.empty())
        {
            auto it = cst.global_lbl.find(entry_label);

            if (it != cst.global_lbl.end())
                entry_point = it->second;
            else
                needs_entry_point = true;
        }

        VmOperationVector action_v;
        Ast::AsmEntryVector asm_entry_v;

        for (const auto prg : prg_v)
        {
            if (needs_entry_point)
            {
                auto it = prg->local_lbl.find(entry_label);

                if (it != prg->local_lbl.end())
                    asm_entry_v.push_back(it->second);
            }
            try
            {
                prg->compile(cst, action_v);

                auto it = mem_mgr->memIter<uint8_t>(prg->virtual_addr);
                for (auto b : prg->gdata.getData())
                    *it++ = b;
            }
            catch (EAsm::Error &err)
            {
                last_error = EAsm::Error(std::move(err));
                return 2;
            }
        }

        if (needs_entry_point)
        {
            if (asm_entry_v.size() == 0)
            {
                last_error = EAsm::Error("Entry point ", cboldText(fcolor::red, entry_label),
                                         " hasn't been defined neither public nor local label\n");

                return 2;
            }
            if (asm_entry_v.size() > 1)
            {
                EAsm::ErrorInfoVector einfov(asm_entry_v.size());

                for (auto ent : asm_entry_v)
                {
                    einfov.push_back(EAsm::makeErrorInfo(
                        colorText(fcolor::green, ent->getFilename()), ":",
                        colorText(fcolor::yellow, ent->getLinenum()), '\n'));
                }

                last_error = EAsm::Error("Entry point ",
                                         cboldText(fcolor::red, entry_label),
                                         " has been defined mutiple times as local label in:\n",
                                         std::move(einfov),
                                         "To avoid this problem declare it as ",
                                         boldText(".global"), cboldText(fcolor::green, entry_label),
                                         " only once\n");

                return 2;
            }
            entry_point = asm_entry_v[0];
        }

        if (action_v.empty())
        {
            last_error = EAsm::Error(colorText(fcolor::yellow, "WARNING"),
                                     "Nothing to do. Program is empty\n");

            return 2;
        }

        VirtualAddr entry_addr = entry_point ? entry_point->virtual_addr : 0x400000;

        auto time1 = sys_clk::now();
        int res = exec(action_v, entry_addr, 0);
        auto time2 = sys_clk::now();

        auto d = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);
        exec_time_us = static_cast<size_t>(d.count());

        return res;
    }

    int VirtualMachine::exec(const VmOperationVector &action_v,
                             VirtualAddr entry_point,
                             VirtualAddr initial_ra)
    {
        rt_ctx->setPC(entry_point);
        rt_ctx->reg_file.setReg(RegIndex::Ra, initial_ra);

        VirtualAddr last_pc = 0x400000 + action_v.size() * 4;
        inst_count = 0;

        do
        {
            unsigned idx = (rt_ctx->getPC() - 0x400000) / 4;

            if (idx > action_v.size() - 1)
            {
                last_error = EAsm::Error("Runtime error: Invalid instruction address ",
                                         cboldText(fcolor::red, Cvt::hexVal(rt_ctx->getPC())),
                                         '\n');

                return 1;
            }
            VmOperation act = action_v[idx];
            rt_ctx->setPC(rt_ctx->getPC() + 4);

            if (act.task == nullptr)
            {
                last_error = EAsm::Error(act.src_info, "BUG in the machine, action is null :-(\n");
                return 3;
            }

            ErrorCode ecode = act.task(*rt_ctx);
            inst_count++;

            if (ecode == ErrorCode::Stop)
                break;

            if (ecode != ErrorCode::Ok)
            {
                if (rt_ctx->last_error.empty())
                    last_error = EAsm::errorCodeDesc(act.src_info, ecode);
                else
                    last_error = std::move(rt_ctx->last_error);

                return 2;
            }
        } while (rt_ctx->getPC() < last_pc);

        return 0;
    }

} // namespace Mips32