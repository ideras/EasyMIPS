#include "mips32_parser.h"

#include <vector>

#include "mips32_ast.h"
#include "easm_error.h"

namespace Mips32
{
/*
input -> EOL* (Label EOL*)* asm_entry (EOL EOL* (Label EOL*)* asm_entry?)*

asm_entry -> asm_directive
asm_entry -> asm_inst
asm_entry -> command

asm_directive -> KwDotData
asm_directive -> KwDotText
asm_directive -> KwDotGlobal Ident
asm_directive -> KwDotByte data_arg_list
asm_directive -> KwDotHWord data_arg_list
asm_directive -> KwDotWord data_arg_list

data_arg_list -> data_arg (, data_arg)*

data_arg -> StrLiteral
data_arg -> constant
data_arg -> KwDotFill OpenPar constant , constant ClosePar

asm_inst -> IDENT (inst_args)?

inst_args -> argument (COMMA argument)*

command -> KWSHOW show_argument data_format?
command -> KWSET cmd_arg OPEQUAL set_rvalue
command -> KWEXEC STR_LITERAL
command -> KWRESET
command -> KWSTOP

show_argument -> cmd_arg
show_argument -> LBracket constant_list RBracket

constant_list -> constant_arg ( COMMA constant_arg )*

set_rvalue -> constant_arg
set_rvalue -> LBracket constant_list RBracket

cmd_single_arg -> argument
                 | mem_ref_arg

mem_ref_arg ->  KWBYTE base_offset_arg
              | KWHWORD base_offset_arg
              | KWWORD base_offset_arg
              | base_offset_arg

cmd_arg -> [ cmd_arg_list ]
cmd_arg -> cmd_single_arg

cmd_arg_list -> cmd_single_arg (COMMA cmd_sigle_arg)*

size_directive -> KwByte
size_directive -> KwHWord
size_directive -> KwWord

data_format -> KwHex
data_format -> KwDec
data_format -> KwSigned KwDecimal
data_format -> KwUnsigned KwDecimal
data_format -> KwBinary
data_format -> KwAscii

argument -> REGISTER
argument -> base_offset_arg
argument -> constant_arg

base_offset_arg -> constant LPAREN argument RPAREN
                 | LPAREN argument RPAREN

constant_arg -> constant
constant_arg -> IDENT
constant_arg -> KWHIHW LPAREN constant_arg RPAREN
constant_arg -> KWLOHW LPAREN constant_arg RPAREN

constant -> DEC_CONST
constant -> OP_MINUS DEC_CONST
constant -> HEX_CONST
constant -> BIN_CONST
constant -> CHAR_CONST
*/

using TokenList = std::initializer_list<unsigned>;

static TokenList firstOfStmt = {
    Token::Ident,
    Token::KwShow,
    Token::KwSet,
    Token::KwExec,
    Token::KwReset,
    Token::KwStop};

static TokenList firstOfCmd = {
    Token::KwShow,
    Token::KwSet,
    Token::KwExec,
    Token::KwReset,
    Token::KwStop};

static TokenList firstOfAsmDir = {
    Token::KwDotGlobal,
    Token::KwDotData,
    Token::KwDotText,
    Token::KwDotByte,
    Token::KwDotHWord,
    Token::KwDotWord,
};

static TokenList firstOfArg = {
    Token::RegName,
    Token::RegIndex,
    Token::DecConst,
    Token::HexConst,
    Token::BinConst,
    Token::CharLiteral,
    Token::Ident,
    Token::DotIdent,
    Token::DollarIdent,
    Token::KwHiHw,
    Token::KwLoHw,
};

class ParserHelper
{
    friend class Parser;

public:
    ParserHelper(Lexer &lexer, Ast::NodePool &ctx)
    : lexer(lexer), ctx(ctx)
    {}

private:

    template <typename... TArgs>
    bool tokenIs(TArgs... tks)
    {
        return ((curr_tk.token_id == tks) || ...);
    }

    bool tokenIs(const TokenList &tkl)
    {
        for (auto tk_id : tkl)
        {
            if (curr_tk.token_id == tk_id)
                return true;
        }
        return false;
    }

    void getNextToken()
    { curr_tk = lexer.getNextToken(); }

    void match(unsigned tk_id, const char *text = "")
    {
        if (curr_tk.token_id != tk_id)
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                              "Expected: ", cboldText(fcolor::cyan, text),
                              ", found ", cboldText(fcolor::red, curr_tk.text),
                              '\n');
        }
        getNextToken();
    }

    void skipEol()
    {
        while (tokenIs(Token::Eol))
            getNextToken();
    }

    Ast::AsmProgram *input()
    {
        skipEol();

        Ast::AsmEntryVector asm_entries;

        handleLabel(asm_entries);

        Ast::AsmEntry *n_entry = asmEntry();
        asm_entries.push_back(n_entry);

        while (tokenIs(Token::Eol))
        {
            skipEol();
            
            if (tokenIs(Token::Eof))
                break;

            handleLabel(asm_entries);

            if (tokenIs(Token::Eof))
            {
                asm_entries.push_back(ctx.EmptyStmtCreate());
                break;
            }

            Ast::AsmEntry *n_entry = asmEntry();
            asm_entries.push_back(n_entry);
        }

        if (!tokenIs(Token::Eof))
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                       cboldText(fcolor::red, curr_tk.text),
                       " is not a valid start of statement\n");
        }

        return ctx.AsmProgramCreate(asm_entries);
    }

    void handleLabel(Ast::AsmEntryVector &asm_entries)
    {
        while (tokenIs(Token::Label))
        {
            Ast::AsmEntry *n_lbl = ctx.LabelEntryCreate(curr_tk.text);
            n_lbl->setLinenum(curr_tk.line_num);

            asm_entries.push_back(n_lbl);
            getNextToken();
            skipEol();
        }
    }

    Ast::AsmEntry *asmEntry()
    {
        Ast::AsmEntry *n_entry = nullptr;

        if (tokenIs(firstOfCmd))
            n_entry = command();
        else if (tokenIs(firstOfAsmDir))
            n_entry = asmDirective();
        else if (tokenIs(Token::Ident))
            n_entry = asmInstruction();
        else
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                       cboldText(fcolor::red, curr_tk.text),
                       " is not a valid start of statement\n");
        }

        if (!tokenIs(Token::Eol, Token::Eof))
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                       "Expected end of line but found ",
                       cboldText(fcolor::red, curr_tk.text),
                       '\n');
        }

        return n_entry;
    }

    Ast::AsmEntry *asmDirective()
    {
        long line_num = curr_tk.line_num;

        ctx.setCurrLinenum(line_num);

        if (tokenIs(Token::KwDotData))
        {
            getNextToken();

            return ctx.SectionDataCreate();
        }
        else if (tokenIs(Token::KwDotText))
        {
            getNextToken();

            return ctx.SectionTextCreate();
        }
        else if (tokenIs(Token::KwDotGlobal))
        {   
            getNextToken();
            std::string text = curr_tk.text;
            match(Token::Ident, "identifier");

            return ctx.GlobalDirCreate(text);
        }
        else if (tokenIs(Token::KwDotByte))
        {
            getNextToken();
            Ast::DataArgList *args = dataArgList();

            if (!tokenIs(Token::Eol, Token::Eof))
            {
                throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                           "Unexpected ", cboldText(fcolor::red, curr_tk.text),
                           " expecting end of line\n");
            }
            ctx.setCurrLinenum(line_num);

            return ctx.ByteDataCreate(args);
        }
        else if (tokenIs(Token::KwDotHWord))
        {
            getNextToken();
            Ast::DataArgList *args = dataArgList();

            if (!tokenIs(Token::Eol, Token::Eof))
            {
                throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                           "Unexpected ", cboldText(fcolor::red, curr_tk.text),
                           " expecting end of line\n");
            }
            ctx.setCurrLinenum(line_num);

            return ctx.HWordDataCreate(args);
        }
        else if (tokenIs(Token::KwDotWord))
        {
            getNextToken();
            Ast::DataArgList *args = dataArgList();

            if (!tokenIs(Token::Eol, Token::Eof))
            {
                throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                           "Unexpected ", cboldText(fcolor::red, curr_tk.text),
                           " expecting end of line\n");
            }
            ctx.setCurrLinenum(line_num);

            return ctx.WordDataCreate(args);
        }
        else
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                              "Unexpected ", cboldText(fcolor::red, curr_tk.text),
                              " expecting an assembler directive\n");
        }
        
    }

    Ast::DataArgList *dataArgList()
    {
        Ast::DataArgVector args;
        long line_num = curr_tk.line_num;

        Ast::DataArg *arg = dataArg();
        args.push_back(arg);
        while (tokenIs(Token::Comma))
        {
            getNextToken();
            arg = dataArg();
            args.push_back(arg);
        }

        ctx.setCurrLinenum(line_num);
        
        return ctx.DataArgListCreate(args);
    }

    Ast::DataArg *dataArg()
    {
        ctx.setCurrLinenum(curr_tk.line_num);

        switch (curr_tk.token_id)
        {
            case Token::StrLiteral:
            {
                std::string text = curr_tk.text;
                getNextToken();

                return ctx.StrLiteralDataArgCreate(text);
            }
            default:
            {
                Ast::DataArg *arg = constDataArg();

                if (tokenIs(Token::Colon))
                {
                    getNextToken();
                    Ast::ConstDataArg *n_rep = constDataArg();

                    arg = ctx.FillDataArgCreate(Ast::node_cast<Ast::ConstDataArg>(arg), n_rep); 
                }
                return arg;
            }
        }
    }

    Ast::ConstDataArg *constDataArg()
    {
        ctx.setCurrLinenum(curr_tk.line_num);

        Ast::ConstDataArg *n_arg = [this]() -> Ast::ConstDataArg*
        {
            switch (curr_tk.token_id)
            {
                case Token::DecConst:
                    return ctx.DecConstDataArgCreate(curr_tk.text);

                case Token::HexConst:
                    return ctx.HexConstDataArgCreate(curr_tk.text);

                case Token::BinConst:
                    return ctx.BinConstDataArgCreate(curr_tk.text);

                case Token::CharLiteral:
                    return ctx.CharLiteralDataArgCreate(curr_tk.text);

                default:
                    throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                                      "Unexpected text ", cboldText(fcolor::red, curr_tk.text),
                                      " in data definition\n");
            }
        }();

        getNextToken();
        
        return n_arg;
    }

    Ast::AsmEntry *asmInstruction()
    {
        long line_num = curr_tk.line_num;
        std::string ident = curr_tk.text;

        getNextToken();

        Ast::Arg *n_args = [this]() -> Ast::Arg*
        {
            if (tokenIs(firstOfArg))
                return argumentList();
            else
                return ctx.EmptyArgCreate();
        }();

        ctx.setCurrLinenum(line_num);

        return ctx.InstCreate(ident, n_args);
    }

    Ast::AsmEntry *command()
    {
        long line_num = curr_tk.line_num;

        Ast::AsmEntry *n_cmd;
        switch (curr_tk.token_id)
        {
            case Token::KwShow:
            {
                match(Token::KwShow);
                Ast::Arg *n_arg = cmdArgument();

                Ast::Arg *n_sep = nullptr;
                std::optional<Ast::ShowFormat> ofmt = std::nullopt;

                do {
                    if (tokenIs(Token::KwSep))
                    {
                        if (n_sep != nullptr)
                        {
                            throw EAsm::Error(EAsm::SrcInfo{ctx.currFilename(), line_num},
                                              "Separator specified multiple times\n");
                        }
                        getNextToken();
                        match(Token::OpEqual, "operator '='");
                        n_sep = cmdSingleArgument();
                    }
                    else
                    {
                        if (ofmt != std::nullopt) break;

                        ofmt = [this]()
                        {
                            switch (curr_tk.token_id)
                            {
                                case Token::KwHex:
                                    getNextToken();
                                    return Ast::Fmt_Hex;

                                case Token::KwDec:
                                    getNextToken();
                                    return Ast::Fmt_Dec;

                                case Token::KwSigned:
                                    getNextToken();
                                    match(Token::KwDec, "keyword decimal");
                                    return Ast::Fmt_SDec;

                                case Token::KwUnsigned:
                                    getNextToken();
                                    match(Token::KwDec, "keyword decimal");
                                    return Ast::Fmt_UDec;

                                case Token::KwBinary:
                                    getNextToken();
                                    return Ast::Fmt_Bin;

                                case Token::KwAscii:
                                    getNextToken();
                                    return Ast::Fmt_Ascii;

                                default:
                                    return Ast::Fmt_Auto;
                            }
                        }();
                    }
                } while (true);

                ctx.setCurrLinenum(line_num);

                if (!n_sep) n_sep = ctx.EmptyArgCreate();

                return ctx.ShowCmdCreate(n_arg, n_sep, *ofmt);
            }
            case Token::KwSet:
            {
                getNextToken();
                Ast::Arg *n_larg = cmdArgument();
                match(Token::OpEqual, "=");
                Ast::Arg *n_rarg = cmdArgument();

                ctx.setCurrLinenum(line_num);

                return ctx.SetCmdCreate(n_larg, n_rarg);
            }
            case Token::KwExec:
            {
                getNextToken();
                std::string str = curr_tk.text;
                match(Token::StrLiteral, "string literal");

                ctx.setCurrLinenum(line_num);

                return ctx.ExecCmdCreate(ctx.StrLiteralCreate(str));
            }
            case Token::KwReset:
            {
                getNextToken();
                ctx.setCurrLinenum(line_num);

                return ctx.ResetCmdCreate();
            }
            case Token::KwStop:
            {
                getNextToken();
                ctx.setCurrLinenum(line_num);

                return ctx.StopCmdCreate();
            }
            default:
                throw EAsm::Error(EAsm::SrcInfo{ctx.currFilename(), line_num},
                                  "Expected start of command, but found ",
                                  cboldText(fcolor::red, curr_tk.text),
                                  '\n');
        }
    }

    Ast::Arg *argumentList()
    {
        long line_num = curr_tk.line_num;
        
        Ast::ArgVector args;
        Ast::Arg *arg = argument();
        args.push_back(arg);

        while (tokenIs(Token::Comma))
        {
            getNextToken();

            arg = argument();
            args.push_back(arg);
        }
        if (!tokenIs(Token::Eol, Token::Eof))
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                       "Unexpected text ", cboldText(fcolor::red, curr_tk.text),
                       ". Did you forget a ", cboldText(fcolor::cyan, "comma"),
                       " before ", cboldText(fcolor::yellow, curr_tk.text), "?",
                       '\n');
        }

        ctx.setCurrLinenum(curr_tk.line_num);
        
        return ctx.ArgListCreate(args);
    }

    Ast::Arg *cmdSingleArgument()
    {
        ctx.setCurrLinenum(curr_tk.line_num);
        
        switch (curr_tk.token_id)
        {
            case Token::KwByte:
            case Token::KwHword:
            case Token::KwWord:
                return memRefArgument();

            case Token::StrLiteral:
            {
                std::string text = curr_tk.text;
                getNextToken();

                return ctx.StrLiteralCreate(text);
            }
            default:
            {
                Ast::Arg *arg = argument();
                Ast::Arg *n_const = nullptr;

                if (tokenIs(Token::Colon))
                {
                    getNextToken();
                    n_const = constant();
                }

                return (n_const? ctx.MemRefCreate(Ast::SS_Empty, n_const, arg) : arg);
            }
        }
    }

    Ast::Arg *memRefArgument()
    {
        long line_num = curr_tk.line_num;

        Ast::SizeSpec sz_spec = [this]()
        {
            switch (curr_tk.token_id)
            {
                case Token::KwByte:
                    match(Token::KwByte);
                    return Ast::SS_Byte;

                case Token::KwHword:
                    match(Token::KwHword);
                    return Ast::SS_HWord;

                case Token::KwWord:
                    match(Token::KwWord);
                    return Ast::SS_Word;

                default:
                    return Ast::SS_Empty;
            }
        }();

        Ast::Arg *n_addr = argument();

        Ast::Arg *n_const = [this]() -> Ast::Arg*
        {
            if (tokenIs(Token::Colon))
            {
                getNextToken();
                return constant();
            }
            else
                return ctx.EmptyArgCreate();
        }();

        ctx.setCurrLinenum(curr_tk.line_num);

        return ctx.MemRefCreate(sz_spec, n_const, n_addr);
    }

    Ast::Arg *cmdArgument()
    {
        switch (curr_tk.token_id)
        {
            case Token::OpenBracket:
                return cmdArgumentList();

            default:
                return cmdSingleArgument();
        }
    }

    Ast::Arg *cmdArgumentList()
    {
        long line_num = curr_tk.line_num;

        getNextToken();
        
        Ast::ArgVector args;
        Ast::Arg *arg = cmdSingleArgument();
        args.push_back(arg);

        while (tokenIs(Token::Comma))
        {
            getNextToken();

            arg = cmdSingleArgument();
            args.push_back(arg);
        }
        if (!tokenIs(Token::CloseBracket))
        {
            throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                       "Unexpected text ", cboldText(fcolor::red, curr_tk.text),
                       ". Did you forget a ", cboldText(fcolor::cyan, "comma"),
                       " before ", cboldText(fcolor::yellow, curr_tk.text), "?",
                       '\n');
        }
        getNextToken();

        ctx.setCurrLinenum(curr_tk.line_num);

        return ctx.ArgListCreate(args);
    }

    Ast::Arg *argument()
    {
        ctx.setCurrLinenum(curr_tk.line_num);

        switch (curr_tk.token_id)
        {
            case Token::RegName:
            {
                std::string rname = curr_tk.text;
                getNextToken();

                return ctx.RegNameCreate(rname);
            }
            case Token::RegIndex:
            {
                std::string s_index = curr_tk.text;
                getNextToken();

                return ctx.RegIndexCreate(s_index);
            }
            case Token::OpenPar:
            {
                getNextToken();
                Ast::Node *n_arg = argument();
                match(Token::ClosePar, ")");
                return ctx.BaseOffsetCreate(ctx.DecConstCreate("0"), n_arg);
            }
            default:
            {
                Ast::Arg *n_arg = constant();
                if (tokenIs(Token::OpenPar))
                {
                    getNextToken();
                    Ast::Node *n_arg1 = argument();
                    match(Token::ClosePar, ")");
                    n_arg = ctx.BaseOffsetCreate(n_arg, n_arg1);
                }
                return n_arg;
            }
        }
    }

    Ast::Arg *constant()
    {
        ctx.setCurrLinenum(curr_tk.line_num);

        switch (curr_tk.token_id)
        {
            case Token::DecConst:
            {
                std::string sval = curr_tk.text;
                getNextToken();

                return ctx.DecConstCreate(sval);
            }
            case Token::HexConst:
            {
                std::string sval = curr_tk.text;
                getNextToken();

                return ctx.HexConstCreate(sval);
            }
            case Token::BinConst:
            {
                std::string sval = curr_tk.text;
                getNextToken();

                return ctx.BinConstCreate(sval);
            }
            case Token::CharLiteral:
            {
                std::string sval = curr_tk.text;
                getNextToken();

                return ctx.CharLiteralCreate(sval);
            }
            case Token::DotIdent:
            case Token::DollarIdent:
            case Token::Ident:
            {
                std::string str = curr_tk.text;
                getNextToken();

                return ctx.IdentCreate(str);
            }
            case Token::KwHiHw:
            {
                getNextToken();
                match(Token::OpenPar, "(");
                Ast::Node *n_arg1 = argument();
                match(Token::ClosePar, ")");

                return ctx.HiHwCreate(n_arg1);
            }
            case Token::KwLoHw:
            {
                getNextToken();
                match(Token::OpenPar, "(");
                Ast::Node *n_arg1 = argument();
                match(Token::ClosePar, ")");

                return ctx.LoHwCreate(n_arg1);
            }
            default:
                throw EAsm::Error(EAsm::SrcInfo{ ctx.currFilename(), curr_tk.line_num },
                                  "Unexpected ", cboldText(fcolor::red, curr_tk.text),
                                  ", expected ", cboldText(fcolor::cyan, "decimal"),
                                  ", ", cboldText(fcolor::cyan, "hexadecimal"),
                                  ", ", cboldText(fcolor::cyan, "binary"),
                                  " or ", cboldText(fcolor::cyan, "char"),
                                  " constant\n");
        }
    }

private:
    Token curr_tk;
    Lexer &lexer;
    Ast::NodePool &ctx;
};

Ast::AsmProgram *Parser::parse()
{
    ParserHelper ph(lexer, ctx);

    ph.getNextToken();
    return ph.input();
}

}  // Namespace Mips32