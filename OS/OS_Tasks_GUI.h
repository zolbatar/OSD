#pragma once
#include <OS_Includes.h>

class OSDTaskGUI
{
  public:
    void *GetWindow()
    {
        return w;
    }

    void SetWindow(void *w)
    {
        this->w = w;
    }

    bool fullscreen = false;
    int d_x;
    int d_y;
    int d_w;
    int d_h;
    void *w = NULL;
};