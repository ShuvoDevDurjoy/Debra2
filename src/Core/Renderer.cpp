#include <GraphEngine/Core/Renderer.hpp>
#include <GraphEngine/Core/GraphApp.hpp> // For window_width / window_height

void Renderer::Draw(const std::vector<GraphMathObject*>& objects, Camera* camera, float dt) {
    if (!camera) return;

    // Ensure the entire frame starts with a clean stencil and depth buffer
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pass 1: Draw Fills
    for (GraphMathObject* obj : objects) {
        if (!obj->showGraph || !obj->showFill) continue;

        if (GraphObject* gObj = dynamic_cast<GraphObject*>(obj)) {
            renderGraphObjectFill(gObj, camera, dt);
        } else if (ThreeDObject* tObj = dynamic_cast<ThreeDObject*>(obj)) {
            renderThreeDObjectFill(tObj, camera, dt);
        }
    }

    // Pass 2: Draw Strokes
    for (GraphMathObject* obj : objects) {
        if (!obj->showGraph || !obj->showStroke) continue;

        if (GraphObject* gObj = dynamic_cast<GraphObject*>(obj)) {
            renderGraphObjectStroke(gObj, camera, dt);
        } else if (ThreeDObject* tObj = dynamic_cast<ThreeDObject*>(obj)) {
            renderThreeDObjectStroke(tObj, camera, dt);
        }
    }
}

void Renderer::renderGraphObjectFill(GraphObject* gObj, Camera* camera, float dt) {
    if (gObj->fillProgress <= 0.0f) return;
    if (!gObj->fill_data_initialized)
    {
        gObj->InitFillData();
        if (!gObj->fill_data_initialized) return;
    }
    if (!gObj->fill_data_initialized)
    {
        gObj->InitFillData();
        if (!gObj->fill_data_initialized) return;
    }

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    // Stencil Pass Setup
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // no colour output
    glDepthMask(GL_TRUE); // Should be true for stencil test? Wait. 
    // Actually, we don't want to write to depth during STENCIL pass.
    glDepthMask(GL_FALSE);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    
    // Increment for CW, decrement for CCW -> non-zero fill rule
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    glDisable(GL_CULL_FACE);

    if (gObj->stencil_shader) {
        gObj->stencil_shader->use();
        gObj->stencil_shader->setMat4("model", gObj->model);
        gObj->stencil_shader->setMat4("view", view);
        gObj->stencil_shader->setMat4("projection", projection);
        gObj->stencil_shader->setInt("vertexCount", gObj->getSize());
        gObj->stencil_shader->setFloat("u_fill_progress", gObj->fillProgress);
    }
    
    gObj->drawFill(dt); // Just binds VAO and issues draw call

    // Cover Pass
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO); // clear stencil after painting

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // Ensure it renders even if at same Z if needed, or GL_LESS

    if (gObj->cover_shader) {
        gObj->cover_shader->use();
        gObj->cover_shader->setMat4("model", gObj->model);
        gObj->cover_shader->setMat4("view", view);
        gObj->cover_shader->setMat4("projection", projection);
        
        glm::vec3 fc = glm::vec3(0.5f, 1.0f, 0.5f);
        if (fc == glm::vec3(0) && !gObj->colors.empty())
            fc = glm::vec3(gObj->colors[0].RED, gObj->colors[0].GREEN, gObj->colors[0].BLUE);
            
        gObj->cover_shader->setVec3("u_fill_color", fc.r, fc.g, fc.b);
        gObj->cover_shader->setFloat("u_fill_opacity", gObj->fillOpacity);
        gObj->cover_shader->setFloat("u_fill_progress", gObj->fillProgress);
        gObj->cover_shader->setInt("vertexCount", gObj->getSize());
    }

    gObj->drawFill(dt); // Issues draw call again for cover

    glDepthMask(GL_TRUE); 
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::renderGraphObjectStroke(GraphObject* gObj, Camera* camera, float dt) {
    if (!gObj->stroke_data_initialized || !gObj->stroke_shader) return;

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();
    glm::vec3 camPos = camera->getPosition();

    gObj->stroke_shader->use();
    gObj->stroke_shader->setMat4("model", gObj->model);
    gObj->stroke_shader->setMat4("view", view);
    gObj->stroke_shader->setMat4("projection", projection);

    gObj->stroke_shader->setFloat("uProgress", gObj->progress);
    gObj->stroke_shader->setInt("uSegmentCount", std::max(0, gObj->getStrokeVertexCount() / 3));
    gObj->stroke_shader->setInt("vertexCount", gObj->getSize());
    gObj->stroke_shader->setVec3("objectColor", 0.67f, 0.67f, 0.67f);
    gObj->stroke_shader->setVec3("lightColor", 1.0f, 0.8f, 0.8f);
    gObj->stroke_shader->setVec3("lightPos", camPos.x, camPos.y, camPos.z);
    gObj->stroke_shader->setVec3("viewPos", camPos.x, camPos.y, camPos.z);
    gObj->stroke_shader->setFloat("u_thickness", 1.0f);
    gObj->stroke_shader->setFloat("u_stroke_opacity", gObj->stroke_opacity);
    gObj->stroke_shader->setFloat("u_radius", 2.0f);
    gObj->stroke_shader->setVec2("uViewportSize", (float)GraphApp::window_width, (float)GraphApp::window_height);
    gObj->stroke_shader->setFloat("uAntiAliasWidth", 1.0f);
    gObj->stroke_shader->setFloat("u_line_width", gObj->line_width);
    gObj->stroke_shader->setFloat("uv_anti_alias_width_pass", 1.0f);
    gObj->stroke_shader->setFloat("user_bezier_always", gObj->use_bezier_always ? 1.0f : 0.0f);
    gObj->stroke_shader->setInt("u_layer", gObj->layer);

    gObj->stroke_shader->setFloat("uMiterLimit", gObj->miter_limit);
    int manim_join_style = 3;
    switch (gObj->stroke_join_style) {
        case GraphMathObject::StrokeJoinStyle::MITER:
            manim_join_style = 3;
            break;
        case GraphMathObject::StrokeJoinStyle::BEVEL:
            manim_join_style = 2;
            break;
        case GraphMathObject::StrokeJoinStyle::ROUND:
            manim_join_style = 1;
            break;
    }
    gObj->stroke_shader->setInt("uStrokeJoinStyle", manim_join_style);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gObj->drawStroke(dt);
}

void Renderer::renderThreeDObjectFill(ThreeDObject* tObj, Camera* camera, float dt) {
    if (tObj->fill_shader) {
        tObj->fill_shader->use();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix();
        glm::vec3 camPos = camera->getPosition();

        tObj->fill_shader->setMat4("model", tObj->model);
        tObj->fill_shader->setMat4("view", view);
        tObj->fill_shader->setMat4("projection", projection);

        tObj->fill_shader->setFloat("u_progress", tObj->fillProgress);
        tObj->fill_shader->setInt("u_generation_direction", static_cast<int>(tObj->generation_dir));
        tObj->fill_shader->setVec2("u_resolution", tObj->resolution.first, tObj->resolution.second);

        tObj->fill_shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        tObj->fill_shader->setVec3("lightPos", camPos);
        tObj->fill_shader->setVec3("viewPos", camPos);
        tObj->fill_shader->setInt("u_use_grid", tObj->showGrid ? 1 : 0);
        tObj->fill_shader->setFloat("u_gloss", tObj->gloss);
        tObj->fill_shader->setFloat("u_shadow", tObj->shadow);
        tObj->fill_shader->setFloat("u_fill_opacity", tObj->fillOpacity);
        
        glm::vec3 fc(1.0f, 1.0f, 1.0f);
        if (!tObj->fill_colors.empty()) {
            fc = glm::vec3(tObj->fill_colors[0].RED, tObj->fill_colors[0].GREEN, tObj->fill_colors[0].BLUE);
        }
        tObj->fill_shader->setVec3("objectColor", fc.r, fc.g, fc.b);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);

        // Pass 1: Render back faces (without writing to depth buffer)
        glCullFace(GL_FRONT);
        glDepthMask(GL_FALSE);
        tObj->drawFill(dt);

        // Pass 2: Render front faces (normal depth writing)
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
        tObj->drawFill(dt);

        glDisable(GL_CULL_FACE);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}

void Renderer::renderThreeDObjectStroke(ThreeDObject* tObj, Camera* camera, float dt) {
    if (tObj->stroke_shader) {
        tObj->stroke_shader->use();

        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix();
        glm::vec3 camPos = camera->getPosition();

        tObj->stroke_shader->setMat4("model", tObj->model);
        tObj->stroke_shader->setMat4("view", view);
        tObj->stroke_shader->setMat4("projection", projection);

        tObj->stroke_shader->setFloat("u_progress", tObj->fillProgress);
        tObj->stroke_shader->setInt("u_generation_direction", static_cast<int>(tObj->generation_dir));
        tObj->stroke_shader->setVec2("u_resolution", tObj->resolution.first, tObj->resolution.second);
        tObj->stroke_shader->setFloat("u_stroke_width", tObj->line_width);
        tObj->stroke_shader->setVec2("u_screen_size", (float)GraphApp::window_width, (float)GraphApp::window_height);

        tObj->stroke_shader->setVec3("lightColor", 0.5f, 0.5f, 0.5f);
        tObj->stroke_shader->setVec3("lightPos", camPos);
        tObj->stroke_shader->setVec3("viewPos", camPos);

        glm::vec3 sc(1.0f, 0.67f, 0.67f);
        if (!tObj->stroke_colors.empty()) {
            sc = glm::vec3(tObj->stroke_colors[0].RED, tObj->stroke_colors[0].GREEN, tObj->stroke_colors[0].BLUE);
        }
        tObj->stroke_shader->setVec3("objectColor", sc.r, sc.g, sc.b);

        tObj->drawStroke(dt);

        glDisable(GL_DEPTH_TEST);
    }
}
