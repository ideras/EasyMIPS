#include <iostream>
#include <cstring>
#include <istream>
#include "mips32_lexer.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
/*!max:re2c*/
   
namespace Mips32
{
static const char *reg_name[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", 
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
    "$lo", "$hi", "$pc"
};

struct Keyword {
    const char *text;
    unsigned token_id;
};

static Keyword kw_show[] = {
    {"hexadecimal", Token::KwHex}, {"hex", Token::KwHex },
    {"decimal", Token::KwDec }, {"signed", Token::KwSigned },
    {"unsigned", Token::KwUnsigned }, {"binary", Token::KwBinary },
    {"byte", Token::KwByte }, {"ascii", Token::KwAscii},
    {"hword", Token::KwHword}, {"word", Token::KwWord},
    {"sep", Token::KwSep}
};

static Keyword kw_set[] = {
    {"byte", Token::KwByte },
    {"hword", Token::KwHword},
    {"word", Token::KwWord}
};

static Keyword dot_kw[] = {
    {".global", Token::KwDotGlobal},
    {".data", Token::KwDotData},
    {".text", Token::KwDotText},
    {".byte", Token::KwDotByte},
    {".hword", Token::KwDotHWord},
    {".word", Token::KwDotWord},
};

Token Lexer::resolveIdent()
{
    std::string text = ctx.tokenText();
    if (state == State::InSetCmd)
    {
        for (int i = 0; i < ARRAY_SIZE(kw_set); i++)
        {
            if (kw_set[i].text == text)
                return Token(ctx.line_num, kw_set[i].token_id, text);
        }
    }
    else if (state == State::InShowCmd)
    {
        for (int i = 0; i < ARRAY_SIZE(kw_show); i++)
        {
            if (kw_show[i].text == text)
                return Token(ctx.line_num, kw_show[i].token_id, text);
        }
    }
    return Token(ctx.line_num, Token::Ident, text);
}

Token Lexer::resolveDollarIdent()
{
    std::string text = ctx.tokenText();
    for (int i = 0; i < ARRAY_SIZE(reg_name); i++)
    {
        if (text == reg_name[i])
            return Token(ctx.line_num, Token::RegName, text);
    }
    return Token(ctx.line_num, Token::DollarIdent, text);
}

Token Lexer::resolveDotIdent()
{
    std::string text = ctx.tokenText();
    for (int i = 0; i < ARRAY_SIZE(dot_kw); i++)
    {
        if (dot_kw[i].text == text)
            return Token(ctx.line_num, dot_kw[i].token_id, text);
    }

    return Token(ctx.line_num, Token::DotIdent, text);
}

Token Lexer::getNextToken() {
    #define YYFILL(n) do { \
        FillStatus st = ctx.fill(n); \
        switch (st) { \
            case FillStatus::Eof: return TkEof(); \
            case FillStatus::Error: return TkError(); \
            default: \
                break; \
        } \
    }  while (0)

    while (true) {
        ctx.tok = ctx.cur;
        ctx.line_num = line_num;
        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = ctx.cur;
            re2c:define:YYLIMIT = ctx.limit;
            re2c:define:YYMARKER = ctx.mark;
            re2c:define:YYFILL:naked = 0;

            END = "\x00";
            HEX_CONST = "0"[xX][0-9a-fA-F]+;
            BIN_CONST = "0"[bB][01]+;
            DEC_CONST = [-+]?[0-9]+;
            REG_INDEX = "$"[0-9]+;
            IDENT = [a-zA-Z_][a-zA-Z0-9_]*;
            DOLLAR_IDENT = "$"[a-zA-Z_][a-zA-Z0-9_]*;
            DOT_IDENT = "."[a-zA-Z_][a-zA-Z0-9_]*;
            WSP = [ \t]+;

            *   { return TkError(); }
            WSP { continue; }
            "\r\n" { line_num++; state = State::Default; return makeToken(Token::Eol); }
            "\n"   { line_num++; state = State::Default; return makeToken(Token::Eol); }
            "#show" { state = State::InShowCmd; return makeToken(Token::KwShow); }
            "#set" { state = State::InSetCmd; return makeToken(Token::KwSet); }
            "#stop" { return makeToken(Token::KwStop); }
            "#hihw" { return makeToken(Token::KwHiHw); }
            "#lohw" { return makeToken(Token::KwLoHw); }
            "#exec" { return makeToken(Token::KwExec); }
            "#debug" { return makeToken(Token::KwDebug); }
            "#reset" { return makeToken(Token::KwReset); }
            DOT_IDENT { return resolveDotIdent(); }
            DOLLAR_IDENT { return resolveDollarIdent(); }
            IDENT  { return resolveIdent(); }
            (DOT_IDENT | IDENT | DOLLAR_IDENT) ":" { return makeToken(Token::Label); }
            REG_INDEX { return makeToken(Token::RegIndex); }
            '"'[^"]*'"' { return makeToken(Token::StrLiteral, true); }
            "'"[^']+"'" { return makeToken(Token::CharLiteral, true); }
            BIN_CONST { return makeToken(Token::BinConst); }
            HEX_CONST { return makeToken(Token::HexConst); }
            DEC_CONST { return makeToken(Token::DecConst); }
            ";"[^\n\x00]* { continue; }
            "(" { return makeToken(Token::OpenPar); }
            ")" { return makeToken(Token::ClosePar); }
            "[" { return makeToken(Token::OpenBracket); }
            "]" { return makeToken(Token::CloseBracket); }
            "," { return makeToken(Token::Comma); }
            "-" { return makeToken(Token::OpMinus); }
            "=" { return makeToken(Token::OpEqual); }
            "." { return makeToken(Token::Dot); }
            ":" { return makeToken(Token::Colon); }
            END { return ctx.eof? TkEof() : TkError(); }
        */
    }
}

Lexer::Context::Context(std::istream& in): in(in) {
    buff.resize(LEX_BUFF_SIZE + YYMAXFILL);
    limit = buff.data() + LEX_BUFF_SIZE;
    cur = limit;
    mark = limit;
    tok = limit;
    eof = false;
}

Lexer::FillStatus Lexer::Context::fill(size_t need) {
    if (eof) {
        return FillStatus::Eof;
    }
    const size_t free = tok - buff.data();
    if (free < need) {
        return FillStatus::Error;
    }
    memmove(buff.data(), tok, limit - tok);
    limit -= free;
    cur -= free;
    mark -= free;
    tok -= free;
    in.read(limit, free);
    limit += in.gcount();
    if (limit < buff.data() + LEX_BUFF_SIZE) {
        eof = true;
        memset(limit, 0, YYMAXFILL);
        limit += YYMAXFILL;
    }
    return FillStatus::Ok;
}

const char *Lexer::tokenToString(unsigned tk_id)
{
    switch (tk_id) {
        default:
            return "Unknown";
    }
}


} // namespace Mips32