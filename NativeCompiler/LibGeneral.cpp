#include "NativeCompiler.h"
#include "../Exception/DARICException.h"
#include "../OS/OS.h"
#include "../GUI/Window/Window.h"

void call_END() {
	GetCurrentTask()->TerminateTask();
}

void call_YIELD() {
    GetCurrentTask()->Yield();
}