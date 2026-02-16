#ifndef __NATIVE_LIB_H__
#define __NATIVE_LIB_H__

#ifdef _WIN32
#include <windows.h>

using HLib = HINSTANCE;

#else
// Assume Unix system
#include <dlfcn.h>

using HLib = void*;

#endif

#include <string>

using HFunc = void*;
#define HLIBNULL nullptr

class NativeLib
{
public:
    NativeLib(): hlib(HLIBNULL)
    {}
    
    NativeLib(const std::string& lib)
    { open(lib); }

    ~NativeLib()
    { close(); }

    void open(const std::string& lib);
    
    bool isOpen()
    { return hlib != HLIBNULL; }

    HFunc getFuncAddr(const std::string& func);

    void close();
private:
    HLib hlib;
};

#endif
