#include <sstream>
#include "../../Compiler/NativeCompiler/NativeCompiler.h"
#include "../../Compiler/Exception/DARICException.h"
#include "../OS.h"
#include "../../Tasks/System/WindowManager/window/Window.h"

extern uint64_t GetClock();

void call_END()
{
    auto task = GetCurrentTask();

    double t2 = (double)(GetClock() - task->Code.GetT1()) / 100.0;
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << t2;

    // End message and wait for key
    if (task->Code.WaitAtEnd())
    {
        auto window = (Window *)task->GUI.GetWindow();
        auto canvas = window->GetCanvas();
        canvas->SetFG(0xFFFFFF);
        std::string finish_message = "Run time was " + out.str() + " seconds. Press any key to continue.";
        canvas->PrintNewLine();
        canvas->PrintString(finish_message.c_str());
        canvas->PrintNewLine();
        while (task->KeyboardQueueEmpty())
        {
            task->Yield();
        }
    }
    GetCurrentTask()->TerminateTask();
}

void call_YIELD()
{
    GetCurrentTask()->Yield();
}