#include "GameClient/IMEManager.h"

class UnixIMEManager : public IMEManagerInterface
{
public:
  UnixIMEManager()
  {
  }

  virtual ~UnixIMEManager() override
  {
  }

  // SubsystemInterface
  virtual void init(void) override
  {
  }
  virtual void reset(void) override
  {
  }
  virtual void update(void) override
  {
  }

  // IMEManagerInterface

  virtual void attach(GameWindow *window) override
  {
  }
  virtual void detatch(void) override
  {
  }
  virtual void enable(void) override
  {
  }
  virtual void disable(void) override
  {
  }
  virtual Bool isEnabled(void) override
  {
    return false;
  }
  virtual Bool isAttachedTo(GameWindow *window) override
  {
    return false;
  }
  virtual GameWindow *getWindow(void) override
  {
    return NULL;
  }
  virtual Bool isComposing(void) override
  {
    return false;
  }
  virtual void getCompositionString(UnicodeString &string) override
  {
  }
  virtual Int getCompositionCursorPosition(void) override
  {
    return 0;
  }
  virtual Int getIndexBase(void) override
  {
    return 0;
  }

  virtual Int getCandidateCount() override
  {
    return 0;
  }
  virtual UnicodeString *getCandidate(Int index) override
  {
    return NULL;
  }
  virtual Int getSelectedCandidateIndex() override
  {
    return 0;
  }
  virtual Int getCandidatePageSize() override
  {
    return 0;
  }
  virtual Int getCandidatePageStart() override
  {
    return 0;
  }

  /// Returns TRUE if message serviced
  virtual Bool serviceIMEMessage(void *windowsHandle,
                                 UnsignedInt message,
                                 Int wParam,
                                 Int lParam) override
  {
    return false;
  }
  virtual Int result(void) override
  {
    return 0;
  }
};

IMEManagerInterface *TheIMEManager = NULL;
IMEManagerInterface *CreateIMEManagerInterface(void)
{
  return NEW UnixIMEManager;
}
