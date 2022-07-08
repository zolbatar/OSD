#include "InputManager.h"
#include <circle/logger.h>

OSDTask *InputManager::current_task = NULL;
std::map<uint8_t, uint32_t> InputManager::lv_to_ro;
CKeyMap InputManager::keymap;
bool InputManager::ShiftDown = false;
bool InputManager::CtrlDown = false;
bool InputManager::AltDown = false;

InputManager::InputManager()
{
    this->id = "Input Manager";
    this->SetName("Input Manager");
    this->priority = TaskPriority::System;

    // LV to RISC OS keycode
    AddKey(4, 65);
    AddKey(5, 100);
    AddKey(6, 82);
    AddKey(7, 50);
    AddKey(8, 34);
    AddKey(9, 67);
    AddKey(10, 83);
    AddKey(11, 84);
    AddKey(12, 37);
    AddKey(13, 69);
    AddKey(14, 70);
    AddKey(15, 86);
    AddKey(16, 101);
    AddKey(17, 85);
    AddKey(18, 54);
    AddKey(19, 55);
    AddKey(20, 16);
    AddKey(21, 51);
    AddKey(22, 81);
    AddKey(23, 35);
    AddKey(24, 53);
    AddKey(25, 99);
    AddKey(26, 33);
    AddKey(27, 66);
    AddKey(28, 68);
    AddKey(29, 97);
    AddKey(30, 48);
    AddKey(31, 49);
    AddKey(32, 17);
    AddKey(33, 18);
    AddKey(34, 19);
    AddKey(35, 52);
    AddKey(36, 36);
    AddKey(37, 21);
    AddKey(38, 38);
    AddKey(39, 39);
    AddKey(40, 73);
    AddKey(42, 47);
    AddKey(43, 96);
    AddKey(45, 23);
    AddKey(46, 93);
    AddKey(47, 56);
    AddKey(48, 88);
    AddKey(49, 120);
    AddKey(51, 87);
    AddKey(52, 79);
    AddKey(53, 45);
    AddKey(57, 64);
    AddKey(58, 113);
    AddKey(59, 114);
    AddKey(60, 115);
    AddKey(61, 20);
    AddKey(62, 116);
    AddKey(63, 117);
    AddKey(64, 22);
    AddKey(65, 118);
    AddKey(66, 119);
    AddKey(67, 30);
    AddKey(68, 28);
    AddKey(69, 29);
}

void InputManager::AddKey(uint8_t lv, uint8_t ro)
{
    lv_to_ro.insert(std::make_pair(lv, ro));
}

void InputManager::Run()
{
    SetNameAndAddToList();
    while (1)
    {
        Yield();
    }
}

void InputManager::ProcessModifiers(uint8_t modifiers)
{
    ShiftDown = modifiers & (KEY_LSHIFT_MASK | KEY_RSHIFT_MASK);
    CtrlDown = modifiers & (KEY_LCTRL_MASK | KEY_RCTRL_MASK);
    AltDown = modifiers & (KEY_ALT_MASK | KEY_ALTGR_MASK);
}

Key InputManager::GetKeyInfo(uint8_t lv, uint8_t modifiers)
{
    Key k;
    k.lv_keycode = lv;

    // Keymap
    k.keycode = keymap.Translate(lv, modifiers);
    if (k.keycode <= 255)
    {
        k.ascii = k.keycode & 0xFF;
    }
    else
    {
        k.ascii = 0;
    }
    //    CLogger::Get()->Write("lvgl", LogNotice, "%d '%s'", k.keycode, ks);

    // RO
    auto rf = lv_to_ro.find(lv);
    if (rf != lv_to_ro.end())
    {
        k.ro_keycode = rf->second;
    }

    return k;
}

void InputManager::KeyDown(Key ki)
{
    if (current_task != NULL)
        current_task->ReceiveKey(ki);
}

void InputManager::KeyUp(uint8_t lv)
{
}

void InputManager::ClaimInput(OSDTask *task)
{
    current_task = task;
}