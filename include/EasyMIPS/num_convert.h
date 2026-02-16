#ifndef _NUM_CONVERT_H_
#define _NUM_CONVERT_H_

#include <cstdint>
#include <cstddef>
#include <iosfwd>

namespace Cvt
{
    enum class Format
    {
        SDecimal, UDecimal,
        Hex, Octal,
        Binary, Ascii
    };

    template <typename T>
    struct FmtVal
    {
        FmtVal(T val, Format fmt)
        : val(val), fmt(fmt)
        {}

        T val;
        Format fmt;
    };

    template <Format fmt, typename T>
    struct TFmtVal
    {
        TFmtVal(T val, size_t len)
        : val(val), len(len)
        {}

        T val;
        size_t len;
    };

    template <typename T>
    TFmtVal<Format::Hex, T> hexVal(T val, size_t len = sizeof(T) * 2)
    { return TFmtVal<Format::Hex, T>(val, len); }

    template <typename T>
    TFmtVal<Format::Octal, T> octVal(T val, size_t len = (sizeof(T) * 8 + 2) / 3)
    { return TFmtVal<Format::Octal, T>(val, len); }

    template <typename T>
    TFmtVal<Format::Binary, T> binVal(T val, size_t len = sizeof(T) * 8)
    { return TFmtVal<Format::Binary, T>(val, len); }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const TFmtVal<Format::Hex, T>& hval)
    {
        static const char *digits = "0123456789abcdef";

        out << "0x";
        for (size_t i = 2, j = (hval.len - 1) * 4; i < hval.len + 2; i++, j -= 4)
            out << digits[(hval.val >> j) & 0x0f];

        return out;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const TFmtVal<Format::Octal, T>& oval)
    {
        static const char *digits = "01234567";

        out << '0';
        for (size_t i = 1, j = (oval.len - 1) * 3; i < oval.len + 1; i++, j -= 3)
            out << digits[(oval.val >> j) & 0x07];

        return out;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const TFmtVal<Format::Binary, T>& bval)
    {
        out << "0b";
     
        uint32_t mask = 1 << (bval.len - 1);
        for (int i = 2; mask > 0; mask >>= 1, i++)
            out << (((bval.val & mask) == 0) ? '0' : '1');

        return out;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const FmtVal<T>& fval)
    {
        switch (fval.fmt)
        {
            case Format::SDecimal:
                out << static_cast<typename std::make_signed_t<T>>(fval.val);
                break;

            case Format::UDecimal:
                out << static_cast<typename std::make_unsigned_t<T>>(fval.val);
                break;

            case Format::Hex:
                out << hexVal(fval.val);
                break;

            case Format::Octal:
                out << octVal(fval.val);
                break;

            case Format::Binary:
                out << binVal(fval.val);
                break;

            case Format::Ascii:
                out << static_cast<char>(fval.val);
                break;

            default:
                out << fval.val;
        }

        return out;
    }
}

#endif