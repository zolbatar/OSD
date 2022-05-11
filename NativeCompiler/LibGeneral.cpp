#include "NativeCompiler.h"
#include "../Exception/DARICException.h"
#include "../OS/OS.h"
#include "../GUI/Window.h"

void call_END() {
    GetCurrentTask()->SendGUIMessage(Messages::WM_CloseWindow, NULL);
    Window *w;
    do {
        GetCurrentTask()->Yield();
        w = (Window *) GetCurrentTask()->GetWindow();
    } while (w != NULL);
    GetCurrentTask()->Terminate();
}

void call_YIELD() {
    GetCurrentTask()->Yield();
}