#include "Universe.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "spdlog/spdlog.h"


void Universe::initializeGL()
{
    printf("Inside initializeGL\n");

    spdlog::info("Opening planet.frag.glsl");
    planetGlslProgram.compileShadersFromFile("planet.vert.glsl", "planet.frag.glsl");
    planetGlslProgram.link();

    sunGlslProgram.compileShadersFromFile("sun.vert.glsl", "sun.frag.glsl");
    sunGlslProgram.link();

    starGlslProgram.compileShadersFromFile("star.vert.glsl", "star.frag.glsl");
    starGlslProgram.link();

    simpleGlslProgram.compileShadersFromFile("simple.vert.glsl", "simple.frag.glsl");
    simpleGlslProgram.link();

    fontGlslProgram.compileShadersFromFile("font.vert.glsl", "font.frag.glsl");
    fontGlslProgram.link();

    //---------------------------------------------------------------------------------------------------


    sunRenderer.setAsLightSource();
    sunRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    sunRenderer.constructVerticesAndSendToGpu();

#ifdef DRAW_LINES_INSTEAD_OF_TRIANGLES
    earthRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
#else
    earthRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
#endif
    earthRenderer.constructVerticesAndSendToGpu();
    earthRenderer.bShowLatitudesAndLongitudes = true;
    earthRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);


    moonRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    moonRenderer.constructVerticesAndSendToGpu();
    moonRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);


    marsRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    marsRenderer.constructVerticesAndSendToGpu();
    marsRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    marsRenderer.bShowOrbit = False;

    jupiterRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    jupiterRenderer.constructVerticesAndSendToGpu();
    jupiterRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    jupiterRenderer.bShowOrbit = False;

    saturnRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    saturnRenderer.constructVerticesAndSendToGpu();
    saturnRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    saturnRenderer.bShowOrbit = False;

    uranusRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    uranusRenderer.constructVerticesAndSendToGpu();
    uranusRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    uranusRenderer.bShowOrbit = False;

    neptuneRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    neptuneRenderer.constructVerticesAndSendToGpu();
    neptuneRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);
    neptuneRenderer.bShowOrbit = False;

    //------------------------------

    constructFontInfrastructureAndSendToGpu();


    //---------------------------------------------------------------------------------------------------
    // Axis
    axisRenderer.constructVerticesAndSendToGpu();
    starsRenderer.constructVerticesAndSendToGpu();

    glBindVertexArray(0);       // Disable VBO
}


void Universe::constructFontInfrastructureAndSendToGpu()
{
    glGenVertexArrays(1, &fontVao);
    glGenBuffers(1, &fontVbo);

    glBindVertexArray(fontVao);
    glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //---------------------
    // Large font

    glGenVertexArrays(1, &largeFontVao);
    glGenBuffers(1, &largeFontVbo);

    glBindVertexArray(largeFontVao);
    glBindBuffer(GL_ARRAY_BUFFER, largeFontVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}


/*
 * Top level render method called from the main loop
 */
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

    //spdlog::info("projectionMatrix = {}", glm::to_string(projectionMatrix));


    //=====================================================================================
    // Render all objects in the scene
    //=====================================================================================
    renderAllNontransparentObjects();

    // Objects with alpha < 1.0 have to be rendered after all objects with alpha = 1.0
    // It still doesn't solve all problems.
    renderAllTransparentObjects();

    glBindVertexArray(0);

}

void Universe::renderAllNontransparentObjects()
{
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Labels that are allowed to hide under other objects should be written before the objects.
    // This is because all labels, regardless of what object they are for, are written at a
    // very shallow depth.

    fontGlslProgram.use();
    glm::mat4 projection = glm::ortho(0.0f, float(curWidth), 0.0f, float(curHeight));
    fontGlslProgram.setMat4("projection", glm::value_ptr(projection));

    renderUsingFontGlslProgram();

    fontGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    planetGlslProgram.use();
    planetGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    planetGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    // turn usage of texture on/off based on global setting. Individual spheres might change this
    // depending on whether they have texture set up.
    planetGlslProgram.setBool("useTexture", bRealisticSurfaces);

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

}

void Universe::renderAllTransparentObjects()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    simpleGlslProgram.use();
    simpleGlslProgram.setMat4("view", glm::value_ptr(viewMatrix));
    simpleGlslProgram.setMat4("proj", glm::value_ptr(projectionMatrix));

    renderTransparentUsingSimpleGlslProgram();

    simpleGlslProgram.unuse();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    glDisable(GL_BLEND);

}

void Universe::renderUsingPlanetGlslProgram()
{
    planetGlslProgram.setVec3("sunCenterTransformed", glm::value_ptr(sun.getModelTransformedCenter()));
    planetGlslProgram.setFloat("sunRadius", sun.getRadius());
    planetGlslProgram.setBool("realisticShading", bRealisticShading);

    for (int i = 0; systemRenderers[i] != NULL; i++)
    {
        systemRenderers[i]->renderSphere(planetGlslProgram);
        systemRenderers[i]->renderLatitudeAndLongitudes(planetGlslProgram);
    }

    if (bShowPlanetAxis)
    {
        for (int i = 0; systemRenderers[i] != NULL; i++)
        {
            systemRenderers[i]->renderRotationAxis(planetGlslProgram);
        }
    }
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
    sunGlslProgram.setBool("useTexture", bRealisticSurfaces);
    sunRenderer.renderSphere(sunGlslProgram);
}

void Universe::renderUsingSimpleGlslProgram()
{
    if (bShowAxis)
    {
        axisRenderer.render(simpleGlslProgram);
    }

    if (bShowOrbitsGlobalEnable)
    {
        for (int i = 0; systemRenderers[i] != NULL; i++)
        {
            systemRenderers[i]->renderOrbit(simpleGlslProgram);
        }
    }
}

void Universe::renderTransparentUsingSimpleGlslProgram()
{
    for (int i = 0; systemRenderers[i] != NULL; i++)
    {
        systemRenderers[i]->renderOrbitalPlane(simpleGlslProgram);
    }
}

void Universe::renderUsingFontGlslProgram()
{
    if (bShowMonthNames)
    {
        if (bMonthLabelsCloserToSphere)
            earth.calculateMonthPositions((earth._orbitalRadius + 1.5f*earth._radius) / earth._orbitalRadius);
        else
            earth.calculateMonthPositions(1.2f);


        //glm::vec2 projected = getScreenCoordinates(earth.getModelTransformedCenter());
        //spdlog::info("projected = {}", glm::to_string(projected));

        std::vector<std::string> monthNames = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        for (int i = 0; i < 12; i++)
        {
            glm::vec3 projected = getScreenCoordinates(earth.monthPositions[i]);
            //glm::vec2 projected = getScreenCoordinates(glm::vec3(0.0f, 0.0f, 0.0f));

            //spdlog::info("month Position {}: {}", i, glm::to_string(projected));

            if (projected.z < 1.0f)
            {
                RenderText(
                    monthNames[i].c_str(),
                    projected.x,
                    projected.y,
                    1.0f,
                    glm::vec3(1.0f, 1.0f, 0.0f));
            }
        }
    }
    //RenderText("Hello World", 100.0f, 100.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
}


// prerequisites: fontGlslProgram must be active before calling this method.

void Universe::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    fontGlslProgram.setVec3("textColor", glm::value_ptr(color));

    GLboolean curDepthMaskEnable;
    GLboolean prevBlendEnable;
    
    glGetBooleanv(GL_BLEND, &prevBlendEnable);          // backup blending enable/disable status before enabling it.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGetBooleanv(GL_DEPTH_WRITEMASK, &curDepthMaskEnable);       // backup current depth mask before disabling it

    if (!bShowLabelsOnTop)
        glDepthMask(GL_FALSE);            // disable writing to depth buffer.  This will allow other objects (spheres, etc) to
                                          // overwrite the label when they are drawn later.
    //---------------------------------

    glActiveTexture(GL_TEXTURE0);
    if (bShowLargeLabels)
        glBindVertexArray(largeFontVao);
    else
        glBindVertexArray(fontVao);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch;

        //spdlog::info("*c = {}", *c);
        if (bShowLargeLabels)
            ch = largeCharacters[*c];
        else
            ch = characters[*c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
            {xpos,      ypos + h,  0.0f, 0.0f },
            {xpos,      ypos,      0.0f, 1.0f },
            {xpos + w,  ypos,      1.0f, 1.0f },

            {xpos,      ypos + h,  0.0f, 0.0f },
            {xpos + w,  ypos,      1.0f, 1.0f },
            {xpos + w,  ypos + h,  1.0f, 0.0f },
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // update content of VBO memory
        if (bShowLargeLabels)
            glBindBuffer(GL_ARRAY_BUFFER, largeFontVbo);
        else
            glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // advance cursors for next glyph (advance is 1/64 pixels)
        x += (ch.advance >> 6) * scale;

    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthMask(curDepthMaskEnable);    // restore depth mask
    if (!prevBlendEnable)               // disabled blending if it was previously disabled
        glDisable(GL_BLEND);

}



void Universe::createFontCharacterTexture()
{
    spdlog::info("Generating font character textures");

    //if (FT_Load_Glyph(face, "X", FT_LOAD_RENDER))
    //    cout << "ERROR:FREETYPE Failed to load Glyph" << endl;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // no byte alignment restriction

    for (int i = 0; i < 2; i++)
    {
        if (FT_Init_FreeType(&ft))
            spdlog::error("ERROR:FREETYPE Could not init FreeType library");

        //if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
        if (FT_New_Face(ft, "fonts/Roboto-Medium.ttf", 0, &face))
            spdlog::error("ERROR:FREETYPE Failed to load font");

        if (i == 0)
            FT_Set_Pixel_Sizes(face, 0, 20);
        else
            FT_Set_Pixel_Sizes(face, 0, 64);

        //----------------------

        for (unsigned char c = 0; c < 128; c++)
        {
            if (FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_RENDER)) {
                spdlog::error("ERROR:FREETYPE Failed to load Glyph");
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0, GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };

            if (i == 0)
                characters.insert(std::pair<char, Character>(c, character));
            else
                largeCharacters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    //----------------------

}


// Given a 3d point in the scene, return the x and y coordinates of the point on the 2-d viewport.
// E.g. if the view is locked on earth, the return value will be (w/2, h/2),
//          where w and h are width and height of the viewport.
//
// `scenePoint` is a model-transformed point.
glm::vec3 Universe::getScreenCoordinates(glm::vec3& scenePoint)
{
    //spdlog::info("getScreenCoordinates: {}", glm::to_string(scenePoint));

    glm::vec4 viewport(0.0f, 0.0f, float(curWidth), float(curHeight));

    glm::vec3 projected = glm::project(
        scenePoint,
        viewMatrix,
        projectionMatrix,
        viewport
    );

    return projected;
    //return glm::vec2(projected.x, projected.y);
    //return glm::vec2(0.0f, 0.0f);
}


#include "imgui_internal.h"
extern ImGuiContext * GImGui;

// Like small buttons, small checkboxes fit within text without additional vertical spacing.
bool Universe::SmallCheckbox(const char* label, bool* v)
{
    ImGuiContext& g = *GImGui;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ImGui::Checkbox(label, v);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

void Universe::generateImGuiWidgets()
{
    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
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
            ImGui::TextWrapped("Leela\n\n");
            ImGui::PopFont();

            ImGui::PushFont(appFontMedium);
            ImGui::TextWrapped(
                "This program is intended to be a teaching aid for various celestial concepts. "
                "It primarily intends to explain phenomenon related to orbital mechanics of the planetary motion. \n\n"

                "The program doesn't make any effort to show sizes, distances, rotation & revolution speeds, etc to scale. "
                "If it did, it will hinder its ability to help explain concepts. Showing the earth, moon and sun on the screen at "
                "the same time will be impractical, as they will only be a few pixels wide.\n\n"

                "With distances and sizes not to scale, angles are exagerated to show the same effect.  For example, to show why solar eclipses don't occurr on every "
                "new moon day, the tilt of the moons's orbital plane with respect to earth's orbital plane has been increased to roughly 30 degrees from about 5 degrees.\n\n"

                "Use the demo buttons to see some of the phenomenon.\n\n"

                "You can navigate in the 3D space using mouse and keyboard.  Look at the respective help window for details."

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
            // Helper function used later below
            auto populateKeyboardShortcutTwoColumnTable = [](const char* arr[][2], int numRows) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));
                for (int i = 0; i < numRows; i++)
                {
                    if (arr[i][0] == nullptr) {
                        ImGui::Separator();
                    }
                    else {
                        ImGui::Text(arr[i][0]);  ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 150);
                        ImGui::TextWrapped(arr[i][1]);  ImGui::NextColumn();
                    }
                }
                ImGui::PopStyleColor();
            };

            const char* shortcutHelp[][2] = {
            { "Space bar",      "Pause time. All navigation will continue to work." },
            { "f",              "Pressing and keeping it pressed will cause simulation time to elapse faster by about 10 times."},
            { "r",              "Pressing and keeping it pressed with cause simulation time to run in reverse by about 10 times the current forward speed. "
                                "Releasing this key shall restore the forward movement of time at the speed it was just before pressing this key."},
            { "Right arrow",    "Speeds up the time passage by 100%%."},
            { "Left arrow",     "Slows down the time passage to 50%%."},
            { "d",              "Show default view. Bring the camera to far away position so that the entire earth's orbit is visible. "
                                "If you get lost navigating, press this key to reorient yourself."},
            { "Escape",         "Get back mouse cursor control if in navigation mode." },
            { nullptr, nullptr },

            { "a",              "Toggle visibility of the XYZ coordinate axis.  When looking from the default view position, +ve X direction is towards bottom left from origin (blue),"
                                "+ve Y is towards bottom right from origin (green), and +ve Z is upwards from origin (cyan)." },
            { "e",              "Toggle visibility of earth's orbital plane." },
            { "m",              "Toggle visibility of moon's orbital plane." },

            { "z",              "Toggle earth lock mode. When turned on, the earth will appear at the center of the screen even as it moves in its orbit. "
                                "The camera will stay at its position, but will always look towards the center of the earth. "
                                "Left/right/up/down mouse movements shall rotate the camera around the earth at a constant distance. "
                                "Zoom in and zoom out will work as expected." },
            { "c",              "Toggle sun lock mode. This behavior is similar to earth lock mode, except that the sun will now appear at the center of the screen." },
            
            { "b",              "Toggle follow mode on the currently locked target. The camera will follow the target at a constant distance looking at it from a fixed angle. "
                                "The distance from the target can be changed using zoom in and zoom out controls. But the viewing angle will be the same as it was when the mode was turned on.\n\n"

                                "This mode is used in the 6 month long day and night demo.  Use this mode when you want to observe the earth from a fixed angle as it "
                                "revolves around the sun.  Contrast this mode with the normal lock mode where the camera stays put while only changing its viewing direction. "
                                "Only zoom in/out navigation will work in this mode." },
            { "0 (zero)",       "Toggle earth's revolution motion.  Use this to pause the earth in its orbit so that you can observe various things such as the tilted orbit, shadows, etc." },


            { nullptr, nullptr },
            { nullptr, nullptr },

            { "",               "The 6 keys -- Ins, Del, Home, End, Page Up and Page Down -- form the main keyboard navigation control.\n"
                                "Note that mouse navigation is easier and is preferred."
            },
            { nullptr, nullptr },

            { "Home",           "Zoom in"
                                "\n\n"
            },

            { "End",            "Zoom out"
                                "\n\n"
            },
            { "Del",            "Yaw left\n\n"
                                "Turn left.  In Shift mode, shift left.  If there is a locked target, rotate left around it."
                                "\n\n"
            },
            { "Page Down",      "Yaw right\n\n"
                                "Turn right.  In Shift mode, shift right.  If there is a locked target, rotate right around it."
                                "\n\n"
            },
            { "Page Up",        "Pitch up\n\n"
                                "Rotate up.  In Shift mode, shift up.  If there is a locked target, rotate up around it. "
                                "\n\n"
            },
            { "Insert",         "Pitch down\n\n"
                                "Rotate down.  In Shift mode, shift down.  If there is a locked target, rotate down around it."
                                "\n\n"
            },
            { "Alt + Page Up",  "Roll right\n\n"
                                "Rotate right along the axis the camera is looking at. This will result in the object in front appear to being rotated left.\n"
                                "If there is a locked target, this key combination is ignored."
                                "\n\n"
            },
            { "Alt + Insert",   "Roll left\n\n"
                                "Rotate left along the axis the camera is looking at. This will result in the object in front appear to being rotated right.\n"
                                "If there is a locked target, this key combination is ignored."
                                "\n\n"
            },
            { nullptr, nullptr },
            { "v",              "Toggle 'Shift mode' navigation.  In this mode, left/right/up/down mouse movements will result in the camera being shifted in those directions as "
                                "as opposed to rotating in those directions.  When the camera shifts, the viewing direction vector does not change. It is as if the camera is looking at a point at infinity. "
                                "If you want to look at how the background stars shift due to parallex as the earth moves in its orbit, turn this mode on, go to default "
                                "view (d), and then move the mouse left and right."
            },


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

            ImGui::PushFont(appFontMedium);
            ImGui::Columns(2, "mycolumn");
            ImGui::Text("Shortcut"); ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 150);
            ImGui::Text("Description"); ImGui::NextColumn();
            ImGui::Separator();

            populateKeyboardShortcutTwoColumnTable(shortcutHelp, sizeof(shortcutHelp) / sizeof(shortcutHelp[0]));
            ImGui::PopFont();
        }
        ImGui::End();

    }

    if (bShowMouseNavigationHelp)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(1024, 768));
        if (ImGui::Begin("Mouse Navigation Help", &bShowMouseNavigationHelp))
        {
            auto populateMouseNavigationTwoColumnTable = [](const char* arr[][2], int numRows) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));
                for (int i = 0; i < numRows; i++)
                {
                    if (arr[i][0] == nullptr) {
                        ImGui::Separator();
                    }
                    else {
                        ImGui::Text(arr[i][0]);  ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 300);
                        ImGui::TextWrapped(arr[i][1]);  ImGui::NextColumn();
                    }
                }
                ImGui::PopStyleColor();
            };

            const char* shortcutHelp[][2] = {
            { "Click in the middle area of the screen",       "Mouse cursor will hide and will be grabbed by the application. "
                                                              "You will gain navigation control of the camera / space ship. "
                                                              "You won't be able to click on the menus, buttons, checkboxes, etc."
            },
            
            {nullptr, nullptr },

            { "Double click while in navigation mode\n"
              "or press Escape key",                          "Mouse cursor will be visible and stop being grabbed. "
                                                              "Buttons, checkboxes & menus can now be clicked."
            },

            {nullptr, nullptr },
            {nullptr, nullptr },

            { "Left/right\n(without clicking any button)",
                                                "Yaw.\n\n"

                                                "Rotate camera left/right.  "
                                                "Camera position will not change, only viewing direction will change. \n\n"
                                                
                                                "If there is a locked target (as is the case when the application starts), this will "
                                                " cause horizontal rotation around the target. "
                                                "Both the camera position and direction will change. "

                                                "If 'shift mode' is on, and no target is locked, camera will be *shifted* left/right. "
                                                "In this case, camera "
                                                "position will change, but viewing direction will remain the same.\n\n"

            },
            {nullptr, nullptr },

            { "Foward/backward\n(without clicking any button)",  
                                                
                                                "Pitch.\n\n"

                                                "Similar to mouse left/right action, except that the rotation or shifting will "
                                                " be in up/down/vertical direction"
            },
            {nullptr, nullptr },

            { "Left click + forward/backward",  "Throttle.\n\n"
                                                "Zoom camera in and out"
            },
            {nullptr, nullptr },

            { "Right click + left/right",       "Roll.\n\n"
                                                "This motion is equivalant to tilting your head left/right\n\n"
                                                "If a target is locked, this action will be ignored."
            },
            {nullptr, nullptr },
            {nullptr, nullptr },

            { "Ctrl key + any above action",
                                                "This is like using the 'fine' adjustment knob for the same action.\n"
                                                "Performs that action with less sensitivity to input.  For the same amount of input, "
                                                "there will be less motion.  Use for more control of small movements."
            },
            {nullptr, nullptr },

            { "Shift key + any above action",
                                                "This is like using the 'coarse' adjustment knob for the same action.\n"
                                                "Performs that action with high sensitivity to input. "
                                                "E.g. to move far away from the sun, press shift and then do left click + backward ."
            },


            };

            ImGui::PushFont(appFontMedium);
            ImGui::Columns(2, "mycolumn");
            ImGui::Text("Action"); ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 300);
            ImGui::Text("Description"); ImGui::NextColumn();
            ImGui::Separator();

            populateMouseNavigationTwoColumnTable(shortcutHelp, sizeof(shortcutHelp) / sizeof(shortcutHelp[0]));
            ImGui::PopFont();
        }
        ImGui::End();
    }

    if (bMouseGrabbed)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f, 20.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.25f);
        ImGui::PushFont(appFontSmallMedium);
        if (ImGui::Begin("Escape message", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Escape key / double-click to get mouse back");
        }
        ImGui::End();
        ImGui::PopFont();
    }
    else
    {
        ImGui::PushFont(appFontSmallMedium);
        // Show menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Leela"))
            {
                if (ImGui::MenuItem("Exit Application"))
                    bQuit = true;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                // menu shortcuts are display-only. (https://github.com/ocornut/imgui/issues/6381)
                if (ImGui::MenuItem("Show Fullscreen", "F11")) {
                    toggleFullScreen();
                }
                // Don't pass address of boolean below. if menu is activated using mouse, use the toggle function to achieve the boolean toggle.
                // The same toggle function is used when the shorcut is handled in InputHandling file.
                // We wouldn't have to do it this way if shortcuts were handled by Dear ImGui.
                if (ImGui::MenuItem("Show Control Panel in Navigation mode", "F8", bAlwaysShowControlPanel, true)) {
                    toggleControlPanelVisibilityWhenMouseGrabbed();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("Introduction", nullptr, &bShowIntroduction);
                ImGui::MenuItem("Show Mouse navigation help", nullptr, &bShowMouseNavigationHelp);
                ImGui::MenuItem("Show Keyboard Shortcuts", nullptr, &bShowKeyboardShortcuts);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::PopFont();
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
            if (ImGui::CollapsingHeader("Demos", ImGuiTreeNodeFlags_None)) {
                ImGui::PushFont(appFontExtraSmall);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Total Solar Eclipse## demo"))
                    ShowDemo(UDemo_TotalSolarEclipse);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Total Solar Eclipse view from\nnear the sun## demo"))
                    ShowDemo(UDemo_TotalSolarEclipseViewFromSun);

                if (ImGui::Button("Total Solar Eclipse on north pole## demo"))
                    ShowDemo(UDemo_TotalSolarEclipseOnNorthPole);

                if (ImGui::Button("Annular Solar Eclipse from space## demo"))
                    ShowDemo(UDemo_AnnularSolarEclipseFromSpace);
                ImGui::SameLine();
                HelpMarker("This also shows umbra starting to travel over the earth. This happens between 3rd and 4th contact for a few seconds. So watch out!");

                if (ImGui::Button("Partial Lunar Eclipse## demo"))
                    ShowDemo(UDemo_PartialLunarEclipse);

                if (ImGui::Button("Tilted orbital planes## demo"))
                    ShowDemo(UDemo_TiltedOrbitalPlanes);
                ImGui::SameLine();
                HelpMarker(
                    "Moon's orbital plane tilt is highly exagerated since other distances and sizes are not to scale."
                );

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole (another angle)"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle);

                if (ImGui::Button("Precession motion## demo"))
                    ShowDemo(UDemo_PrecessionMotion);
                ImGui::SameLine();
                HelpMarker(
                    "Notice how earth's north pole changes its orientation in space while maintaining the tilt angle."
                );

                if (ImGui::Button("Star Parallex## demo"))
                    ShowDemo(UDemo_StarParallex);
                ImGui::SameLine();
                HelpMarker(
                    "Observe the movement of the stars in the background as the earth moves in its orbit.  The stars appear to shift horizontaly."
                    "Some stars shift more than others.  Stars closer to earth shift more than those farther away."
                );

                if (ImGui::Button("Moon's Nodal Precession## demo"))
                    ShowDemo(UDemo_MoonsNodalPrecession);

                ImGui::PopFont();
            }
            ImGui::PopFont();
            ImGui::Separator();

            //-----------------------------------------------------

            SmallCheckbox("Realistic shading", &bRealisticShading);
            SmallCheckbox("Textured surfaces", &bRealisticSurfaces);
            SmallCheckbox("Wireframe surfaces", &bShowWireframeSurfaces);
            SmallCheckbox("Orbits (o)", &bShowOrbitsGlobalEnable);
            SmallCheckbox("Coordinate axis (a)", &bShowAxis);
            SmallCheckbox("Planet axis", &bShowPlanetAxis);
            SmallCheckbox("Low darkness at night", &bShowLowDarknessAtNight);
            SmallCheckbox("Month names", &bShowMonthNames);
            SmallCheckbox("Month names closer to planet", &bMonthLabelsCloserToSphere);
            SmallCheckbox("Labels on top", &bShowLabelsOnTop);
            SmallCheckbox("Large Labels", &bShowLargeLabels);

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Time:");
            ImGui::PopFont();
            ImGui::Indent();

            SmallCheckbox("Time pause (space bar)", &bSimulationPause);

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
            ImGui::Separator();

            // Common font for the content of all the below sections
            ImGui::PushFont(appFontExtraSmall);

            //-----------------------------------------------------
            // Earth
            ImGui::PushFont(appFontSmall);
            ImGui::Text("Earth:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            SmallCheckbox("Rotation## earth", &earth.bRotationMotion); ImGui::SameLine();
            SmallCheckbox("Revolution  (0)## earth", &earth.bRevolutionMotion);
            SmallCheckbox("Orbit## earth", &earthRenderer.bShowOrbit);  ImGui::SameLine();
            SmallCheckbox("Orbital plane (e)## earth", &earthRenderer.bShowOrbitalPlane);
            SmallCheckbox("Precession (F6)## earth", &earth.bPrecessionMotion);
            ImGui::SameLine();
            if (ImGui::Button("Reset## earth precession motion"))
                Earth_PrecessionMotion(UCmdParam_Reset);
            SmallCheckbox("Show latitudes/longituedes## earth", &earthRenderer.bShowLatitudesAndLongitudes);
            ImGui::PushItemWidth(100);
            if (ImGui::SliderFloat("Orbital Radius## earth", &earth._orbitalRadius, 500.0f, 4000.0f)) {
                earthRenderer.constructOrbit();
                earthRenderer.constructOrbitalPlaneVertices();
                earthRenderer.constructOrbitalPlaneGridVertices();
            } ImGui::SameLine();
            if (ImGui::Button("Reset## earth orbital radius")) {
                earth.restoreOrbitalRadius();
                earthRenderer.constructOrbit();
                earthRenderer.constructOrbitalPlaneVertices();
                earthRenderer.constructOrbitalPlaneGridVertices();
            }
            if (ImGui::SliderFloat("Axis tilt## earth", &earth._axisTiltAngle_Deg, 0.0f, 90.0f)) {
                earth._axisTiltAngle = glm::radians(earth._axisTiltAngle_Deg);
            } ImGui::SameLine();
            ImGui::PopItemWidth();
            if (ImGui::Button("Reset## earth axis tilt")) {
                earth.restoreAxisTiltAngleFromBackup();
            }
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------
            // Moon

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Moon:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            SmallCheckbox("Revolution", &moon.bRevolutionMotion);
            SmallCheckbox("Orbit## moon", &moonRenderer.bShowOrbit); ImGui::SameLine();
            SmallCheckbox("Orbital plane (m)##moon", &moonRenderer.bShowOrbitalPlane);
            SmallCheckbox("Nodal Precession (F5)", &moon.bOrbitalPlaneRotation);
            ImGui::SameLine();
            if (ImGui::Button("Reset## moon orbital plane rotation"))
                Moon_OrbitalPlaneRotation(UCmdParam_Reset);
            SmallCheckbox("Sync with earth's revolution", &moon.bOrbitalPlaneRotationSyncToParent);
            ImGui::PushItemWidth(100);
            //ImGui::SetNextItemWidth(180);
            if (ImGui::SliderFloat("Orbital Radius## moon", &moon._orbitalRadius, 120.0f, 600.0f)) {
                moonRenderer.constructOrbit();
                moonRenderer.constructOrbitalPlaneVertices();
                moonRenderer.constructOrbitalPlaneGridVertices();
            } ImGui::SameLine();
            if (ImGui::Button("Reset## moon orbital radius")) {
                moon.restoreOrbitalRadius();
                moonRenderer.constructOrbit();
                moonRenderer.constructOrbitalPlaneVertices();
                moonRenderer.constructOrbitalPlaneGridVertices();
            }
            if (ImGui::SliderFloat("Orbital Tilt", &moon._orbitalPlaneTiltAngle_Deg, 0.0f, 30.0f)) {
                moon._orbitalPlaneTiltAngle = glm::radians(moon._orbitalPlaneTiltAngle_Deg);
                moonRenderer.constructOrbit();
                moonRenderer.constructOrbitalPlaneVertices();
                moonRenderer.constructOrbitalPlaneGridVertices();
            }
            ImGui::PopItemWidth();
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Mars:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            SmallCheckbox("Revolution", &mars.bRevolutionMotion);
            SmallCheckbox("Orbit## mars", &marsRenderer.bShowOrbit); ImGui::SameLine();
            SmallCheckbox("Orbital plane (,)##mars", &marsRenderer.bShowOrbitalPlane);
            ImGui::Unindent();

            ImGui::Separator();


            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Navigation:");
            ImGui::PopFont();

            ImGui::Indent();
            //SmallCheckbox("Shift mode (v)", &bSidewaysMotionMode);
            //ImGui::SameLine(); HelpMarker("When checked, Shift left/right/up/down on mouse movements.\nWhen unchecked, rotate instead.");
            //SmallCheckbox("Lock on earth (z)", &bLockOntoEarth);
            //if (ImGui::IsItemEdited())
            //    SetLockTargetAndMode(bLockOntoEarth ? UCmdParam_On : UCmdParam_Off);
            //ImGui::SameLine(); HelpMarker("Also pauses earth's revolution. Activate this mode and\nthen use mouse to view earth from different angles.");

            //SmallCheckbox("Directional lock on earth (b)", &bEarthFollowMode);
            //if (ImGui::IsItemEdited())
            //    NavigationLockOntoEarthWithConstantDirection(bEarthFollowMode ? UCmdParam_On : UCmdParam_Off);
            //ImGui::SameLine();
            //HelpMarker("Earth follow mode");

            //----------------------------------------------------------
            ImGui::Text("Lock target:");  ImGui::SameLine(); HelpMarker("Camera will always look at the selected target.");
            int lock = (lockTarget == &earth) ? 0 : (lockTarget == &sun) ? 1 : (lockTarget == &moon) ? 2 : 3;
            int previousLock = lock;
            ImGui::RadioButton("earth", &lock, 0); ImGui::SameLine();
            ImGui::RadioButton("sun", &lock, 1); ImGui::SameLine();
            ImGui::RadioButton("moon", &lock, 2); ImGui::SameLine();
            ImGui::RadioButton("none", &lock, 3);
            if (lock != previousLock)
                SetLockTargetAndMode(
                (lock == 0) ? &earth : (lock == 1) ? &sun : (lock == 2) ? &moon : nullptr,
                    TargetLockMode_ViewTarget);

            //----------------------------------------------------------
            ImGui::Text("Lock mode: "); ImGui::SameLine(); HelpMarker("If 'Follow lock' selected, camera will move with the object looking at it from the same direction.");
            TargetLockMode mode = lockMode;
            TargetLockMode previousMode = mode;
            ImGui::RadioButton("View lock", (int*)& mode, int(TargetLockMode_ViewTarget)); ImGui::SameLine();
            ImGui::RadioButton("Follow lock", (int*)&mode, int(TargetLockMode_FollowTarget));
            if (mode != previousMode)
                SetLockMode(mode);

            //----------------------------------------------------------
            if (ImGui::Button("Show default view (d)"))
                SetDefaultView();
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            _stepMultiplierFrameRateAdjustment = REFERENCE_FRAME_RATE / ImGui::GetIO().Framerate;

            ImGui::Separator();
            ImGui::Text("S: %.4f, %.4f, %.4f", space.S.x, space.S.y, space.S.z);
            //ImGui::Text("D: %.4f, %.4f, %.4f", space.D.x, space.D.y, space.D.z);
            //ImGui::Text("E orbital angle: %.4f", earth._orbitalAngle);
            //ImGui::Text("_stepMultiplier: %f", _stepMultiplier);



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

