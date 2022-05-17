#pragma once
#include "../OS/OS.h"

class Kernel : public OSDTask {
public:
	Kernel();
	void Run();
};
