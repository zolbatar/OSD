#include "../OS/OS.h"
#include "GUI.h"

class DARICWindow : public OSDTask {
public:
	DARICWindow(std::string id, std::string name, bool exclusive, int x, int y, int w, int h);
	void Run();
	void SetSourceCode(std::string code);
private:
	std::string code;
	std::string id;
	std::string name;
	int x;
	int y;
	int w;
	int h;
};
