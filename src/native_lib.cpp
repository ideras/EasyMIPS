#include "native_lib.h"

#ifdef _WIN32

void NativeLib::open(const std::string& lib)
{
    hlib = LoadLibrary(lib.c_str());
}

HFunc NativeLib::getFuncAddr(const std::string& func)
{
    if (hlib == HLIBNULL)
        return nullptr;

    return reinterpret_cast<HFunc>(GetProcAddress(hlib, func.c_str()));
}

void NativeLib::close()
{
    if (hlib != HLIBNULL)
        FreeLibrary(hlib);
}

#else

void NativeLib::open(const std::string& lib)
{
    hlib = dlopen(lib.c_str(), RTLD_LAZY);
}

HFunc NativeLib::getFuncAddr(const std::string& func)
{
    if (hlib == HLIBNULL)
        return nullptr;
    
    return dlsym(hlib, func.c_str());
}

void NativeLib::close()
{
    if (hlib != HLIBNULL)
        dlclose(hlib);
}

#endif
