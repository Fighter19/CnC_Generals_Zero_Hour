#include "GameClient/IMEManager.h"
#include "GameClient/GameWindowManager.h"
#include "GameClient/GameWindow.h"

class SDL3IMEManager : public IMEManagerInterface
{
public:
  SDL3IMEManager()
  {
  }

  virtual ~SDL3IMEManager() override
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
    m_window = window;
  }
  virtual void detatch(void) override
  {
    m_window = nullptr;
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
    return window == m_window;
  }
  virtual GameWindow *getWindow(void) override
  {
    return m_window;
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

private:
  GameWindow *m_window = nullptr;
};

IMEManagerInterface *CreateIMEManagerInterface(void)
{
  return NEW SDL3IMEManager;
}
