#include "PreRTS.h"
#include "Common/OSDisplay.h"

#include "GameClient/GameText.h"

#include <SDL3/SDL.h>

extern HWND ApplicationHWnd;

OSDisplayButtonType OSDisplayWarningBox(AsciiString p, AsciiString m, UnsignedInt buttonFlags, UnsignedInt otherFlags)
{
  if (!TheGameText)
  {
    return OSDBT_ERROR;
  }

  UnicodeString promptStr = TheGameText->fetch(p);
  UnicodeString mesgStr = TheGameText->fetch(m);

  AsciiString promptA, mesgA;
  promptA.translate(promptStr);
  mesgA.translate(mesgStr);
  // Make sure main window is not TOP_MOST
  int returnResult = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, promptA.str(), mesgA.str(), NULL);
  if (returnResult == 0)
  {
    return OSDBT_OK;
  }

#pragma message("Message Boxes likely not completely implemented")

  return OSDBT_CANCEL;
}
