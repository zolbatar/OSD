#include "InputManager.h"
#include <circle/logger.h>
#include "../../GUI/KeyboardCodes.h"

OSDTask* InputManager::current_task = NULL;

InputManager::InputManager()
{
	this->id = "Input Manager";
	this->name = "Input Manager";
	this->priority = TaskPriority::Low;
}

void InputManager::Run()
{
	SetNameAndAddToList();
	while (1) {
		Yield();
	}
}

void InputManager::KeyDown(uint32_t key)
{
	Key k;
	if (key<=127) {
		k.ascii = key;
		k.keycode = 0;
	}
	else {
		// Convert to upper
		if (key>='a' && key<='z')
			key -= 32;
		k.ascii = 0;
		k.keycode = key;
	}
	k.ro_keycode = KeyRISCOSTranslate(key);
	if (current_task!=NULL)
		current_task->ReceiveKey(k);
//	CLogger::Get()->Write("Input Manager", LogNotice, "%d", key);
}

void InputManager::KeyUp(uint32_t key)
{
}

void InputManager::ClaimInput(OSDTask* task)
{
	current_task = task;
}

int InputManager::KeyRISCOSTranslate(uint32_t key)
{
	switch (key) {
		// Top row
		case KEY_Escape:
			return 112;
		case KEY_F1:
			return 113;
		case KEY_F2:
			return 114;
		case KEY_F3:
			return 115;
		case KEY_F4:
			return 20;
		case KEY_F5:
			return 116;
		case KEY_F6:
			return 117;
		case KEY_F7:
			return 22;
		case KEY_F8:
			return 118;
		case KEY_F9:
			return 119;
		case KEY_F10:
			return 30;
		case KEY_F11:
			return 28;
		case KEY_F12:
			return 29;
//		case SDL_SCANCODE_PRINTSCREEN:
//			return 32;
//		case SDL_SCANCODE_SCROLLLOCK:
//			return 31;
//		case SDL_SCANCODE_PAUSE:
//			return 44;

			// Row 2
		case '`':
			return 45;
		case '1':
			return 48;
		case '2':
			return 49;
		case '3':
			return 17;
		case '4':
			return 18;
		case '5':
			return 19;
		case '6':
			return 52;
		case '7':
			return 36;
		case '8':
			return 21;
		case '9':
			return 38;
		case '0':
			return 39;
		case '-':
			return 23;
		case '=':
			return 93;
		case KEY_Backspace:
			return 47;

			// Row 3
		case KEY_Tab:
			return 96;
		case 'Q':
			return 16;
		case 'W':
			return 33;
		case 'E':
			return 34;
		case 'R':
			return 51;
		case 'T':
			return 35;
		case 'Y':
			return 68;
		case 'U':
			return 53;
		case 'I':
			return 37;
		case 'O':
			return 54;
		case 'P':
			return 55;
		case '[':
			return 56;
		case ']':
			return 88;
//		case SDL_SCANCODE_NONUSBACKSLASH:
//			return 120;

			// Row 4
//		case SDL_SCANCODE_LCTRL:
//			return 1;
		case 'A':
			return 65;
		case 'S':
			return 81;
		case 'D':
			return 50;
		case 'F':
			return 67;
		case 'G':
			return 83;
		case 'H':
			return 84;
		case 'J':
			return 69;
		case 'K':
			return 70;
		case 'L':
			return 86;
		case ';':
			return 72;
		case '\'':
			return 79;
		case '#':
			return 125;
		case 13:
			return 73;

			// Row 4
//		case SDL_SCANCODE_LSHIFT:
//			return 3; // 0
		case 'Z':
			return 97;
		case 'X':
			return 66;
		case 'C':
			return 82;
		case 'V':
			return 99;
		case 'B':
			return 100;
		case 'N':
			return 85;
		case 'M':
			return 101;
		case ',':
			return 102;
		case '.':
			return 103;
		case '/':
			return 104;
//		case SDL_SCANCODE_RSHIFT:
//			return 6; // 0

			// Row 5
//		case SDL_SCANCODE_CAPSLOCK:
//			return 64;
//		case SDL_SCANCODE_LALT:
//			return 2;
			// return 5;
		case ' ':
			return 98;
//		case SDL_SCANCODE_RALT:
//			return 2;
			// return 8;
//		case SDL_SCANCODE_RCTRL:
//			return 1;
			// return 7;

			// Navigation keys
		case KEY_Insert:
			return 61;
		case KEY_Home:
			return 62;
		case KEY_PageUp:
			return 63;
		case KEY_Delete:
			return 89;
		case KEY_End:
			return 105;
		case KEY_PageDown:
			return 78;

			// Cursor keys
		case KEY_Up:
			return 57;
		case KEY_Left:
			return 25;
		case KEY_Down:
			return 41;
		case KEY_Right:
			return 121;

			// Keypad
/*		case SDL_SCANCODE_NUMLOCKCLEAR:
			return 77;
		case SDL_SCANCODE_KP_DIVIDE:
			return 74;
		case SDL_SCANCODE_KP_MULTIPLY:
			return 91;
		case SDL_SCANCODE_KP_7:
			return 27;
		case SDL_SCANCODE_KP_8:
			return 42;
		case SDL_SCANCODE_KP_9:
			return 43;
		case SDL_SCANCODE_KP_MINUS:
			return 59;
		case SDL_SCANCODE_KP_4:
			return 122;
		case SDL_SCANCODE_KP_5:
			return 123;
		case SDL_SCANCODE_KP_6:
			return 26;
		case SDL_SCANCODE_KP_PLUS:
			return 58;
		case SDL_SCANCODE_KP_1:
			return 107;
		case SDL_SCANCODE_KP_2:
			return 123;
		case SDL_SCANCODE_KP_3:
			return 108;
		case SDL_SCANCODE_KP_ENTER:
			return 60;
		case SDL_SCANCODE_KP_0:
			return 106;
		case SDL_SCANCODE_KP_PERIOD:
			return 76;*/
		default:
			return -1;
	}
}