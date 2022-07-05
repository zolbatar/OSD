#pragma once
#include <map>
#include "InputManagerStruct.h"
#include "../../GUI/Window/Window.h"
#include "../../GUI/lvgl.h"

class InputManager : public OSDTask
{
  public:
    InputManager();
    void Run();
    static void KeyDown(uint8_t lv);
    static void KeyUp(uint8_t lv);
    static void ClaimInput(OSDTask *task);
    static Key GetKeyInfo(uint8_t lv);

  private:
    static OSDTask *current_task;
    static std::map<uint8_t, uint32_t> lv_to_ascii;
    static std::map<uint8_t, uint32_t> lv_to_ascii_shift;
    static std::map<uint8_t, uint32_t> lv_to_ro;
    static void AddKeyS(uint8_t lv, uint32_t key, uint32_t shift_key, uint8_t ro);
    static void AddKey(uint8_t lv, uint32_t key, uint8_t ro);
};
