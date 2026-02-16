#ifndef __SIM_RUNTIME_H__
#define __SIM_RUNTIME_H__

#include <cstdint>
#include <unordered_map>

using VirtualAddr = uint32_t;

enum class WordSize: unsigned { _8Bit, _16Bit, _32Bit };

template <typename TFrom, typename TTo>
TTo extend_cast(TFrom v)
{   return static_cast<TTo>(v); }

static inline size_t sizeOf(WordSize ws)
{ return (1 << static_cast<unsigned>(ws)); }

#endif
