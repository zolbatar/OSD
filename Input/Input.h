#pragma once
#ifdef CLION
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#else
#include <circle/usb/usbmouse.h>
#include <circle/usb/usbkeyboard.h>
#endif
#include <queue>
#include <array>
#include <string>

enum class EventType {
	Text,
	KeyDown,
	KeyUp
};

struct Event {
	EventType type;
	int keyCode;
	char ascii;
};

class Input {
public:
	Input();
#ifdef CLION
	void ProcessEvent(SDL_Event& event);
#endif
	bool CheckForEvent();
	Event GetEvent();
	int Get();
	std::string Gets();
	int Inkey(int timeout_or_keycode);
	std::string Inkeys(int timeout_or_keycode);
	void Mouse(int* x, int* y, int* state);

private:
#ifdef CLION
	int KeyRISCOSTranslate(SDL_Keysym key, bool keydown);
#else
	static void KeyPressedHandler(const char *pString);
#endif
	static std::queue<Event>* keyEvents;
	static std::queue<Event>* mouseEvents;
	std::array<bool, 256> keyPressed;
};
