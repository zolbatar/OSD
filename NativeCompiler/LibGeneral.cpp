#include <sstream>
#include "NativeCompiler.h"
#include "../Exception/DARICException.h"
#include "../OS/OS.h"
#include "../GUI/Window/Window.h"
extern uint64_t GetClock();

void call_END()
{
	auto task = GetCurrentTask();

	double t2 = (double)(GetClock()-task->GetT1())/100.0;
	std::ostringstream out;
	out.precision(2);
	out << std::fixed << t2;

	// End message and wait for key
	if (task->WaitAtEnd()) {
		auto window = (Window*)task->GetWindow();
		auto canvas = window->GetCanvas();
		std::string finish_message = "Run time was "+out.str()+" seconds. Press any key to continue.";
		canvas->PrintNewLine();
		canvas->PrintString(finish_message.c_str());
		canvas->PrintNewLine();
		while (task->KeyboardQueueEmpty()) {
			task->Yield();
		}
	}
	GetCurrentTask()->TerminateTask();
}

void call_YIELD()
{
	GetCurrentTask()->Yield();
}