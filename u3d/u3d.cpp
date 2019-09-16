// u3d.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "U3d.h"
#include "UCore.h"
#include "Renderer.h"
#include "Controller.h"

#undef main

// Following function was copied from imgui_demo.cpp
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static std::string FindFontFile(const char * fileName)
{
    struct stat fileStat;
    std::string fullPath;

    fullPath = std::string("fonts\\") + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0)
        return std::string(fullPath);

    fullPath = std::string("..\\external\\fonts\\") + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0)
        return std::string(fullPath);

    // File not found
    return std::string("");
}



void U3d::toggleFullScreen()
{
    if (bIsWindowFullScreen) {
        SDL_SetWindowFullscreen(window, 0);
        bIsWindowFullScreen = false;
    }
    else {
        printf("  Changing to fullscreen\n");
        //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        bIsWindowFullScreen = true;
    }
}


void U3d::cleanupAndExitApplication()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void U3d::setWidgetControlMode()
{
    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void U3d::resetWidgetControlMode()
{
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}



void U3d::onKeyDown(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_c:
        _ucore.NavigationLockOntoSun(UCmdParam_Toggle);
        break;
    case SDLK_d:
        _ucore.SetDefaultView();
        break;
    case SDLK_f:
        _ucore.FastForward(UCmdParam_Start);
        break;
    case SDLK_g:
        _ucore.Earth_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_m:
        _ucore.Moon_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_p:
    case SDLK_PAUSE:
    case SDLK_SPACE:
        _ucore.SimulationPause(UCmdParam_Toggle);
        break;
    case SDLK_r:
        _ucore.Rewind(UCmdParam_Start);
        break;
    case SDLK_v:
        _ucore.ChangeSidewaysMotionMode();
        break;
    case SDLK_z:
        _ucore.NavigationLockOntoEarth(UCmdParam_Toggle);
        break;

    case SDLK_0:
        _ucore.Earth_RevolutionMotion(UCmdParam_Toggle);
        break;
    case SDLK_1:
        _ucore.Earth_SetOrbitalPositionAngle(M_PI / 2);
        _ucore.Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_2:
        _ucore.Earth_SetOrbitalPositionAngle(M_PI);
        _ucore.Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_3:
        _ucore.Earth_SetOrbitalPositionAngle(3 * M_PI / 2);
        _ucore.Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_4:
        _ucore.Earth_SetOrbitalPositionAngle(0);
        _ucore.Earth_RevolutionMotion(UCmdParam_Off);
        break;



        // Function keys
    case SDLK_F6:
        if (_ucore.bShiftModifier)
            _ucore.Earth_PrecessionMotion(UCmdParam_Reset);
        else
            _ucore.Earth_PrecessionMotion(UCmdParam_Toggle);
        break;
    case SDLK_F5:
        if (_ucore.bShiftModifier)
            _ucore.Moon_OrbitalPlaneRotation(UCmdParam_Reset);
        else
            _ucore.Moon_OrbitalPlaneRotation(UCmdParam_Toggle);
        break;
    case SDLK_F11:
        toggleFullScreen();
        break;

        // Arrow keys
    case SDLK_UP:
        _ucore._stepMultiplier *= 1.6666;
        break;
    case SDLK_DOWN:
        _ucore._stepMultiplier *= 0.6;
        break;


    case SDLK_HOME:
        _ucore.throttle = _ucore.nominalThrottle;
        break;
    case SDLK_END:
        _ucore.throttle = -_ucore.nominalThrottle;
        break;
    case SDLK_PAGEDOWN:
        _ucore.yaw = _ucore.nominalYaw;
        break;
    case SDLK_DELETE:
        _ucore.yaw = -_ucore.nominalYaw;
        break;
    case SDLK_PAGEUP:
        if (!_ucore.bAltModifier)
            _ucore.pitch = _ucore.nominalPitch;
        else
            _ucore.roll = _ucore.nominalRoll;
        break;
    case SDLK_INSERT:
        if (!_ucore.bAltModifier)
            _ucore.pitch = -_ucore.nominalPitch;
        else
            _ucore.roll = -_ucore.nominalRoll;
        break;

        // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        _ucore.bCtrlModifier = true;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        _ucore.bShiftModifier = true;
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        _ucore.bAltModifier = true;
        break;

    }

}

void U3d::onKeyUp(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_f:
        _ucore.FastForward(UCmdParam_Stop);
        break;
    case SDLK_r:
        _ucore.Rewind(UCmdParam_Stop);
        break;

    case SDLK_HOME:
    case SDLK_END:
        _ucore.throttle = _ucore.noThrottle;
        break;
    case SDLK_PAGEDOWN:
    case SDLK_DELETE:
        _ucore.yaw = _ucore.noYaw;
        break;
    case SDLK_PAGEUP:
    case SDLK_INSERT:
        _ucore.roll = _ucore.noRoll;
        _ucore.pitch = _ucore.noPitch;
        break;


        // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        _ucore.bCtrlModifier = false;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        _ucore.bShiftModifier = false;
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        _ucore.bAltModifier = false;
        break;

    }

}

void U3d::onMouseMotion(int xrel, int yrel)
{
    float dx = xrel;
    float dy = yrel;

    if (_ucore.bCtrlModifier)
    {
        dx /= 15.0f;
        dy /= 15.0f;
    }

    //printf("dx = %d, dy = %d\n", dx, dy);

    if (_ucore.F_REFERENCE_VECTOR_ALONG_Z == 1)
    {
        if (_ucore.bLeftMouseButtonDown) {
            _ucore.space.moveFrame(Movement_Forward, -dy * 5);
            if (_ucore.bLockOntoEarth)
                _ucore.space.rotateFrame(_ucore.earth.getCenter(), dx / 10., 0);
            else if (_ucore.bLockOntoSun)
                _ucore.space.rotateFrame(_ucore.sun.getCenter(), dx / 10., 0);
        }
        else {
            if (_ucore.bLockOntoEarth)
                _ucore.space.rotateFrame(_ucore.earth.getCenter(), dx / 10., -dy / 10.);
            else if (_ucore.bLockOntoSun)
                _ucore.space.rotateFrame(_ucore.sun.getCenter(), dx / 10., -dy / 10.);
        }
        return;
    }


    while (1)
    {
        if (_ucore.bLeftMouseButtonDown) {
            _ucore.space.moveFrame(Movement_Forward, -dy * 5);


            if (_ucore.bSidewaysMotionMode == 1) {
                _ucore.space.moveFrame(Movement_RotateRight, 90);
                _ucore.space.moveFrame(Movement_Forward, dx);
                _ucore.space.moveFrame(Movement_RotateLeft, 90);
            }
            else {
                _ucore.space.moveFrame(Movement_RotateRight, dx / 10.0);
            }
            break;
        }

        if (_ucore.bRightMouseButtonDown) {
            _ucore.space.moveFrame(Movement_RightAlongSD, dx / 10.0);
            break;
        }

        if (_ucore.bSidewaysMotionMode) {
            _ucore.space.moveFrame(Movement_RotateRight, 90);
            _ucore.space.moveFrame(Movement_Forward, dx);
            _ucore.space.moveFrame(Movement_RotateLeft, 90);

            _ucore.space.moveFrame(Movement_RotateUp, 90);
            _ucore.space.moveFrame(Movement_Forward, -dy);
            _ucore.space.moveFrame(Movement_RotateDown, 90);

        }
        else {
            _ucore.space.moveFrame(Movement_RotateRight, (dx / 10.0));
            _ucore.space.moveFrame(Movement_RotateUp, -(dy / 10.0));
        }
        break;
    }
}


void U3d::generateImGuiWidgets()
{
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    // Always showing overlay window showing status of various flags
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(5.0f, io.DisplaySize.y - 5.0f), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Flags", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs))
    {

        ImVec4 onColor = ImColor(0.3f, 0.7f, 0.3f);
        ImVec4 offColor = ImColor(0.0f, 0.0f, 0.0f);
        ImVec4 color;

        if (_ucore.bSidewaysMotionMode) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("V"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (_ucore.bLockOntoEarth) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("Z"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (!_ucore.earth.bRevolutionMotion) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("0"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (_ucore.bSimulationPause) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("Pause"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (_mantle.earthRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("G"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (_mantle.moonRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("M");
        ImGui::PopStyleColor();
    }
    ImGui::End();


    if (_ucore.bMouseGrabbed)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f, 10.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Escape message", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Escape key / double-click to get mouse back");
        }
        ImGui::End();
    }
    else
    {
        // Show menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Universe3d"))
            {
                if (ImGui::MenuItem("Show Fullscreen", "F11"))
                    toggleFullScreen();
                ImGui::MenuItem("Persistent Control Panel", nullptr, &bAlwaysShowControlPanel, true);
                if (ImGui::MenuItem("Exit Application"))
                    bQuit = true;

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (!_ucore.bMouseGrabbed || bAlwaysShowControlPanel)
    {
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 5.0f, 27.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        //ImGui::SetNextWindowSize(ImVec2(250.0f, curHeight - 25.0f));
        ImGui::SetNextWindowBgAlpha(0.8f);

        {
            // Create a window called "Hello, world!" and append into it.
            ImGui::Begin(
                "Control Panel",
                nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
            );
            static float f = 0.0f;
            ImGui::PushFont(appFontSmall);
            if (ImGui::CollapsingHeader("Demos", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushFont(appFontSmall);
                ImGui::Indent();
                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Total Solar Eclipse## demo"))
                    _ucore.ShowDemo(UDemo_TotalSolarEclipse);

                if (ImGui::Button("Total Solar Eclipse on north pole## demo"))
                    _ucore.ShowDemo(UDemo_TotalSolarEclipseOnNorthPole);

                if (ImGui::Button("Annular Solar Eclipse from space## demo"))
                    _ucore.ShowDemo(UDemo_AnnularSolarEclipseFromSpace);
                ImGui::SameLine();
                HelpMarker("Also shows umbra touching the earth");

                if (ImGui::Button("Partial Lunar Eclipse## demo"))
                    _ucore.ShowDemo(UDemo_PartialLunarEclipse);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Tilted orbital planes## demo"))
                    _ucore.ShowDemo(UDemo_TiltedOrbitalPlanes);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Precession motion## demo"))
                    _ucore.ShowDemo(UDemo_PrecessionMotion);

                //ImGui::SetNextItemWidth(-10);
                //ImGui::DragFloat("##float5a", &f);

                //ImGui::SetNextItemWidth(-10);
                //ImGui::DragFloat("##float5b", &f);

                //ImGui::SetNextItemWidth(-10);
                //ImGui::DragFloat("##float5c", &f);

                //ImGui::SetNextItemWidth(-10);
                //ImGui::Text("All the way");
                ImGui::Unindent();
                ImGui::PopFont();
            }
            ImGui::PopFont();
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Earth:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();
            ImGui::Indent();
            ImGui::Checkbox("Revolution motion (0)## earth", &_ucore.earth.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (g)## earth", &_mantle.earthRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Precession motion (F6)## earth", &_ucore.earth.bPrecessionMotion);
            ImGui::SameLine();
            if (ImGui::Button("Reset## earth precession motion"))
                _ucore.Earth_PrecessionMotion(UCmdParam_Reset);
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Moon:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Revolution motion", &_ucore.moon.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (m)##moon", &_mantle.moonRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Orbital plane rotation (F5)", &_ucore.moon.bOrbitalPlaneRotation);
            ImGui::SameLine();
            if (ImGui::Button("Reset## moon orbital plane rotation"))
                _ucore.Moon_OrbitalPlaneRotation(UCmdParam_Reset);
            ImGui::Unindent();
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Navigation:");
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Shift mode (v)", &_ucore.bSidewaysMotionMode);
            ImGui::SameLine(); HelpMarker("When checked, Shift left/right/up/down on mouse movements.\nWhen unchecked, rotate instead.");
            ImGui::Checkbox("Lock on earth's position (z)", &_ucore.bLockOntoEarth);
            if (ImGui::IsItemEdited())
                _ucore.NavigationLockOntoEarth(_ucore.bLockOntoEarth ? UCmdParam_On : UCmdParam_Off);
            ImGui::SameLine(); HelpMarker("Also pauses earth's revolution. Activate this mode and\nthen use mouse to view earth from different angles.");

            ImGui::Checkbox("Lock on sun's position (c)", &_ucore.bLockOntoSun);
            if (ImGui::IsItemEdited())
                _ucore.NavigationLockOntoSun(_ucore.bLockOntoSun ? UCmdParam_On : UCmdParam_Off);

            if (ImGui::Button("Default view (d)"))
                _ucore.SetDefaultView();
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Time:");
            ImGui::PopFont();
            ImGui::Indent();

            ImGui::Checkbox("Time pause (space bar)", &_ucore.bSimulationPause);

            ImGui::Button("Fast Rewind (r)");
            if (ImGui::IsItemActivated())       _ucore.Rewind(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     _ucore.Rewind(UCmdParam_Off);

            ImGui::SameLine();
            ImGui::Button("Fast Forward (f)");
            if (ImGui::IsItemActivated())       _ucore.FastForward(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     _ucore.FastForward(UCmdParam_Off);

            static int timeSpeed = 0;
            ImGui::SetNextItemWidth(100);
            ImGui::Combo("Time Speed", &timeSpeed, "Very Slow\0Slow\0Normal\0Fast\0Very Fast\0\0");
            // todo - actually apply this value in the app

            ImGui::Unindent();

            //-----------------------------------------------------

            ImGui::Separator();
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("S: %.4f, %.4f, %.4f", _ucore.space.S.x, _ucore.space.S.y, _ucore.space.S.z);
            ImGui::Text("D: %.4f, %.4f, %.4f", _ucore.space.D.x, _ucore.space.D.y, _ucore.space.D.z);
            ImGui::Text("E orbital angle: %.4f", _ucore.earth._orbitalAngle);
            ImGui::End();
        }

        // 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::End();
        //}

        // Rendering
    }

    ImGui::Render();
}



/*************************************************************************************************


**************************************************************************************************/
int U3d::run()
{
    setvbuf(stdout, 0, _IONBF, 0);
    const char* glsl_version = "#version 130";

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Universe3d", 100, 100, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    printf("Created SDL GL window\n");
    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    glewInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    std::string fullFontFilePath = FindFontFile("Roboto-Medium.ttf");
    appFontSmall = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 16);
    appFontSmallMedium = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 18);
    appFontMedium = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 20);
    appFontLarge = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 24);
    //if (!font1)
    //    printf("ERROR: Could not load font Cousine-Regular\n");

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    SDL_GetWindowSize(window, &curWidth, &curHeight);
    printf("width = %d\n", curWidth);
    printf("height = %d\n", curHeight);

    printf("initializing scene objects... ");
    _ucore.initSceneObjects();
    _mantle.initializeGL();
    printf("done\n");


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);


    SDL_Event event;
    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type)
            {
            case SDL_QUIT:           bQuit = true;                  break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    setWidgetControlMode();
                    _ucore.bMouseGrabbed = false;
                }
                else
                    onKeyDown(&event);
                break;
            case SDL_KEYUP:
                onKeyUp(&event);
                break;
            case SDL_WINDOWEVENT:
                if ((event.window.event == SDL_WINDOWEVENT_RESIZED) ||
                    (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
                    curWidth = event.window.data1;
                    curHeight = event.window.data2;

                    _mantle.seCurrenttScreenSize(curWidth, curHeight);

                    printf("width = %d\n", curWidth);
                    printf("height = %d\n", curHeight);
                    glViewport(0, 0, curWidth, curHeight);
                }
                break;
            }

            //----------------------------------------------------
            // Pass mouse events to universe3d application only if ImGui isn't using them.
            if (!io.WantCaptureMouse)
            {
                switch (event.type)
                {
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        if (event.button.clicks == 1) {
                            if (!doubleClicked.get() && !_ucore.bMouseGrabbed) {
                                resetWidgetControlMode();
                                _ucore.bMouseGrabbed = true;
                            }
                        }
                        if (event.button.clicks == 2) {
                            if (_ucore.bMouseGrabbed) {
                                doubleClicked.set(50);
                                setWidgetControlMode();
                                _ucore.bMouseGrabbed = false;
                            }
                        }
                        bLeftMouseButtonDown = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:   bLeftMouseButtonDown = false;   break;
                    case SDL_BUTTON_RIGHT:  bRightMouseButtonDown = false;  break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (_ucore.bMouseGrabbed)
                        onMouseMotion(event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_MOUSEWHEEL:
                    if (_ucore.bMouseGrabbed) {
                        // Hack
                        bool oldValue = bLeftMouseButtonDown;
                        bLeftMouseButtonDown = true;
                        onMouseMotion(-event.wheel.x * 10, -event.wheel.y * 10);
                        bLeftMouseButtonDown = oldValue;
                    }
                    break;
                }
            }

            if (bQuit)
                break;

        } // while SDL event poll

        generateImGuiWidgets();

        if (bQuit)
            break;

        doubleClicked.tick();
        _ucore.processFlags();
        _mantle.render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }

    cleanupAndExitApplication();
    return 0;
}

int main()
{
    Universe core;
    Renderer renderer(core);
    Controller controller(core, renderer);
    U3d u3d(core, renderer, controller);
    return u3d.run();
}

