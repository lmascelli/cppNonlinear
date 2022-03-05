#include "nonlinear.hpp"

#ifdef WINDOWS
#define DECL_DLL __declspec(dllexport)
#else
#define DECL_DLL
#endif

extern "C"
{
    SystemDescriptor DECL_DLL *getSystemDescriptor();
}