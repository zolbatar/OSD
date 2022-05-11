#include "Input.h"
#include "../Chrono/Chrono.h"
#include <string.h>
#include "../OS/OS.h"

std::queue<Event>* Input::keyEvents;
std::queue<Event>* Input::mouseEvents;

#ifdef CLION
Input::Input()
{
	keyEvents = NEW std::queue<Event>();
	mouseEvents = NEW std::queue<Event>();
}
#else
extern CTimer *timer;
extern int ScreenResX;
extern int ScreenResY;
Input::Input()
{
	keyEvents = NEW std::queue<Event>();
	mouseEvents = NEW std::queue<Event>();
}

void Input::KeyPressedHandler(const char *pString)
{
	Event e;
	if (strlen(pString) == 1)
	{
		e.type = EventType::Text;
		e.ascii = pString[0];
	}
	else
	{
		e.ascii = 0;
		e.type = EventType::KeyDown;
		e.ascii = 0;
		if (strcmp(pString, "\E[[A") == 0) // F1
		{
			e.keyCode = 113;
		}
		else if (strcmp(pString, "\E[[B") == 0) // F2
		{
			e.keyCode = 114;
		}
		else if (strcmp(pString, "\E[[C") == 0) // F3
		{
			e.keyCode = 115;
		}
		else if (strcmp(pString, "\E[[D") == 0) // F4
		{
			e.keyCode = 20;
		}
		else if (strcmp(pString, "\E[[E") == 0) // F5
		{
			e.keyCode = 116;
		}
		else if (strcmp(pString, "\E[17~") == 0) // F6
		{
			e.keyCode = 117;
		}
		else if (strcmp(pString, "\E[18~") == 0) // F7
		{
			e.keyCode = 117;
		}
		else if (strcmp(pString, "\E[19~") == 0) // F8
		{
			e.keyCode = 118;
		}
		else if (strcmp(pString, "\E[20~") == 0) // F9
		{
			e.keyCode = 119;
		}
	}
	keyEvents->push(std::move(e));
}
#endif

#ifdef CLION
void Input::ProcessEvent(SDL_Event& event)
{
	switch (event.type) {
		case SDL_TEXTINPUT: {
			int i = 0;
			while (event.text.text[i]!=0) {
				Event e;
				e.type = EventType::Text;
				e.ascii = event.text.text[i++];
				keyEvents->push(std::move(e));
			}
			break;
		}
		case SDL_KEYDOWN: {
			auto key_code = KeyRISCOSTranslate(event.key.keysym, true);
			if (key_code>0) {
				keyPressed[key_code] = true;
				Event e;
				e.type = EventType::KeyDown;
				e.keyCode = key_code;
				keyEvents->push(std::move(e));
			}
			if (SDL_IsTextInputActive()) {
				if (event.key.keysym.sym<32) {
					switch (event.key.keysym.sym) {
						case 8:
						case 13:
						case 27:
							Event e;
							e.type = EventType::Text;
							e.ascii = event.key.keysym.sym;
							e.keyCode = key_code;
							keyEvents->push(std::move(e));
							break;
					}
				}
			}
			break;
		}
		case SDL_KEYUP: {
			auto key_code = KeyRISCOSTranslate(event.key.keysym, false);
			if (key_code>0) {
				keyPressed[key_code] = false;
				Event e;
				e.type = EventType::KeyUp;
				e.keyCode = key_code;
				keyEvents->push(std::move(e));
			}
			break;
		}
	}
}
#endif

bool Input::CheckForEvent()
{
	return !keyEvents->empty();
}

Event Input::GetEvent()
{
	auto k = keyEvents->front();
	keyEvents->pop();
	return k;
}

#ifdef CLION
int Input::KeyRISCOSTranslate(SDL_Keysym key, bool keydown)
{
	switch (key.scancode) {
		// Top row
		case SDL_SCANCODE_ESCAPE:
			return 112;
		case SDL_SCANCODE_F1:
			return 113;
		case SDL_SCANCODE_F2:
			return 114;
		case SDL_SCANCODE_F3:
			return 115;
		case SDL_SCANCODE_F4:
			return 20;
		case SDL_SCANCODE_F5:
			return 116;
		case SDL_SCANCODE_F6:
			return 117;
		case SDL_SCANCODE_F7:
			return 22;
		case SDL_SCANCODE_F8:
			return 118;
		case SDL_SCANCODE_F9:
			return 119;
		case SDL_SCANCODE_F10: {
			return 30;
		}
		case SDL_SCANCODE_F11:
			return 28;
		case SDL_SCANCODE_F12:
			return 29;
		case SDL_SCANCODE_PRINTSCREEN:
			return 32;
		case SDL_SCANCODE_SCROLLLOCK:
			return 31;
		case SDL_SCANCODE_PAUSE:
			return 44;

			// Row 2
		case SDL_SCANCODE_GRAVE:
			return 45;
		case SDL_SCANCODE_1:
			return 48;
		case SDL_SCANCODE_2:
			return 49;
		case SDL_SCANCODE_3:
			return 17;
		case SDL_SCANCODE_4:
			return 18;
		case SDL_SCANCODE_5:
			return 19;
		case SDL_SCANCODE_6:
			return 52;
		case SDL_SCANCODE_7:
			return 36;
		case SDL_SCANCODE_8:
			return 21;
		case SDL_SCANCODE_9:
			return 38;
		case SDL_SCANCODE_0:
			return 39;
		case SDL_SCANCODE_KP_PLUSMINUS:
			return 23;
		case SDL_SCANCODE_EQUALS:
			return 93;
		case SDL_SCANCODE_BACKSPACE:
			return 47;

			// Row 3
		case SDL_SCANCODE_TAB:
			return 96;
		case SDL_SCANCODE_Q:
			return 16;
		case SDL_SCANCODE_W:
			return 33;
		case SDL_SCANCODE_E:
			return 34;
		case SDL_SCANCODE_R:
			return 51;
		case SDL_SCANCODE_T:
			return 35;
		case SDL_SCANCODE_Y:
			return 68;
		case SDL_SCANCODE_U:
			return 53;
		case SDL_SCANCODE_I:
			return 37;
		case SDL_SCANCODE_O:
			return 54;
		case SDL_SCANCODE_P:
			return 55;
		case SDL_SCANCODE_LEFTBRACKET:
			return 56;
		case SDL_SCANCODE_RIGHTBRACKET:
			return 88;
		case SDL_SCANCODE_NONUSBACKSLASH:
			return 120;

			// Row 4
		case SDL_SCANCODE_LCTRL:
			return 1;
		case SDL_SCANCODE_A:
			return 65;
		case SDL_SCANCODE_S:
			return 81;
		case SDL_SCANCODE_D:
			return 50;
		case SDL_SCANCODE_F:
			return 67;
		case SDL_SCANCODE_G:
			return 83;
		case SDL_SCANCODE_H:
			return 84;
		case SDL_SCANCODE_J:
			return 69;
		case SDL_SCANCODE_K:
			return 70;
		case SDL_SCANCODE_L:
			return 86;
		case SDL_SCANCODE_SEMICOLON:
			return 72;
		case SDL_SCANCODE_APOSTROPHE:
			return 79;
		case SDL_SCANCODE_BACKSLASH:
			return 125;
		case SDL_SCANCODE_RETURN:
			return 73;

			// Row 4
		case SDL_SCANCODE_LSHIFT:
			return 3; // 0
		case SDL_SCANCODE_Z:
			return 97;
		case SDL_SCANCODE_X:
			return 66;
		case SDL_SCANCODE_C:
			return 82;
		case SDL_SCANCODE_V:
			return 99;
		case SDL_SCANCODE_B:
			return 100;
		case SDL_SCANCODE_N:
			return 85;
		case SDL_SCANCODE_M:
			return 101;
		case SDL_SCANCODE_COMMA:
			return 102;
		case SDL_SCANCODE_PERIOD:
			return 103;
		case SDL_SCANCODE_SLASH:
			return 104;
		case SDL_SCANCODE_RSHIFT:
			return 6; // 0

			// Row 5
		case SDL_SCANCODE_CAPSLOCK:
			return 64;
		case SDL_SCANCODE_LALT:
			return 2;
			// return 5;
		case SDL_SCANCODE_SPACE:
			return 98;
		case SDL_SCANCODE_RALT:
			return 2;
			// return 8;
		case SDL_SCANCODE_RCTRL:
			return 1;
			// return 7;

			// Navigation keys
		case SDL_SCANCODE_INSERT:
			return 61;
		case SDL_SCANCODE_HOME:
			return 62;
		case SDL_SCANCODE_PAGEUP:
			return 63;
		case SDL_SCANCODE_DELETE:
			return 89;
		case SDL_SCANCODE_END:
			return 105;
		case SDL_SCANCODE_PAGEDOWN:
			return 78;

			// Cursor keys
		case SDL_SCANCODE_UP:
			return 57;
		case SDL_SCANCODE_LEFT:
			return 25;
		case SDL_SCANCODE_DOWN:
			return 41;
		case SDL_SCANCODE_RIGHT:
			return 121;

			// Keypad
		case SDL_SCANCODE_NUMLOCKCLEAR:
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
			return 76;
		default:
			return -1;
	}
}
#endif

int Input::Get()
{
	while (1) {
#ifdef CLION
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ProcessEvent(event);
		}
#else
		timer->SimpleMsDelay(1);
#endif

		if (!keyEvents->empty()) {
			auto s = keyEvents->front();
			keyEvents->pop();
			if (s.type==EventType::Text) {
				return s.ascii;
			}
		}
	}
}

std::string Input::Gets()
{
	while (1) {
#ifdef CLION
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ProcessEvent(event);
		}
#else
		timer->SimpleMsDelay(1);
#endif
		if (!keyEvents->empty()) {
			auto s = keyEvents->front();
			keyEvents->pop();
			if (s.type==EventType::Text) {
				return std::string(1, s.ascii);
			}
		}
	}
}

int Input::Inkey(int timeout_or_keycode)
{
	if (timeout_or_keycode<=-1) {
		return keyPressed[static_cast<size_t>(-timeout_or_keycode)-1];
	}
	else {
		int clock = GetClock();
		do {
#ifdef CLION
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ProcessEvent(event);
			}
#else
#endif

			// Scan
			while (!keyEvents->empty()) { // Scan until we find a keydown
				auto s = keyEvents->front();
				keyEvents->pop();
				if (s.type==EventType::Text) {
					return s.ascii;
				}
			}
		}
		while (GetClock()-clock<timeout_or_keycode);
		return -1;
	}
}

std::string Input::Inkeys(int timeout_or_keycode)
{
	auto c = Inkey(timeout_or_keycode);
	return std::string(1, static_cast<char>(c));
}

void Input::Mouse(int* x, int* y, int* state)
{
#ifdef CLION
	int xl, yl;
	auto statel = SDL_GetMouseState(&xl, &yl);
	*x = xl;
	*y = yl;
	*state = 0;
	if (statel & SDL_BUTTON(SDL_BUTTON_LEFT))
		*state |= 4;
	if (statel & SDL_BUTTON(SDL_BUTTON_MIDDLE))
		*state |= 2;
	if (statel & SDL_BUTTON(SDL_BUTTON_RIGHT))
		*state |= 1;
#endif
}