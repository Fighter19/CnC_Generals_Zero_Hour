#pragma once


#define GWL_STYLE 1


DWORD GetWindowLong(HWND hWnd, int nIndex);
void AdjustWindowRect(RECT *pRect, DWORD dwStyle, BOOL bMenu);

typedef enum eSetWindowPosFlags
{
  SWP_NOSIZE = 0x0001,
  SWP_NOMOVE = 0x0002,
  SWP_NOZORDER = 0x0004,
} eSetWindowPosFlags;

#define HWND_TOPMOST ((HWND)-1)

void SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
void GetWindowRect(HWND hWnd, RECT *pRect);

void GetClientRect(HWND hWnd, RECT *pRect);

HWND GetDesktopWindow();
HDC GetDC(HWND hWnd);
int ReleaseDC(HWND hWnd, HDC hDC);

void SetDeviceGammaRamp(HDC hDC, LPVOID lpRamp);

typedef enum eMessageBoxType
{
  MB_OK
} eMessageBoxType;

void MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

void SetCursor(void *hCursor);