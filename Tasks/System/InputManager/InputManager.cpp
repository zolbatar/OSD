#include "InputManager.h"
#include <circle/logger.h>
#include "../../GUI/KeyboardCodes.h"

OSDTask *InputManager::current_task = NULL;
std::map<uint8_t, uint32_t> InputManager::lv_to_ascii;
std::map<uint8_t, uint32_t> InputManager::lv_to_ascii_shift;
std::map<uint8_t, uint32_t> InputManager::lv_to_ro;

InputManager::InputManager()
{
    this->id = "Input Manager";
    this->name = "Input Manager";
    this->priority = TaskPriority::Low;

    // LV to ascii
    AddKeyS(4, 'A', 'a', 65);
    AddKeyS(5, 'B', 'b', 100);
    AddKeyS(6, 'C', 'c', 82);
    AddKeyS(7, 'D', 'd', 50);
    AddKeyS(8, 'E', 'e', 34);
    AddKeyS(9, 'F', 'f', 67);
    AddKeyS(10, 'G', 'g', 83);
    AddKeyS(11, 'H', 'h', 84);
    AddKeyS(12, 'I', 'i', 37);
    AddKeyS(13, 'J', 'j', 69);
    AddKeyS(14, 'K', 'k', 70);
    AddKeyS(15, 'L', 'l', 86);
    AddKeyS(16, 'M', 'm', 101);
    AddKeyS(17, 'N', 'n', 85);
    AddKeyS(18, 'O', 'o', 54);
    AddKeyS(19, 'P', 'p', 55);
    AddKeyS(20, 'Q', 'q', 16);
    AddKeyS(21, 'R', 'r', 51);
    AddKeyS(22, 'S', 's', 81);
    AddKeyS(23, 'T', 't', 35);
    AddKeyS(24, 'U', 'u', 53);
    AddKeyS(25, 'V', 'v', 99);
    AddKeyS(26, 'W', 'w', 33);
    AddKeyS(27, 'X', 'x', 66);
    AddKeyS(28, 'Y', 'y', 68);
    AddKeyS(29, 'Z', 'z', 97);
    AddKeyS(30, '1', '!', 48);
    AddKeyS(31, '2', '"', 49);
    AddKeyS(32, '3', '£', 17);
    AddKeyS(33, '4', '$', 18);
    AddKeyS(34, '5', '%', 19);
    AddKeyS(35, '6', '^', 52);
    AddKeyS(36, '7', '&', 36);
    AddKeyS(37, '8', '*', 21);
    AddKeyS(38, '9', '(', 38);
    AddKeyS(39, '0', ')', 39);
    AddKey(40, KEY_Return, 73);
    AddKey(42, KEY_Backspace, 47);
    AddKey(43, KEY_Tab, 96);
    AddKeyS(45, '-', '_', 23);
    AddKeyS(46, '=', '+', 93);
    AddKeyS(47, '[', '{', 56);
    AddKeyS(48, ']', '}', 88);
    AddKeyS(49, '#', '~', 120);
    AddKeyS(51, ';', ':', 87);
    AddKeyS(52, '\'', '@', 79);
    AddKey(53, '`', 45);
    AddKey(57, KEY_CapsLock, 64);
    AddKey(58, KEY_F1, 113);
    AddKey(59, KEY_F2, 114);
    AddKey(60, KEY_F3, 115);
    AddKey(61, KEY_F4, 20);
    AddKey(62, KEY_F5, 116);
    AddKey(63, KEY_F6, 117);
    AddKey(64, KEY_F7, 22);
    AddKey(65, KEY_F8, 118);
    AddKey(66, KEY_F9, 119);
    AddKey(67, KEY_F10, 30);
    AddKey(68, KEY_F11, 28);
    AddKey(69, KEY_F12, 29);
    AddKeyS(100, '\\', '|', 94);
}

void InputManager::AddKeyS(uint8_t lv, uint32_t key, uint32_t shift_key, uint8_t ro)
{
    lv_to_ascii.insert(std::make_pair(lv, key));
}

void InputManager::AddKey(uint8_t lv, uint32_t key, uint8_t ro)
{
    lv_to_ascii.insert(std::make_pair(lv, key));
}

void InputManager::Run()
{
    SetNameAndAddToList();
    while (1)
    {
        Yield();
    }
}

Key InputManager::GetKeyInfo(uint8_t lv)
{
    Key k;
    k.lv_keycode = lv;

    // Ascii
    auto af = lv_to_ascii.find(lv);
    if (af != lv_to_ascii.end())
    {
        k.ascii = af->second;
    }

    // RO
    auto rf = lv_to_ro.find(lv);
    if (rf != lv_to_ro.end())
    {
        k.ro_keycode = af->second;
    }

    return k;
}

void InputManager::KeyDown(uint8_t lv)
{
    /*    Key k;
        if (key <= 127)
        {
            k.ascii = key;
            k.keycode = 0;
        }
        else
        {
            // Convert to upper
            if (key >= 'a' && key <= 'z')
                key -= 32;
            k.ascii = 0;
            k.keycode = key;
        }
        //    k.ro_keycode = KeyRISCOSTranslate(key);
        if (current_task != NULL)
            current_task->ReceiveKey(k);
        //	CLogger::Get()->Write("Input Manager", LogNotice, "%d", key);*/
}

void InputManager::KeyUp(uint8_t lv)
{
}

void InputManager::ClaimInput(OSDTask *task)
{
    current_task = task;
}