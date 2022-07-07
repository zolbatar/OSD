#pragma once
#include <map>
#include <circle/input/keymap.h>
#include "InputManagerStruct.h"
#include "../WindowManager/window/Window.h"
#include "../WindowManager/lvgl/lvgl.h"

class InputManager : public OSDTask
{
  public:
    InputManager();
    void Run();
    static void KeyDown(Key ki);
    static void KeyUp(uint8_t lv);
    static void ClaimInput(OSDTask *task);
    static OSDTask *GetInput()
    {
        return current_task;
    }
    static void ProcessModifiers(uint8_t modifiers);
    static Key GetKeyInfo(uint8_t lv, uint8_t modifiers);

    static bool ShiftDown;
    static bool CtrlDown;
    static bool AltDown;

  private:
    static CKeyMap keymap;
    static OSDTask *current_task;
    static std::map<uint8_t, uint32_t> lv_to_ro;
    static void AddKey(uint8_t lv, uint8_t ro);
};
