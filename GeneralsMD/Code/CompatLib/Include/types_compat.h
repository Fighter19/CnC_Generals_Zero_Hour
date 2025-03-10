#pragma once

#include <stdint.h>
#include <stddef.h>

typedef void *HANDLE;
typedef HANDLE HWND;
typedef HANDLE HINSTANCE;
typedef HANDLE HKEY;
typedef int32_t HRESULT;

typedef int BOOL;
typedef unsigned char BYTE;

#define INVALID_HANDLE_VALUE ((HANDLE)-1)

typedef void *LPVOID;

typedef int32_t LONG;
typedef uint32_t ULONG;
typedef uint32_t DWORD;

typedef uint32_t UINT;


typedef int32_t *LPARAM;
typedef size_t WPARAM;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define S_OK 0

// Microsoft direction specifiers, used purely for documentation purposes
#define IN
#define OUT