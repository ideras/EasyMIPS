#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "mem_iterator.h"

TEST_CASE("Memory Iterator - Big Endian")
{
    uint32_t words[] = {0xaabbccdd, 0x11223344, 0xbacafea0};
    uint8_t bytes[] = {0xaa, 0xbb, 0xcc, 0xdd, 0x11, 0x22, 0x33, 0x44, 0xba, 0xca, 0xfe, 0xa0};
    
    MemIterator<uint8_t> itb(words, ByteOrder::BigEndian);

    for (size_t i = 0; i < sizeof(bytes); i++)
    {
        CHECK( *itb == bytes[i] );
        itb++;
    }

    MemIterator<uint16_t> ith(words, ByteOrder::BigEndian);
    uint16_t hwords[] = {0xaabb, 0xccdd, 0x1122, 0x3344, 0xbaca, 0xfea0};
    
    for (size_t i = 0; i < sizeof(hwords)/sizeof(hwords[0]); i++)
    {
        CHECK( *ith == hwords[i] );
        ith++;
    }

    MemIterator<uint32_t> itw(words, ByteOrder::BigEndian);
    for (size_t i = 0; i < sizeof(words)/sizeof(words[0]); i++)
    {
        CHECK( *itw == words[i] );
        itw++;
    }
}

TEST_CASE("Memory Iterator - Little Endian")
{
    uint32_t words[] = {0xaabbccdd, 0x11223344, 0xbacafea0};
    uint8_t bytes[] = {0xdd, 0xcc, 0xbb, 0xaa, 0x44, 0x33, 0x22, 0x11, 0xa0, 0xfe, 0xca, 0xba};
    
    MemIterator<uint8_t> itb(words, ByteOrder::LittleEndian);

    for (size_t i = 0; i < sizeof(bytes); i++)
    {
        CHECK( *itb == bytes[i] );
        itb++;
    }

    MemIterator<uint16_t> ith(words, ByteOrder::LittleEndian);
    uint16_t hwords[] = {0xccdd, 0xaabb, 0x3344, 0x1122, 0xfea0, 0xbaca};

    for (size_t i = 0; i < sizeof(hwords)/sizeof(hwords[0]); i++)
    {
        CHECK( *ith == hwords[i] );
        ith++;
    }

    MemIterator<uint32_t> itw(words, ByteOrder::LittleEndian);
    for (size_t i = 0; i < sizeof(words)/sizeof(words[0]); i++)
    {
        CHECK( *itw == words[i] );
        itw++;
    }
}