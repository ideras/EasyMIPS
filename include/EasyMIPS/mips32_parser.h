#ifndef __MIPS32_PARSER_H__
#define __MIPS32_PARSER_H__

#include "mips32_lexer.h"
#include "mips32_ast.h"

namespace Mips32 {

class Parser {
public:
    Parser(Lexer& lexer, Ast::NodePool& ctx)
        : lexer(lexer), ctx(ctx) {}

    Ast::AsmProgram* parse();

private:
    Lexer& lexer;
    Ast::NodePool& ctx;
};

}
#endif