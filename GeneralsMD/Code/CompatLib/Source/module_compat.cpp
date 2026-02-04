#include "types_compat.h"
#include "module_compat.h"

#include <unistd.h>

#include <dlfcn.h>

#include <string.h>

#include <filesystem>

bool GetModuleFileName(HINSTANCE hInstance, char* buffer, int size)
{
  ssize_t count = readlink("/proc/self/exe", buffer, size);
  if (count == -1)
    return false;
  buffer[count] = '\0';
  return true;
}

HMODULE LoadLibrary(const char* lpFileName)
{
  std::filesystem::path pathFile(lpFileName);
  if (pathFile.extension() == ".dll")
  {
    // Remove extension
    pathFile = pathFile.replace_extension().string();
  }

  void *handle = dlopen(pathFile.c_str(), RTLD_LAZY);
  if (!handle)
  {
    return NULL;
  }

  // Find DllMain and call it if applicable
  typedef BOOL (*DllMainFunc)(HINSTANCE, DWORD, LPVOID);
  DllMainFunc DllMain = (DllMainFunc)dlsym(handle, "DllMain");
  if (DllMain)
  {
    const DWORD DLL_PROCESS_ATTACH = 1;
    if (!DllMain((HINSTANCE)handle, DLL_PROCESS_ATTACH, NULL))
    {
      dlclose(handle);
      return NULL;
    }
  }

  return (HMODULE)handle;
}

FARPROC GetProcAddress(HMODULE hModule, const char* lpProcName)
{
  return (FARPROC)dlsym(hModule, lpProcName);
}

void FreeLibrary(HMODULE hModule)
{
  // Currently misbehaves by unmapping regions not related to the library
  // dlclose(hModule);
}