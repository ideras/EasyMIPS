#ifndef _COLORIZER_H_
#define _COLORIZER_H_

#include <ostream>
#include "rang.hpp"

using fcolor = rang::fg;
using bcolor = rang::bg;

template<typename T>
struct ColorText
{
    ColorText(fcolor fg_color,  T value)
    : fg_color(fg_color), value(value)
    {}

    fcolor fg_color;
    T value;
};

template<typename T>
struct CStyleText
{
    CStyleText(fcolor fg_color,  rang::style style, T value)
    : fg_color(fg_color), style(style), value(value)
    {}

    fcolor fg_color;
    rang::style style;
    T value;
};

template<typename T>
struct StyleText
{
    StyleText(rang::style style, T value)
    : style(style), value(value)
    {}

    rang::style style;
    T value;
};

template<typename T>
ColorText<T> colorText(fcolor fgc, T val)
{
    return ColorText<T>(fgc, val);
}

template<typename T>
CStyleText<T> cboldText(fcolor fgc, T val)
{
    return CStyleText<T>(fgc, rang::style::bold, val);
}

template<typename T>
StyleText<T> boldText(T val)
{
    return StyleText<T>(rang::style::bold, val);
}

template<typename T>
static inline std::ostream& operator<<(std::ostream& out, const ColorText<T>& ctxt)
{
    out << ctxt.fg_color << ctxt.value
        << rang::style::reset << fcolor::reset;

    return out;
}

template<typename T>
static inline std::ostream& operator<<(std::ostream& out, const CStyleText<T>& ctxt)
{
    out << ctxt.fg_color << ctxt.style << ctxt.value
        << rang::style::reset << fcolor::reset;

    return out;
}

template<typename T>
static inline std::ostream& operator<<(std::ostream& out, const StyleText<T>& ctxt)
{
    out << ctxt.style << ctxt.value << rang::style::reset;

    return out;
}

#endif
