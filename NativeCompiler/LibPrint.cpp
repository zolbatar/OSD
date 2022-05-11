#include "NativeCompiler.h"

static bool tabbed = true;
const int tab_size = 20;

void call_PRINT_NL() {
//    PRINTC("\n");
}

void call_PRINT_Tabbed() {
    tabbed = true;
}

void call_PRINT_TabbedOff() {
    tabbed = false;
}

void call_PRINT_integer(int64_t v) {
/*    if (tabbed)
        PRINTC("%20lld", v);
    else
        PRINTC("%lld", v);*/
    tabbed = false;
}

void call_PRINT_real(double v) {
/*    if (tabbed) {
    	char d[64];
		sprintf(d, "%f", v);
        for (auto i = 0; i < 20 - strlen(d); i++)
            PRINTC(" ");
        PRINTC("%s", d);
    } else
        PRINTC("%f", v);*/
    tabbed = false;
}

void call_PRINT_string(int64_t idx) {
/*    auto v = OS_Strings_Get(idx);
    PRINTC("%s", v.c_str());*/
    tabbed = false;
}

void call_PRINT_SPC(int64_t v) {
/*    for (auto i = 0; i < 4; i++)
        PRINTC(" ");*/
}

void call_PRINT_TAB(int64_t v) {
/*    while (Console_X() + 1 != v) {
        PRINTC(" ");
    }*/
}
