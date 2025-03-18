#include "types_compat.h"
#include "wnd_compat.h"

DWORD GetWindowLong(HWND hWnd, int nIndex)
{
  return 0;
}

void AdjustWindowRect(RECT *pRect, DWORD dwStyle, BOOL bMenu)
{

}

BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
  return TRUE;
}

void SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{

}

void GetWindowRect(HWND hWnd, RECT *pRect)
{

}

void GetClientRect(HWND hWnd, RECT *pRect)
{

}

HWND GetDesktopWindow() {
  return NULL;
}

HDC GetDC(HWND hWnd) {
  return NULL;
}

int ReleaseDC(HWND hWnd, HDC hDC) 
{
  return 0;
}

void SetDeviceGammaRamp(HDC hDC, LPVOID lpRamp)
{

}

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
  return 0;
}

void SetCursor(void *hCursor)
{

}

void GetCursorPos(struct POINT *ptCursor) {

}

void ScreenToClient(HWND hWnd, struct POINT *ptCursor)
{

}

void ClientToScreen(HWND hWnd, struct POINT *ptCursor)
{

}