#ifndef __MIPS32_LEXER_H__
#define __MIPS32_LEXER_H__

#include <inttypes.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#define LEX_BUFF_SIZE 4096

namespace Mips32
{
    struct Token
    {
        enum
        {
            KwDotGlobal, KwDotData, KwDotText, KwDotByte, KwDotHWord,
            KwDotWord, KwDotFill, KwShow, KwSet, KwExec,
            KwStop, KwDebug, KwReset, KwByte, KwHword, KwWord,
            KwHex, KwDec, KwSigned, KwUnsigned, KwBinary, KwAscii, KwSep,
            KwHiHw, KwLoHw, RegIndex, RegName, Ident, DotIdent, DollarIdent,
            StrLiteral, Label, OpenBracket, CloseBracket, OpenPar, ClosePar,
            Colon, Comma, Dot, DecConst, HexConst, BinConst, CharLiteral,
            OpEqual, OpMinus, Eol, Error, Eof
        };

        Token() : line_num(0), token_id(-1) {}

        Token(long line, long tid, const std::string &txt)
            : line_num(line), token_id(tid), text(txt)
        { }

        Token(const Token& rhs)
            : line_num(rhs.line_num),
              token_id(rhs.token_id),
              text(rhs.text)
        {}

        Token(Token&& rhs)
            : line_num(rhs.line_num),
              token_id(rhs.token_id),
              text(std::move(rhs.text))
        { rhs.token_id = -1; }

        Token& operator=(Token&& rhs)
        {
            line_num = rhs.line_num;
            token_id = rhs.token_id;
            text = std::move(rhs.text);
            rhs.token_id = -1;

            return *this;
        }

        Token& operator=(const Token& rhs)
        {
            line_num = rhs.line_num;
            token_id = rhs.token_id;
            text = rhs.text;

            return *this;
        }

        bool isNone() { return token_id < 0; }

        long line_num;
        long token_id;
        std::string text;
    };

    class Lexer
    {
    public:
        Lexer(std::istream &in)
            : line_num(1),
              state(State::Default),
              ctx(in) {}

        ~Lexer(){};

        Token getNextToken();
        static const char *tokenToString(unsigned tk);

    private:
        enum class State
        {
            Default,
            InShowCmd,
            InSetCmd
        };
        enum class FillStatus { Ok, Error, Eof };

        struct Context
        {
            std::vector<char> buff;
            char *limit;
            char *cur;
            char *tok;
            char *mark;
            bool eof;
            long line_num;
            std::istream &in;

            Context(std::istream &in);
            FillStatus fill(size_t need);

            std::string tokenText(bool discard_quotes = false)
            {
                return discard_quotes ? std::string(tok + 1, cur - 1) : std::string(tok, cur);
            }
        };

        Token resolveIdent();
        Token resolveDollarIdent();
        Token resolveDotIdent();

        Token makeToken(unsigned tk_id, bool discard_quotes = false)
        {
            return Token(ctx.line_num, tk_id, ctx.tokenText(discard_quotes));
        }

        Token TkEof()
        {
            return Token(ctx.line_num, Token::Eof, "<<EOF>>");
        }

        Token TkError()
        {
            return Token(ctx.line_num, Token::Error, ctx.tokenText());
        }

    private:
        long line_num;
        State state;
        Context ctx;
    };

} // namespace Mips32

#endif // __MIPS32_LEXER_H__
