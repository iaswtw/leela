#include "pch.h"
#include "Universe.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


/*************************************************************************************************


**************************************************************************************************/
void Universe::initializeGL()
{
    printf("Inside initializeGL\n");

    std::string vertFilename("../ucore/shaders/planet.vert.glsl");
    std::string fragFilename("../ucore/shaders/planet.frag.glsl");
    planetGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
    planetGlslProgram.link();


    vertFilename = "../ucore/shaders/sun.vert.glsl";
    fragFilename = "../ucore/shaders/sun.frag.glsl";
    sunGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
    sunGlslProgram.link();

    vertFilename = "../ucore/shaders/star.vert.glsl";
    fragFilename = "../ucore/shaders/star.frag.glsl";
    starGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
    starGlslProgram.link();

    vertFilename = "../ucore/shaders/simple.vert.glsl";
    fragFilename = "../ucore/shaders/simple.frag.glsl";
    simpleGlslProgram.compileShadersFromFile(vertFilename, fragFilename);
    simpleGlslProgram.link();


    //---------------------------------------------------------------------------------------------------
    // Axis
    axisRenderer.constructVerticesAndSendToGpu();
    starsRenderer.constructVerticesAndSendToGpu();

    //---------------------------------------------------------------------------------------------------


    sunRenderer.setAsLightSource();
    sunRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    sunRenderer.constructVerticesAndSendToGpu();


    earthRenderer.setPolygonCountLevel(PolygonCountLevel_High);
    earthRenderer.constructVerticesAndSendToGpu();
    earthRenderer.bShowLatitudesAndLongitudes = true;
    earthRenderer.setNightColorDarkness(NightColorDarkness_Medium);


    moonRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    moonRenderer.constructVerticesAndSendToGpu();
    moonRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);

    glBindVertexArray(0);       // Disable VBO
}


/*************************************************************************************************


**************************************************************************************************/
void Universe::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //=====================================================================================
    // Create View and projection that remain the same for the entire scene
    //=====================================================================================
    // View transformation
    //----------------------------------------------
    // Create the initial View matrix
    viewMatrix = glm::lookAt(
        space.getSourcePoint(),
        space.getDirectionPoint(),
        space.getUpwardDirectionVector());

    // perspective transformation
    //----------------------------------------------
    projectionMatrix = glm::perspective(
        glm::radians(35.0f),
        float(curWidth) / float(curHeight),
        1.0f,
        10000000.0f);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    planetGlslProgram.use();
    planetGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    planetGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    renderUsingPlanetGlslProgram();

    planetGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    starGlslProgram.use();
    printGlError();
    starGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    starGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    renderUsingStarGlslProgram();

    starGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    sunGlslProgram.use();
    sunGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    sunGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    renderUsingSunGlslProgram();

    sunGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    simpleGlslProgram.use();
    simpleGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    simpleGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    renderUsingSimpleGlslProgram();

    simpleGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    glBindVertexArray(0);

}

void Universe::renderUsingPlanetGlslProgram()
{
    planetGlslProgram.setVec3("sunCenterTransformed", glm::value_ptr(sun.getModelTransformedCenter()));
    planetGlslProgram.setFloat("sunRadius", sun.getRadius());
    planetGlslProgram.setBool("realisticShading", bRealisticShading);

    earthRenderer.renderSphere(planetGlslProgram, &sun, &moon);
    earthRenderer.renderLatitudeAndLongitudes(planetGlslProgram);

    moonRenderer.renderSphere(planetGlslProgram, &sun, &earth);
    moonRenderer.renderLatitudeAndLongitudes(planetGlslProgram);

}

void Universe::renderUsingStarGlslProgram()
{
    if (!bGalaxyStars)
    {
        starsRenderer.renderCubeStars(starGlslProgram);
    }
    else
    {
        starsRenderer.renderGalaxyStars(starGlslProgram);
    }
}

void Universe::renderUsingSunGlslProgram()
{
    sunRenderer.renderSphere(sunGlslProgram);
}

void Universe::renderUsingSimpleGlslProgram()
{
    if (bShowAxis)
    {
        axisRenderer.render(simpleGlslProgram);
    }

    earthRenderer.renderOrbitalPlane(simpleGlslProgram);
    if (bShowOrbitsGlobalEnable)
        earthRenderer.renderOrbit(simpleGlslProgram);

    moonRenderer.renderOrbitalPlane(simpleGlslProgram);
    if (bShowOrbitsGlobalEnable)
        moonRenderer.renderOrbit(simpleGlslProgram);
}


void Universe::generateImGuiWidgets()
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

        if (bSidewaysMotionMode) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("v"); ImGui::SameLine();
        ImGui::PopStyleColor();

        //if (bLockOntoEarth) color = onColor; else color = offColor;
        //ImGui::PushStyleColor(ImGuiCol_Button, color);
        //ImGui::Button("z"); ImGui::SameLine();
        //ImGui::PopStyleColor();

        if (!earth.bRevolutionMotion) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("0"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bSimulationPause) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("Pause"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (earthRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("e"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (moonRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("m"); ImGui::SameLine();
        ImGui::PopStyleColor();

        //if (bEarthFollowMode) color = onColor; else color = offColor;
        //ImGui::PushStyleColor(ImGuiCol_Button, color);
        //ImGui::Button("b");
        //ImGui::PopStyleColor();

    }
    ImGui::End();

    if (bShowIntroduction)
    {
        float width = 800.0f;
        float height = 600.0f;
        //float x = io.DisplaySize.x - (width / 2);
        //float y = io.DisplaySize.y - (height / 2);

        ImGui::SetNextWindowSizeConstraints(ImVec2(width, height), ImVec2(width, height));
        //ImGui::SetNextWindowPos(ImVec2(, y), 0, ImVec2(0.5, 0.5));
        //ImGui::SetCursorPos(ImVec2(io.DisplaySize.x / 2.0f, io.DisplaySize.y / 2.0f));
        if (ImGui::Begin("Introduction", &bShowIntroduction))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));

            ImGui::PushFont(appFontGiant);
            ImGui::TextWrapped("Universe3d\n\n");
            ImGui::PopFont();

            //ImGui::PushFont(appFontMedium);
            ImGui::PushFont(appFontExtraSmall);
            ImGui::TextWrapped(
                "This program is intended to be a teaching aid to explain various celestial concepts. "
                "It primarily intends to explain phenomenon related to orbital mechanics of planetary motion. \n\n"

                "The program doesn't make any effort to show sizes, distances, rotation & revolution speeds, etc. to scale. "
                "If it did, it will hinder its ability to help explain concepts due to the impracticality of showing the earth, moon and sun on the screen at "
                "the same time. They will only be a few pixels wide.\n\n"

                "With distances and sizes not to scale, angles have to be exagerated to show the same effect.  For example, to show why solar eclipses don't occurr on every "
                "new moon day, the tilt of the moons's orbital plane with respect to earth's orbital plane had to be exagerated to roughly 30 degrees from about 5 degrees."

            );
            ImGui::PopFont();

            ImGui::PopStyleColor();
        }
        ImGui::End();
    }

    if (bShowKeyboardShortcuts)
    {
        //ImGuiCond_FirstUseEver
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(1024, 768));
        if (ImGui::Begin("Keyboard Shortcuts", &bShowKeyboardShortcuts))
        {
            auto populateShortcutWindow = [](const char* arr[][2], int numRows) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));
                for (int i = 0; i < numRows; i++)
                {
                    if (arr[i][0] == nullptr) {
                        ImGui::Separator();
                    }
                    else {
                        ImGui::Text(arr[i][0]);  ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 100);
                        ImGui::TextWrapped(arr[i][1]);  ImGui::NextColumn();
                    }
                }
                ImGui::PopStyleColor();
            };

            const char* shortcutHelp[][2] = {
            { "Space bar",      "Pause time. Navigation will still work." },
            { "f",              "Pressing this key and keeping it pressed will cause simulation time to elapse faster by about 10 times."},
            { "r",              "Pressing this key and keeping it pressed with cause simulation time to run in reverse by about 10 times the nominal forward speed. "
                                "Releasing this key shall restore the forward movement of time at the speed it was just before pressing this key."},
            { "Right arrow",    "Speeds up the simulation time passage by roughly 66% each time this key is pressed."},
            { "Left arrow",     "Slows down the simulation time passage by roughly 66% each time this key is pressed."},
            { "d",              "Bring the camera to the default position and look in the direction of the sun. From this position, the entire earth's orbit is visible. "
                                " It is a convenient starting position for the simulation.  If you get lost navigating, you can hit this key to get your bearings."},
            { nullptr, nullptr },

            { "a",              "Toggle visibility of the XYZ coordinate axis.  When looking from the default view position, +ve X direction is towards bottom left from origin (blue),"
                                "+ve Y is towards bottom right from origin (green), and +ve Z is upwards from origin (cyan)." },
            { "e",              "Toggle visibility of earth's orbital plane." },
            { "m",              "Toggle visibility of moon's orbital plane." },

            { "z",              "Toggle lock on earth's position. When turned on, the earth will appear at the center of the screen. Left/right/up/down mouse movements shall rotate the camera around the earth at a constant distance. "
                                "Zoom in and zoom out will work as expected.  Turning lock on will also cause the earth to pause in its orbit." },
            { "c",              "Toggle lock on sun's position. When turned on, the sun will appear at the center of the screen." },
            { "b",              "Toggle directional lock on earth. The direction vector from the camera to the earth will be noted at the time of turning this mode on. "
                                "After that, the camera will follow the earth as the earth moves in its orbit, all the while maintaining the original direction vector."
                                "This mode is used in the 6 month long day and night demo.  Use this mode when you want to observe the earth from a fixed angle as it "
                                "revolves around the sun.  Contrast this mode with the normal lock on earth (z) mode where the camera stays put while only changing its viewing direction. "
                                "While in this mode, only zoom in/out navigation will work." },
            { "0 (zero)",       "Toggle earth's revolution motion.  Use this to pause the earth in its orbit so that you can observe various things such as the tilted orbit, shadows, etc." },


            { nullptr, nullptr },

            { "Home",           "Zoom in" },
            { "End",            "Zoom out" },
            { "Del",            "Turn left (or shift left in Shift mode)"},
            { "Page Down",      "Turn right (or shift right in Shift mode)"},
            { "Page Up",        "Rotate up (or shift up in Shift mode)"},
            { "Insert",         "Rotate doen (or shift down in Shift mode"},
            { "Alt + Page Up",  "Rotate right along the axis the camera is looking at. This will result in the object in front being rotated left."},
            { "Alt + Insert",   "Rotate left along the axis the camera is looking at. This will result in the object in front being rotated right."},
            { "v",              "Toggle 'Shift mode' navigation.  In this mode, left/right/up/down mouse movements will result in the camera being shifted in those directions as "
                                "as opposed to rotating in those directions.  When the camera shifts, the viewing direction vector does not change. It is as if the camera is looking at a point at infinity. "
                                "If you want to look at how the background stars shift due to parallex as the earth moves in its orbit, turn this mode on, go to default "
                                "view (d), and then move the mouse left and right." },


            { nullptr, nullptr },

            { "F5",             "Start/Stop rotation of moon's orbital plane. This is best seen if moon's orbital plane is also visible." },
            { "Shift + F5",     "Reset moon's orbital plane tilt to default." },

            { "F6",             "Start/Stop earth's precession motion." },
            { "Shift + F6",     "Reset earth's axis tilt direction to default." },

            { nullptr, nullptr },

            { "1",              "Set earth's position at 0 degrees measured from the positive X axis (blue)." },
            { "2",              "Set earth's position at 90 degrees measured from the positive X axis (blue)." },
            { "3",              "Set earth's position at 180 degrees measured from the positive X axis (blue)." },
            { "4",              "Set earth's position at 270 degrees measured from the positive X axis (blue)." },

            {nullptr, nullptr },

            };

            ImGui::Columns(2, "mycolumn");
            ImGui::Text("Shortcut"); ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 100);
            ImGui::Text("Description"); ImGui::NextColumn();
            ImGui::Separator();

            populateShortcutWindow(shortcutHelp, sizeof(shortcutHelp) / sizeof(shortcutHelp[0]));
        }
        ImGui::End();

    }


    if (bMouseGrabbed)
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
                if (ImGui::MenuItem("Exit Application"))
                    bQuit = true;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Fullscreen", "F11"))
                    toggleFullScreen();
                ImGui::MenuItem("Show Control Panel in Navigation mode", nullptr, &bAlwaysShowControlPanel, true);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("Introduction", nullptr, &bShowIntroduction);
                ImGui::MenuItem("Show Keyboard Shortcuts", nullptr, &bShowKeyboardShortcuts);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (!bMouseGrabbed || bAlwaysShowControlPanel)
    {
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 5.0f, 27.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        //ImGui::SetNextWindowSize(ImVec2(350.0f, curHeight - 25.0f));
        ImGui::SetNextWindowBgAlpha(0.8f);

        {
            // Create a window called "Hello, world!" and append into it.
            ImGui::Begin(
                "Control Panel",
                nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
            );
            static float f = 0.0f;
            ImGui::PushFont(appFontSmall);
            if (ImGui::CollapsingHeader("Demos", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushFont(appFontExtraSmall);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Total Solar Eclipse## demo"))
                    ShowDemo(UDemo_TotalSolarEclipse);

                if (ImGui::Button("Total Solar Eclipse on north pole## demo"))
                    ShowDemo(UDemo_TotalSolarEclipseOnNorthPole);

                if (ImGui::Button("Annular Solar Eclipse from space## demo"))
                    ShowDemo(UDemo_AnnularSolarEclipseFromSpace);
                ImGui::SameLine();
                HelpMarker("This also shows umbra starting to travel over the earth near the bottom left of the screen. This happens roughly halfway between 3rd and 4th contact for a duration of about 2 to 3 seconds. So watch out!");

                if (ImGui::Button("Partial Lunar Eclipse## demo"))
                    ShowDemo(UDemo_PartialLunarEclipse);

                if (ImGui::Button("Tilted orbital planes## demo"))
                    ShowDemo(UDemo_TiltedOrbitalPlanes);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole (another angle)"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle);

                if (ImGui::Button("Precession motion## demo"))
                    ShowDemo(UDemo_PrecessionMotion);

                ImGui::PopFont();
            }
            ImGui::PopFont();
            ImGui::Separator();

            //-----------------------------------------------------
            // Common font for the content of all the below sections
            ImGui::PushFont(appFontExtraSmall);


            ImGui::PushFont(appFontSmall);
            ImGui::Text("Earth:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Revolution motion (0)## earth", &earth.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (e)## earth", &earthRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Precession motion (F6)## earth", &earth.bPrecessionMotion);
            ImGui::SameLine();
            if (ImGui::Button("Reset## earth precession motion"))
                Earth_PrecessionMotion(UCmdParam_Reset);
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Moon:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Revolution motion", &moon.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (m)##moon", &moonRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Orbital plane rotation (F5)", &moon.bOrbitalPlaneRotation);
            ImGui::SameLine();
            if (ImGui::Button("Reset## moon orbital plane rotation"))
                Moon_OrbitalPlaneRotation(UCmdParam_Reset);
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Navigation:");
            ImGui::PopFont();

            ImGui::Indent();
            //ImGui::Checkbox("Shift mode (v)", &bSidewaysMotionMode);
            //ImGui::SameLine(); HelpMarker("When checked, Shift left/right/up/down on mouse movements.\nWhen unchecked, rotate instead.");
            //ImGui::Checkbox("Lock on earth (z)", &bLockOntoEarth);
            //if (ImGui::IsItemEdited())
            //    SetFollowTargetAndMode(bLockOntoEarth ? UCmdParam_On : UCmdParam_Off);
            //ImGui::SameLine(); HelpMarker("Also pauses earth's revolution. Activate this mode and\nthen use mouse to view earth from different angles.");

            //ImGui::Checkbox("Directional lock on earth (b)", &bEarthFollowMode);
            //if (ImGui::IsItemEdited())
            //    NavigationLockOntoEarthWithConstantDirection(bEarthFollowMode ? UCmdParam_On : UCmdParam_Off);
            //ImGui::SameLine();
            //HelpMarker("Earth follow mode");

            //ImGui::Checkbox("Lock on sun (c)", &bLockOntoSun);
            //if (ImGui::IsItemEdited())
            //    NavigationLockOntoSun(bLockOntoSun ? UCmdParam_On : UCmdParam_Off);

            if (ImGui::Button("Show default view (d)"))
                SetDefaultView();
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::Checkbox("Realistic shading", &bRealisticShading);
            ImGui::Checkbox("Show orbits (o)", &bShowOrbitsGlobalEnable);
            ImGui::Checkbox("Show axis (a)", &bShowAxis);
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Time:");
            ImGui::PopFont();
            ImGui::Indent();

            ImGui::Checkbox("Time pause (space bar)", &bSimulationPause);

            ImGui::Button("Fast Rewind (r)");
            if (ImGui::IsItemActivated())       Rewind(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     Rewind(UCmdParam_Off);

            ImGui::SameLine();
            ImGui::Button("Fast Forward (f)");
            if (ImGui::IsItemActivated())       FastForward(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     FastForward(UCmdParam_Off);

            // Arrow buttons with Repeater
            float spacing = ImGui::GetStyle().ItemInnerSpacing.x; 
            ImGui::Text("Time Speed:");
            ImGui::SameLine();
            ImGui::Text("%.3f%%", 100 * _stepMultiplier / 1.0);
            ImGui::SameLine();            
            ImGui::PushButtonRepeat(true);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left))        DecreaseSimulationSpeed();
            ImGui::SameLine(0.0f, spacing);
            if (ImGui::ArrowButton("##right", ImGuiDir_Right))      IncreaseSimulationSpeed();
            ImGui::PopButtonRepeat();
            ImGui::SameLine(0.0f, spacing);
            if (ImGui::Button("Reset"))
                _stepMultiplier = 1.0f;


            ImGui::Unindent();

            //-----------------------------------------------------

            ImGui::Separator();
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            _stepMultiplierFrameRateAdjustment = 60.0f / ImGui::GetIO().Framerate;

            ImGui::Separator();
            ImGui::Text("S: %.4f, %.4f, %.4f", space.S.x, space.S.y, space.S.z);
            ImGui::Text("D: %.4f, %.4f, %.4f", space.D.x, space.D.y, space.D.z);
            ImGui::Text("E orbital angle: %.4f", earth._orbitalAngle);
            ImGui::Text("_stepMultiplier: %f", _stepMultiplier);



            if (!logString.empty())
            {
                ImGui::LogToClipboard();
                ImGui::LogText(logString.c_str());
                logString = "";
                //ImGui::LogText("%f, %f, %f", space.S.x, space.S.y, space.S.z);
                ImGui::LogFinish();
            }

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

