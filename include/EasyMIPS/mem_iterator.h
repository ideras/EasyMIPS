#ifndef _MEM_ITERATOR_H_
#define _MEM_ITERATOR_H_

#include <cstdint>
#include <cstddef>

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif

#define WORD_SIZE (sizeof(uint32_t))

enum class ByteOrder { LittleEndian, BigEndian, None };

template<typename T>
class MemIterator
{
public:
    MemIterator()
    : MemIterator(nullptr, ByteOrder::None, 0)
    {}

    MemIterator(void *mem)
    : MemIterator(mem, ByteOrder::LittleEndian, 0)
    {}
    
    MemIterator(void *mem, ByteOrder byte_order)
    : MemIterator(mem, byte_order, 0)
    {}

    MemIterator(void *mem, ByteOrder byte_order, size_t ofs)
    : mem(mem), byte_order(byte_order), ofs(ofs)
    {}

    T& operator*()
    { 
        size_t aofs = adjustOffset(ofs);

        return reinterpret_cast<T&>(*(reinterpret_cast<uint8_t *>(mem) + aofs));
    }

    bool operator!=(const MemIterator<T>& other)
    { return !operator==(other); }

    bool operator==(const MemIterator<T>& other)
    { 
        return ((mem == other.mem)
                && (byte_order == other.byte_order)
                && (ofs == other.ofs));
    }

    MemIterator operator++(int)
    {
        MemIterator<T> tmp = *this;
        ofs += sizeof(T);
        return tmp;
    }

    bool isNull()
    { return mem == nullptr; }

    long offset()
    { return ofs; }

    ByteOrder byteOrder()
    { return byte_order; }

private:
    size_t adjustOffset(size_t ofs)
    {
        if (byte_order == ByteOrder::LittleEndian)
        {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
            return ofs;
        #else
            size_t word_ofs = ofs / WORD_SIZE;
            size_t byte_ofs = ofs % WORD_SIZE;
            size_t max_byte_ofs = ((WORD_SIZE/sizeof(T)) - 1) * sizeof(T);

            return ((word_ofs * 4) + (max_byte_ofs - byte_ofs));
        #endif
        }
        else
        {
        #if __BYTE_ORDER == __BIG_ENDIAN
            return ofs;
        #else
            size_t word_ofs = ofs / WORD_SIZE;
            size_t byte_ofs = ofs % WORD_SIZE;
            size_t max_byte_ofs = ((WORD_SIZE/sizeof(T)) - 1) * sizeof(T);

            return ((word_ofs * 4) + (max_byte_ofs - byte_ofs));
        #endif
        }
    }

private:
    void *mem;
    ByteOrder byte_order;
    size_t ofs;
};

#endif