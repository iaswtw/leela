#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include "UCore.h"
#include "Utils.h"
#include <stdio.h>

#include "Mantle.h"

class U3d
{
public:
    U3d(Universe& ucore, Mantle& mantle) :
        _ucore(ucore), _mantle(mantle)
    {
    }

    ~U3d()
    {
    }

    int run();
    void generateImGuiWidgets();
    void toggleFullScreen();
    void cleanupAndExitApplication();

    void setWidgetControlMode();
    void resetWidgetControlMode();

    void onKeyDown(SDL_Event* event);
    void onKeyUp(SDL_Event* event);
    void onMouseMotion(int xrel, int yrel);

private:
    Universe& _ucore;
    Mantle& _mantle;

    bool bQuit = false;

    //=================================================================
    // IMGUI, SDL related
    SDL_Window *window = nullptr;
    SDL_GLContext context;

    int curWidth = 0;
    int curHeight = 0;

    bool bAlwaysShowControlPanel = true;
    bool bIsWindowFullScreen = false;
    bool bShowFlagsOverlay = true;

    ImFont *appFontSmall = nullptr;
    ImFont *appFontSmallMedium = nullptr;
    ImFont *appFontMedium = nullptr;
    ImFont *appFontLarge = nullptr;

    OneShotBoolean doubleClicked = OneShotBoolean();

};