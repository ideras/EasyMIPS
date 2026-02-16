#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <sstream>
#include <string>
#include "doctest.h"
#include "mips32_lexer.h"

#define ARRAY_SIZE(a) (sizeof((a))/sizeof((a)[0]))

using Token = Mips32::Token;

#define KW_DOTGLOBAL { Token::KwDotGlobal, ".global" }
#define KW_DOTDATA { Token::KwDotData, ".data" }
#define KW_DOTTEXT { Token::KwDotText, ".text" }
#define KW_DOTBYTE { Token::KwDotByte, ".byte" }
#define KW_DOTHWORD { Token::KwDotHWord, ".hword" }
#define KW_DOTWORD { Token::KwDotWord, ".word" }
#define KW_IMPORT { Token::KwImport, "#import" }
#define KW_SHOW { Token::KwShow, "#show" }
#define KW_SET { Token::KwSet, "#set" }
#define KW_EXEC { Token::KwExec, "#exec" }
#define KW_STOP { Token::KwStop, "#stop" }
#define KW_DEBUG { Token::KwDebug, "#debug" }
#define KW_RESET { Token::KwReset, "#reset" }
#define KW_SEP { Token::KwSep, "sep" }
#define KW_BYTE { Token::KwByte, "byte" }
#define KW_HWORD { Token::KwHword, "hword" }
#define KW_WORD { Token::KwWord, "word" }
#define KW_HEXADECIMAL { Token::KwHex, "hexadecimal" }
#define KW_HEX { Token::KwHex, "hex" }
#define KW_DECIMAL { Token::KwDec, "decimal" }
#define KW_SIGNED { Token::KwSigned, "signed" }
#define KW_UNSIGNED { Token::KwUnsigned, "unsigned" }
#define KW_BINARY { Token::KwBinary, "binary" }
#define KW_ASCII { Token::KwAscii, "ascii" }
#define KW_HIHW { Token::KwHiHw, "#hihw" }
#define KW_LOHW { Token::KwLoHw, "#lohw" }
#define REGINDEX(txt) { Token::RegIndex, txt }
#define REGNAME(txt) { Token::RegName, txt }
#define IDENT(txt) { Token::Ident, txt }
#define STRLITERAL(txt) { Token::StrLiteral, txt }
#define LABEL(txt) { Token::Label, txt }
#define OPENBRACKET { Token::OpenBracket, "[" }
#define CLOSEBRACKET { Token::CloseBracket, "]" }
#define OPENPAR { Token::OpenPar, "(" }
#define CLOSEPAR { Token::ClosePar, ")" }
#define COLON { Token::Colon, ":" }
#define COMMA { Token::Comma, "," }
#define DOT { Token::Dot, "." }
#define DEC_CONST(txt) { Token::DecConst, txt }
#define HEX_CONST(txt) { Token::HexConst, txt }
#define BIN_CONST(txt) { Token::BinConst, txt }
#define CHAR_LITERAL(txt) { Token::CharLiteral, txt }
#define OPEQUAL { Token::OpEqual, "=" }
#define OPMINUS { Token::OpMinus, "-" }
#define EOL { Token::Eol, "\n" }
#define ERROR(txt) { Token::Error, txt }
#define TK_EOF { Token::Eof, "<<EOF>>" }

struct TokenInfo {
    unsigned token;
    const char *text;
};

bool operator==(const Token& tk, const TokenInfo& tki)
{
    return ((tk.token_id == tki.token) &&
            (tk.text == tki.text));
}

const char *tokenIdToString(unsigned tk_id)
{
    switch (tk_id)
    {
        case Token::KwShow: return "KwShow";
        case Token::KwSet: return "KwSet";
        case Token::KwExec: return "KwExec";
        case Token::KwStop: return "KwStop";
        case Token::KwSep: return "KwSep";
        case Token::KwByte: return "KwByte";
        case Token::KwHword: return "KwHword";
        case Token::KwWord: return "KwWord";
        case Token::KwHex: return "KwHex";
        case Token::KwDec: return "KwDec";
        case Token::KwSigned: return "KwSigned";
        case Token::KwUnsigned: return "KwUnsigned";
        case Token::KwBinary: return "KwBinary";
        case Token::KwAscii: return "KwAscii";
        case Token::KwHiHw: return "KwHiHw";
        case Token::KwLoHw: return "KwLoHw";
        case Token::RegIndex: return "RegIndex";
        case Token::RegName: return "RegName";
        case Token::Ident: return "Ident";
        case Token::StrLiteral: return "StrLiteral";
        case Token::OpenBracket: return "OpenBracket";
        case Token::CloseBracket: return "CloseBracket";
        case Token::OpenPar: return "OpenPar";
        case Token::ClosePar: return "ClosePar";
        case Token::Colon: return "Colon";
        case Token::Comma: return "Comma";
        case Token::Dot: return "Dot";
        case Token::DecConst: return "DecConst";
        case Token::HexConst: return "HexConst";
        case Token::BinConst: return "BinConst";
        case Token::CharLiteral: return "CharConst";
        case Token::OpEqual: return "OpEqual";
        case Token::OpMinus: return "OpMinus";
        case Token::Eol: return "Eol";
        case Token::Error: return "Error";
        case Token::Eof: return "Eof";
        default:
            return "Unk";
    }
}

namespace Mips32
{
    std::ostream& operator<< (std::ostream& out, const Token& tk) 
    {
        out << tokenIdToString(tk.token_id) << ":" << tk.text;
        return out;
    }
}

std::ostream& operator<< (std::ostream& out, const TokenInfo& tk) 
{
    out << tokenIdToString(tk.token) << ":" << tk.text;
    return out;
}

const char *test1 = ".global "
                    ".data "
                    ".text "
                    ".byte "
                    ".hword "
                    ".word "
                    "#show "
                    "#set "
                    "#exec "
                    "#stop "
                    "#debug "
                    "#reset "
                    "sep "
                    "byte "
                    "hword "
                    "word "
                    "hex "
                    "dec "
                    "signed "
                    "unsigned "
                    "binary "
                    "ascii "
                    "#hihw "
                    "#lohw "
                    "$30 "
                    "$ra "
                    "long_name_1 "
                    "\"String literal\" "
                    "long_label_name_1: "
                    "["
                    "]"
                    "("
                    ")"
                    ":"
                    ","
                    "."
                    "34587346 "
                    "0xdeadbeef "
                    "0b1001001 "
                    "'H' "
                    "="
                    "-"
                    "\n";

TokenInfo test1Exp[] = {
    KW_DOTGLOBAL,
    KW_DOTDATA,
    KW_DOTTEXT,
    KW_DOTBYTE,
    KW_DOTHWORD,
    KW_DOTWORD,
    KW_SHOW,
    KW_SET,
    KW_EXEC,
    KW_STOP,
    KW_DEBUG,
    KW_RESET,
    IDENT("sep"),
    KW_BYTE,
    KW_HWORD,
    KW_WORD,
    IDENT("hex"),
    IDENT("dec"),
    IDENT("signed"),
    IDENT("unsigned"),
    IDENT("binary"),
    IDENT("ascii"),
    KW_HIHW,
    KW_LOHW,
    REGINDEX("$30"),
    REGNAME("$ra"),
    IDENT("long_name_1"),
    STRLITERAL("String literal"),
    LABEL("long_label_name_1:"),
    OPENBRACKET,
    CLOSEBRACKET,
    OPENPAR,
    CLOSEPAR,
    COLON,
    COMMA,
    DOT,
    DEC_CONST("34587346"),
    HEX_CONST("0xdeadbeef"),
    BIN_CONST("0b1001001"),
    CHAR_LITERAL("H"),
    OPEQUAL,
    OPMINUS,
    EOL,
};

const char *test2 = "; Line comment\n"
                    "#set $t0 = 876 ; Line comment \n"
                    "#set $t1 = 345\n"
                    "addiu $v0, $t0, $t1 ; Line comment \n";

TokenInfo test2Exp[] = {
    EOL,
    KW_SET, REGNAME("$t0"), OPEQUAL, DEC_CONST("876"), EOL,
    KW_SET, REGNAME("$t1"), OPEQUAL, DEC_CONST("345"), EOL,
    IDENT("addiu"), REGNAME("$v0"),  COMMA, REGNAME("$t0"),
    COMMA, REGNAME("$t1"), EOL, TK_EOF
};

static const char *testShowKeywordsStr = R"(
    #show hexadecimal hex decimal signed unsigned binary byte ascii hword word sep
)";

static TokenInfo testShowKeywords[] = {
    EOL,
    KW_SHOW, KW_HEXADECIMAL, KW_HEX,
    KW_DECIMAL, KW_SIGNED,
    KW_UNSIGNED, KW_BINARY,
    KW_BYTE, KW_ASCII,
    KW_HWORD, KW_WORD,
    KW_SEP, EOL, TK_EOF
};

static const char *testSetKeywordsStr = R"(
    #set byte hword word
)";

static TokenInfo testSetKeywords[] = {
    EOL,
    KW_SET, KW_BYTE, KW_HWORD, KW_WORD, EOL, TK_EOF
};

TEST_CASE("MIPS32 lexer test 1: Simple test") {
    std::istringstream in;

    in.str(test1);
    Mips32::Lexer lexer(in);
    Token tk = lexer.getNextToken();

    for (int i = 0; i < ARRAY_SIZE(test1Exp); i++) {
        INFO("Iteration: " << i);
        CHECK( tk == test1Exp[i] );
        tk = lexer.getNextToken();
    }
}

TEST_CASE("MIPS32 lexer test 2: Line comments") {
    std::istringstream in;

    in.str(test2);
    Mips32::Lexer lexer(in);
    Token tk = lexer.getNextToken();

    for (int i = 0; i < ARRAY_SIZE(test2Exp); i++) {
        INFO("Iteration: " << i);
        CHECK( tk == test2Exp[i] );
        tk = lexer.getNextToken();
    }
}

TEST_CASE("MIPS32 lexer test 3: Show keywords") {
    std::istringstream in;

    in.str(testShowKeywordsStr);
    Mips32::Lexer lexer(in);
    Token tk = lexer.getNextToken();

    for (int i = 0; i < ARRAY_SIZE(testShowKeywords); i++) {
        INFO("Iteration: " << i);
        CHECK( tk == testShowKeywords[i] );
        tk = lexer.getNextToken();
    }
}

TEST_CASE("MIPS32 lexer test 4: Set keywords") {
    std::istringstream in;

    in.str(testSetKeywordsStr);
    Mips32::Lexer lexer(in);
    Token tk = lexer.getNextToken();

    for (int i = 0; i < ARRAY_SIZE(testSetKeywords); i++) {
        INFO("Iteration: " << i);
        CHECK( tk == testSetKeywords[i] );
        tk = lexer.getNextToken();
    }
}
