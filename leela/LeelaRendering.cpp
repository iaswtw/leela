#include "Leela.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "spdlog/spdlog.h"



void Leela::constructFontInfrastructureAndSendToGpu()
{
    glGenVertexArrays(1, &fontVao);
    glGenBuffers(1, &fontVbo);

    glBindVertexArray(fontVao);
    glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);


    //-------------
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //-------------

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //---------------------
    // Large font

    glGenVertexArrays(1, &largeFontVao);
    glGenBuffers(1, &largeFontVbo);

    glBindVertexArray(largeFontVao);
    glBindBuffer(GL_ARRAY_BUFFER, largeFontVbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);


    //-------------
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //-------------


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}


/*
 * Top level render method called from the main loop
 */
void Leela::render()
{
    renderAllViewportTypes();
    
}

void Leela::renderAllViewportTypes()
{
    for (auto viewportType : {  ViewportType::Primary,
                                ViewportType::Minimap,
                                ViewportType::AlternateObserver }
                                // add scene types here if new items are added to ViewportType enum
        )
    {
        bool configured = setupViewport(viewportType);
        
        if (configured)
            renderAllStages(viewportType);
    }

    glBindVertexArray(0);
}


void Leela::renderAllStages(ViewportType viewportType)
{

    //=====================================================================================
    // Render all objects in the scene
    //=====================================================================================
    for (auto renderStage : {   RenderStage::Pre,
                                RenderStage::Main,
                                RenderStage::Post,
                                RenderStage::TranslucentMain,
                                RenderStage::Final }
                                // add stages here if new items are added to RenderStage enum
        )
    {
        renderUsingAllShaderPrograms(viewportType, renderStage);
    }

}

void Leela::renderUsingAllShaderPrograms(ViewportType viewportType, RenderStage renderStage)
{
    for (GlslProgram* prog : shaderPrograms)
    {
        prog->use();

        //---------------------------------------------------------
        // Configure shader program
        //---------------------------------------------------------
        if (prog->type() == GlslProgramType::Font)
        {
            glm::mat4 projection = glm::ortho(0.0f, float(curViewportWidth), 0.0f, float(curViewportHeight));
            prog->setMat4("projection", glm::value_ptr(projection));
        }
        else if (prog->type() == GlslProgramType::Planet)
        {

            prog->setMat4("view", glm::value_ptr(viewMatrix));
            prog->setMat4("proj", glm::value_ptr(projectionMatrix));

            // turn usage of texture on/off based on global setting. Individual spheres might change this
            // depending on whether they have texture set up.
            prog->setBool("useTexture", bRealisticSurfaces);

            prog->setVec3("sunCenterTransformed", glm::value_ptr(sun->getModelTransformedCenter()));
            prog->setFloat("sunRadius", sun->getRadius());
            prog->setBool("realisticShading", bRealisticShading);
        }
        else if (prog->type() == GlslProgramType::Star)
        {
            prog->setMat4("view", glm::value_ptr(viewMatrix));
            prog->setMat4("proj", glm::value_ptr(projectionMatrix));
        }
        else if (prog->type() == GlslProgramType::Sun)
        {
            prog->setMat4("view", glm::value_ptr(viewMatrix));
            prog->setMat4("proj", glm::value_ptr(projectionMatrix));
            prog->setBool("useTexture", bRealisticSurfaces);
        }
        else if (prog->type() == GlslProgramType::Simple)
        {
            prog->setMat4("view", glm::value_ptr(viewMatrix));
            prog->setMat4("proj", glm::value_ptr(projectionMatrix));
        }
        else if (prog->type() == GlslProgramType::SimpleOrtho)
        {
        }
        else if (prog->type() == GlslProgramType::BookmarkSphere)
        {
            glm::mat4 projection = glm::ortho(0.0f, float(curViewportWidth), 0.0f, float(curViewportHeight));
            prog->setMat4("projection", glm::value_ptr(projection));
        }
        //------------------------------------------------------

        if (renderStage == RenderStage::TranslucentMain) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        renderSceneUsingGlslProgram(renderStage, *prog, viewportType);

        if (renderStage == RenderStage::TranslucentMain)
            glDisable(GL_BLEND);

        //------------------------------------------------------

        prog->unuse();
    }
}


bool Leela::setupViewport(ViewportType viewportType)
{
    int x, y, w, h;
    bool configured = false;

    glEnable(GL_SCISSOR_TEST);

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    if (viewportType == ViewportType::Primary) {
        curViewportX = 0;
        curViewportY = 0;
        curViewportWidth = curWidth;
        curViewportHeight = curHeight;

        //spdlog::info("Leela::render()");
        glScissor(curViewportX, curViewportY, curViewportWidth, curViewportHeight);
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
            float(curViewportWidth) / float(curViewportHeight),
            1.0f,
            10000000.0f);

        //spdlog::info("projectionMatrix = {}", glm::to_string(projectionMatrix));


        glViewport(curViewportX, curViewportY, curViewportWidth, curViewportHeight);

        configured = true;
    }

    else if (viewportType == ViewportType::Minimap) {

        if (minimapViewport->bEnabled) {
            //int maxViewports;
            //glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
            //spdlog::info("Max viewports = {}", maxViewports);

            //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            curViewportX = minimapViewport->_x;
            curViewportY = minimapViewport->_y;
            curViewportWidth = minimapViewport->_w;
            curViewportHeight = minimapViewport->_h;

            glScissor(curViewportX, curViewportY, curViewportWidth, curViewportHeight);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //viewMatrix = glm::lookAt(
            //    glm::vec3(0.0, -10000.0, 4000.0),
            //    glm::vec3(0, 0, 0),
            //    glm::vec3(0, 500, 0));

            if (minimapMode == "Zoomed Out") {
                // Zoomed-out view
                viewMatrix = glm::lookAt(
                    space.S.translated(10000, VECTOR(space.D, space.S)).toVec3(),
                    space.D.toVec3(),
                    //glm::vec3(0, 0, 0),
                    space.getUpwardDirectionVector());
            }
            else if (minimapMode == "Rear View") {
                // "Review view mirror" view
                viewMatrix = glm::lookAt(
                    space.S.toVec3(),
                    space.S.translated(1000, VECTOR(space.D, space.S)).toVec3(),
                    space.getUpwardDirectionVector());
            }


            // perspective transformation
            //----------------------------------------------
            projectionMatrix = glm::perspective(
                glm::radians(35.0f),
                float(curViewportWidth) / float(curViewportHeight),
                1.0f,
                10000000.0f);

            glViewport(curViewportX, curViewportY, curViewportWidth, curViewportHeight);

            configured = true;
        }
    }

    else if (viewportType == ViewportType::AlternateObserver) {

    }

    return configured;
}

//
// Render all scene objects.
//
void Leela::renderSceneUsingGlslProgram(RenderStage renderStage, GlslProgram& glslProgram, ViewportType viewportType)
{
    //spdlog::info("renderSceneUsingGlslProgram: glslProgram type = {}", (int)glslProgram.type());
    renderSceneObjectUsingGlslProgram(&scene, renderStage, glslProgram, viewportType);
}

//
// Render the given `sceneObject` and all its children recursively.
//
void Leela::renderSceneObjectUsingGlslProgram(SceneObject * sceneObject, RenderStage renderStage, GlslProgram& glslProgram, ViewportType viewportType)
{
    if (!sceneObject->hidden()) {
        for (Renderer* r : sceneObject->_renderers)
        {
            r->render(viewportType, renderStage, glslProgram);
        }

        for (SceneObject* obj : sceneObject->_childSceneObjects)
        {
            renderSceneObjectUsingGlslProgram(obj, renderStage, glslProgram, viewportType);
        }
    }
}



//
// prerequisites: fontGlslProgram must be active before calling this method.
//
void Leela::RenderText(GlslProgram& glslProgram, RenderTextType renderType, std::string text, float x, float y, float z, float scale, glm::vec3 color)
{
    glslProgram.setVec3("textColor", glm::value_ptr(color));

    GLboolean curDepthMaskEnable;
    GLboolean prevBlendEnable;
    
    glGetBooleanv(GL_BLEND, &prevBlendEnable);          // backup blending enable/disable status before enabling it.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (renderType == RenderTextType_ScreenText) {
        glGetBooleanv(GL_DEPTH_WRITEMASK, &curDepthMaskEnable);       // backup current depth mask before disabling it
        if (!bShowLabelsOnTop)
            glDepthMask(GL_FALSE);            // disable writing to depth buffer.  This will allow other objects (spheres, etc) to
                                              // overwrite the label when they are drawn later.
    }

    //---------------------------------

    glActiveTexture(GL_TEXTURE0);
    if (bShowLargeLabels)
        glBindVertexArray(largeFontVao);
    else
        glBindVertexArray(fontVao);


    PNT p(x, y, z), p1, p2, p3, p4, p5, p6;

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

        float ch_w = ch.size.x * scale;
        float ch_h = ch.size.y * scale;

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // update content of VBO memory
        if (bShowLargeLabels)
            glBindBuffer(GL_ARRAY_BUFFER, largeFontVbo);
        else
            glBindBuffer(GL_ARRAY_BUFFER, fontVbo);


        if (renderType == RenderTextType_ScreenText) {
            float xpos = x + ch.bearing.x * scale;
            float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            // update VBO for each character
            float vertices[6][5] = {
                // point coordiinates                 Texture coordinates
                //---------------------------------   -------------------------
                {xpos,         ypos + ch_h,  0.0f,    0.0f, 0.0f },                 // the point here is equivalent to p1 in the `else` part
                {xpos,         ypos,         0.0f,    0.0f, 1.0f },                 // the point here is equivalent to p2 in the `else` part
                {xpos + ch_w,  ypos,         0.0f,    1.0f, 1.0f },                 // the point here is equivalent to p3 in the `else` part

                {xpos,         ypos + ch_h,  0.0f,    0.0f, 0.0f },                 // the point here is equivalent to p1 in the `else` part
                {xpos + ch_w,  ypos,         0.0f,    1.0f, 1.0f },                 // the point here is equivalent to p3 in the `else` part
                {xpos + ch_w,  ypos + ch_h,  0.0f,    1.0f, 0.0f },                 // the point here is equivalent to p6 in the `else` part
            };

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            // advance cursors for next glyph (advance is 1/64 pixels)
            x += (ch.advance >> 6) * scale;

        }
        else {
            // text is upright in z direction and perpendicular to the observer.
            // find the 4 unique points of the two triangles of the character glyph.
            // starting with x, y and z given to this function, move in the "right" direction given observer's position and down direction.
            // 
            // p1/p4 o--o p6   
            //       |\ |
            //       | \|
            // p2    o--o p3/p5

            // find the "right" direction vector
            // assume `space` object's S, D, R and correctly set.  (R gives downward direction with DR perpendicular to SD)
            VECTOR DS = VECTOR(space.D, space.S);
            VECTOR DR = VECTOR(space.D, space.R);
            VECTOR DL = DS.cross(DR);


            // Calculate p2 which is equivalent of this coordinate.
            //      - float xpos = x + ch.bearing.x * scale;
            //      - float ypos = y - (ch.size.y - ch.bearing.y) * scale;
            p2 = p.translated(ch.bearing.x * scale, DL).translated((ch.size.y - ch.bearing.y) * scale, DR);
            p1 = p2.translated(-ch_h, DR);           // -ve sign for `ch_h` because DR is pointing down
            p3 = p2.translated(ch_w, DL);
            p6 = p3.translated(-ch_h, DR);

            float vertices[6][5] = {
                // point coordiinates                 Texture coordinates
                //---------------------------------   -------------------------
                {p1.x, p1.y, p1.z,                    0.0f, 0.0f },
                {p2.x, p2.y, p2.z,                    0.0f, 1.0f },
                {p3.x, p3.y, p3.z,                    1.0f, 1.0f },
                                                    
                {p1.x, p1.y, p1.z,                    0.0f, 0.0f },
                {p3.x, p3.y, p3.z,                    1.0f, 1.0f },
                {p6.x, p6.y, p6.z,                    1.0f, 0.0f },
            };

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            // advance cursors for next glyph (advance is 1/64 pixels)
            p.translate((ch.advance >> 6) * scale, DL);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (renderType == RenderTextType_ScreenText) {
        glDepthMask(curDepthMaskEnable);    // restore depth mask
    }

    if (!prevBlendEnable)               // disabled blending if it was previously disabled
        glDisable(GL_BLEND);

}

int Leela::getHeightOfCharA()
{
    Character ch = characters['A'];
    return ch.size.y;
}

void Leela::createFontCharacterTexture()
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
// `scenePoint` is a model-transformed point in world coordinates.
glm::vec3 Leela::getScreenCoordinates(glm::vec3 scenePoint)
{
    //spdlog::info("getScreenCoordinates: {}", glm::to_string(scenePoint));

    //glm::vec4 viewport(0.0f, 0.0f, float(curWidth), float(curHeight));
    //glm::vec4 viewport(0.0f, 0.0f, curViewportWidth, curViewportHeight);
    glm::vec4 viewport(curViewportX, curViewportY, curViewportWidth, curViewportHeight);

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

// atPoint      - segment of length `segmentLength` is located at this point
float Leela::findScreenProjectedHeight(float segmentLength, glm::vec3 atPoint)
{
    glm::vec3 screenProj1 = getScreenCoordinates(PNT(atPoint).translated(-segmentLength/2, space.DR).toVec3());
    glm::vec3 screenProj2 = getScreenCoordinates(PNT(atPoint).translated(segmentLength /2, space.DR).toVec3());

    return fabs(screenProj1.y - screenProj2.y);
}


#include "imgui_internal.h"
extern ImGuiContext * GImGui;

// Like small buttons, small checkboxes fit within text without additional vertical spacing.
bool Leela::SmallCheckbox(const char* label, bool* v)
{
    ImGuiContext& g = *GImGui;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ImGui::Checkbox(label, v);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

