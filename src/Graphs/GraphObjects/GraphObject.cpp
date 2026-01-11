#include "../../../include/GraphEngine/Graphs/GraphObjects/GraphObject.hpp"

GraphObject::GraphObject()
{
}

void GraphObject::update(float dt)
{
    if (updater_func)
    {
        if (updateStartTime[0] <= dt && (updateEndTime[0] >= dt || updateEndTime[0] == -1))
        {
            UpdateGraphWithFunction((dt - updateStartTime[0]) / (updateEndTime[0] - updateStartTime[0]));
        }
    }
    updateFill(dt);
    updateStroke(dt);
}

void GraphObject::updateStroke(float dt)
{

    if (stroke_data_initialized && stroke_shader)
    {
        stroke_shader->use();
        projection = GraphApp::projection;
        view = GraphApp::view;
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(GraphApp::rotX), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(GraphApp::rotY), glm::vec3(0, 1, 0));

        stroke_shader->setMat4("model", model);
        stroke_shader->setMat4("view", view);
        stroke_shader->setMat4("projection", projection);

        // std::cout << "progress is: " << progress << std::endl;
        stroke_shader->setFloat("uProgress", progress);
        stroke_shader->setInt("vertexCount", getSize());
        stroke_shader->setVec3("objectColor", color->RED, color->GREEN, color->BLUE);
        stroke_shader->setVec3("lightColor", 1.0f, 0.8f, 0.8f);
        stroke_shader->setVec3("lightPos", GraphApp::cameraPos[0], GraphApp::cameraPos[1], GraphApp::cameraPos[2]);
        stroke_shader->setVec3("viewPos", GraphApp::cameraPos[0], GraphApp::cameraPos[1], GraphApp::cameraPos[2]);
        stroke_shader->setFloat("u_thickness", 1.0f);
        stroke_shader->setFloat("u_radius", 2.0f);
        stroke_shader->setVec2("uViewportSize", 1200, 600);
        stroke_shader->setFloat("u_line_width", line_width);
        stroke_shader->setFloat("uv_anti_alias_width_pass", 1.0f);
        glBindVertexArray(StrokeVAO);
        glDrawArrays(GL_LINE_STRIP, drawStart, getSize());
    }

    for (GraphObject *subObj : subGraphObjects)
    {
        subObj->draw(dt);
    }
}

void GraphObject::updateFill(float dt)
{

    fill_shader->use();

    fill_shader->setMat4("model", model);
    fill_shader->setMat4("view", view);
    fill_shader->setMat4("projection", projection);

    fill_shader->setFloat("uProgress", fillProgress);
    fill_shader->setInt("vertexCount", getFillSize());
    fill_shader->setVec3("objectColor", color->RED, color->GREEN, color->BLUE);
    fill_shader->setVec3("lightColor", 1.0f, 0.8f, 0.8f);
    fill_shader->setFloat("fillOpacity", fillOpacity);
    fill_shader->setVec3("lightPos", GraphApp::cameraPos[0], GraphApp::cameraPos[1], GraphApp::cameraPos[2]);
    fill_shader->setVec3("viewPos", GraphApp::cameraPos[0], GraphApp::cameraPos[1], GraphApp::cameraPos[2]);

    glBindVertexArray(FillVAO);
    glDrawArrays(GL_TRIANGLES, 0, getFillSize());
}
void GraphObject::Init(float s_time)
{

    // shader to draw the stroke of the graph
    this->stroke_shader = new Shader(vertexShaderPath, geometricShaderPath, fragmentShaderPath);
    // shader for filling the graph's underneath area
    this->fill_shader = new Shader(fillVertexShaderPath, fillGeometricShaderPath, fillFragmentShaderPath);
    // check if the graph has any points to deal with
    if (!getSize())
    {
        glGenVertexArrays(1, &FillVAO);
        glGenBuffers(1, &FillVBO);
        glGenVertexArrays(1, &FillVAO);
        glGenBuffers(1, &FillVBO);
        std::cerr << "No points in the graph to initialize" << std::endl;
    }
    else
    {
        // Initialize OpenGL Buffer and Array
        glGenVertexArrays(1, &StrokeVAO);
        glGenBuffers(1, &StrokeVBO);

        // initialize the stroke data
        InitStrokeData();
        resolution = getSize() - 1;

        glGenVertexArrays(1, &FillVAO);
        glGenBuffers(1, &FillVBO);
        // initialize the fill data
        InitFillData();

        drawSize = getSize();
        drawStart = 0;
    }

    InitSubObject(s_time);

    is_initialized = true;
}

void GraphObject::InitStrokeData()
{
    setStrokeData();
    applyColorToVertex();
    glBindVertexArray(StrokeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, StrokeVBO);
    initializeStrokeShader();
    uploadStrokeDataToShader();
}

void GraphObject::InitFillData()
{
    setFillData();
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    initializeFillShader();
    uploadFillDataToShader();
}

void GraphObject::initializeStrokeShader()
{
    int buffer_size = getSize() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, 4 * buffer_size, nullptr, GL_DYNAMIC_DRAW);
}
void GraphObject::initializeFillShader()
{
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    int buffer_size = getFillSize() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
}

void GraphObject::InitSelf(float s_time)
{
}

void GraphObject::InitSubObject(float s_time)
{
    for (GraphObject *subObj : subGraphObjects)
    {
        subObj->Init(s_time);
    }
}

void GraphObject::setStrokeData()
{
    stroke_current_points.clear();
    stroke_prev_points.clear();
    stroke_next_points.clear();
    if (!getSize())
    {
        std::cerr << "No Points there to set as stroke data" << std::endl;
        return;
    }
    for (int i = 0; i < getSize(); ++i)
    {
        stroke_current_points.push_back(points[i]);
    }
    // setting up prev points for stroke
    if (isEqual(points[0], points[getSize() - 1]))
    {
        stroke_prev_points.push_back(points[getSize() - 1]);
    }
    else
    {
        stroke_prev_points.push_back(points[0]);
    }

    // add all the points upto getSize() - 1 to the prev point
    for (int i = 0; i < getSize() - 1; ++i)
    {
        stroke_prev_points.push_back(points[i]);
    }

    // setting up next points for stroke
    for (int i = 1; i < getSize(); ++i)
    {
        stroke_next_points.push_back(points[i]);
    }

    if (isEqual(points[0], points[getSize() - 1]))
    {
        stroke_next_points.push_back(points[1]);
    }
    else
    {
        stroke_next_points.push_back(points[getSize() - 1]);
    }
}

inline void showPoints(std::vector<glm::vec3> pts)
{
    for (auto p : pts)
    {
        std::cout << "(" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
    }
}

inline bool intersectSegments(const glm::vec2 &p1, const glm::vec2 &p2,
                              const glm::vec2 &q1, const glm::vec2 &q2,
                              glm::vec2 &out)
{
    glm::vec2 r = p2 - p1;
    glm::vec2 s = q2 - q1;
    float rxs = r.x * s.y - r.y * s.x;
    float qpxr = (q1.x - p1.x) * r.y - (q1.y - p1.y) * r.x;

    if (rxs == 0 && qpxr == 0)
        return false; // collinear
    if (rxs == 0 && qpxr != 0)
        return false; // parallel

    float t = ((q1 - p1).x * s.y - (q1 - p1).y * s.x) / rxs;
    float u = ((q1 - p1).x * r.y - (q1 - p1).y * r.x) / rxs;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        out = p1 + t * r;
        return true;
    }

    return false;
}

inline bool crossesBaseline(float y1, float y2, float baseline = 0.0f)
{
    return (y1 - baseline) * (y2 - baseline) < 0.0f;
}

inline Point2D intersectBaseline(
    const Point2D &p1,
    const Point2D &p2,
    float baseline = 0.0f)
{
    float t = (baseline - p1.second) / (p2.second - p1.second);
    float x = p1.first + t * (p2.first - p1.first);
    return {x, baseline};
}

inline std::vector<Polygonn> buildFillPolygons(
    const std::vector<Point2D> &curve,
    float baseline = 0.0f)
{
    std::vector<Polygonn> result;
    std::vector<Point2D> current;

    glm::vec2 point1 = glm::vec2(curve[0].first, curve[0].second);
    glm::vec2 point2 = glm::vec2(curve[curve.size() - 1].first, curve[curve.size() - 1].second);

    for (int i = 0; i < (int)curve.size() - 1; ++i)
    {
        const auto &p = curve[i];
        const auto &p2 = curve[i + 1];

        glm::vec2 crossPoint;
        if (intersectSegments(point1, point2, glm::vec2(p.first, p.second), glm::vec2(p2.first, p2.second), crossPoint))
        {
            current.push_back({p.first, p.second});
            current.push_back({crossPoint.x, crossPoint.y});
            current.push_back(current.front());
            result.push_back({current});
            current.clear();
            current.push_back({crossPoint.x, crossPoint.y});
        }
        else
        {
            current.push_back(p);
        }
    }

    current.push_back(curve[curve.size() - 1]);
    current.push_back(current.front());

    result.push_back({current});
    current.clear();

    return result;
}

void GraphObject::setFillData(char axis)
{
    using Point2D = std::pair<float, float>;

    std::vector<Point2D> curve;
    current_fill_points.clear();

    for (int i = 0; i < (int)points.size(); ++i)
    {
        curve.push_back({points[i].x, points[i].y});
    }

    auto polygons = buildFillPolygons(curve, 0.0f);

    for (const auto &poly : polygons)
    {
        auto indices = mapbox::earcut<uint32_t>(poly);
        const auto &ring = poly[0];
        for (uint32_t idx : indices)
        {
            // std::cout << idx << ", ";
            current_fill_points.emplace_back(
                ring[idx].first,
                ring[idx].second,
                0.0f);
        }
    }
}

void GraphObject::applyColorToVertex()
{
    int n = getSize();
    stroke_color_array.resize(n);

    if (colors.empty())
    {
        colors = {
            GraphColor()};
    }

    int segments = colors.size() - 1;

    for (int i = 0; i < n; ++i)
    {
        float t = float(i) / float(n - 1); // 0 → 1 across vertices

        float segmentF = t * segments;
        int segment = std::min(int(segmentF), segments - 1);

        float localT = segmentF - segment;

        const auto &c0 = colors[segment];
        const auto &c1 = colors[segment + 1];

        stroke_color_array[i] = glm::vec3(
            glm::mix(c0.RED, c1.RED, localT),
            glm::mix(c0.GREEN, c1.GREEN, localT),
            glm::mix(c0.BLUE, c1.BLUE, localT));
    }
}

void GraphObject::updatePoints(float dt)
{
    points.clear();
    float p = abs(range.second - range.first) / resolution;
    float t = range.first;
    float minx = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
    for (int i = 0; i <= resolution; ++i)
    {
        Var v;
        glm::vec3 point = updater_func(t, dt);
        t += p;
        minx = std::min(minx, (point[0]));
        maxX = std::max(maxX, (point[0]));
        minY = std::min(minY, (point[1]));
        maxY = std::max(maxY, (point[1]));
        points.push_back(point);
    }

    setDimension(minx, maxX, minY, maxY);
}

void GraphObject::generatePoints(glm::vec3 (*func)(float, Var), Var v)
{
    this->func = func;
    float p = abs(range.second - range.first) / resolution;
    float t = range.first;
    float minx = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
    for (int i = 0; i <= resolution; ++i)
    {
        glm::vec3 point = func(t, v);
        t += p;
        minx = std::min(minx, (point[0]));
        maxX = std::max(maxX, (point[0]));
        minY = std::min(minY, (point[1]));
        maxY = std::max(maxY, (point[1]));
        points.push_back(point);
    }

    setDimension(minx, maxX, minY, maxY);
}

void GraphObject::UpdateGraphWithFunction(float dt)
{
    stroke_current_points.clear();
    stroke_prev_points.clear();
    stroke_next_points.clear();

    updatePoints(dt);
    setStrokeData();

    drawSize = getSize();
    drawStart = 0;

    glBindBuffer(GL_ARRAY_BUFFER, StrokeVBO);

    size_t buffer_size = getSize() * sizeof(glm::vec3);

    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, stroke_current_points.data());
    glBufferSubData(GL_ARRAY_BUFFER, buffer_size, buffer_size, stroke_prev_points.data());
    glBufferSubData(GL_ARRAY_BUFFER, 2 * buffer_size, buffer_size, stroke_next_points.data());

    setFillData();
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, getFillSize() * sizeof(glm::vec3), current_fill_points.data());
}

void GraphObject::uploadStrokeDataToShader()
{

    if (!getSize())
    {
        std::cerr << "No Points to Initialize for Stroke" << std::endl;
    }

    size_t buffer_size = getSize() * sizeof(glm::vec3);
    glBindVertexArray(StrokeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, StrokeVBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, stroke_current_points.data());
    glBufferSubData(GL_ARRAY_BUFFER, buffer_size, buffer_size, stroke_prev_points.data());
    glBufferSubData(GL_ARRAY_BUFFER, 2 * buffer_size, buffer_size, stroke_next_points.data());
    glBufferSubData(GL_ARRAY_BUFFER, 3 * buffer_size, buffer_size, stroke_color_array.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(buffer_size));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(2 * buffer_size));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(3 * buffer_size));
    glEnableVertexAttribArray(3);

    stroke_data_initialized = true;
}

void GraphObject::uploadFillDataToShader()
{
    size_t buffer_size = getFillSize() * sizeof(glm::vec3);
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, current_fill_points.data());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);
}

void GraphObject::setDimension(float minX, float maxX, float minY, float maxY)
{
    x = minX;
    y = maxY;
    width = maxX - minX;
    height = maxY - minY;
}

void GraphObject::setTranslate(glm::vec3 trans)
{
    this->translate = trans - glm::vec3(x, y, 0);
}

void GraphObject::moveTo(glm::vec3 newPos)
{
    translate = newPos - glm::vec3(x, y, 0);
    updatePoints();
}

void GraphObject::moveTo(Position pos)
{
    glm::vec3 translate = glm::vec3(0, 0, 0);
    switch (pos)
    {
    case Position::LEFT:
        translate = glm::vec3(x - width, y, 0);
        break;
    case Position::RIGHT:
        translate = glm::vec3(x + width, y, 0);
        break;
    case Position::TOP:
        translate = glm::vec3(x, y + height, 0);
        break;
    case Position::BOTTOM:
        translate = glm::vec3(x, y - height, 0);
        break;
    case Position::TOP_LEFT:
        translate = glm::vec3(x - width, y + height, 0);
        break;
    case Position::TOP_RIGHT:
        translate = glm::vec3(x + width, y + height, 0);
        break;
    case Position::BOTTOM_LEFT:
        translate = glm::vec3(x - width, y - height, 0);
        break;
    case Position::BOTTOM_RIGHT:
        translate = glm::vec3(x + width, y - height, 0);
        break;
    case Position::NONE:
        translate = glm::vec3(x + width / 2, y - height / 2, 0);
        break;
    }

    moveTo(translate);
}

void GraphObject::prepareSubObjects(float start_time, float duration)
{
    int subObjCount = subGraphObjects.size();
    setStartTime(start_time);
    setDuration(duration);
    float interval;
    float ind_duration;
    if (sub_sync || ((sub_obj_show_interval * subObjCount) > duration))
    {
        interval = duration / subObjCount;
        ind_duration = interval;
    }
    else
    {
        interval = sub_obj_show_interval;
        ind_duration = duration - interval * (subObjCount - 1);
    }

    for (int i = 0; i < subObjCount; ++i)
    {
        subGraphObjects[i]->prepareSubObjects(this->startTime + i * interval, ind_duration);
    }
}

void GraphObject::updateStrokePoints()
{
    glm::vec3 scalePivot = glm::vec3(x, y, 0) + glm::vec3(0, -height / 2, 0);
    glm::vec3 scales(scale_x, scale_y, scale_z);

    glm::vec3 rotRad = glm::radians(rotation_vec);

    glm::mat4 R(1.0f);
    R = glm::rotate(R, rotRad.z, glm::vec3(0, 0, 1));
    R = glm::rotate(R, rotRad.y, glm::vec3(0, 1, 0));
    R = glm::rotate(R, rotRad.x, glm::vec3(1, 0, 0));

    float minx = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;

    for (int i = 0; i < (int)points.size(); ++i)
    {
        glm::vec3 p = points[i];

        // ---- scale ----
        p -= scalePivot;
        p *= scales;
        p += scalePivot;

        // ---- rotate around pivot ----
        glm::vec4 hp = glm::vec4(p - rotation_pivot, 1.0f);
        hp = R * hp;
        p = glm::vec3(hp) + rotation_pivot;

        // ---- translate ----
        p += translate;

        points[i] = p;

        minx = std::min(minx, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }

    setDimension(minx, maxX, minY, maxY);
}

void GraphObject::updateFillPoints()
{
    glm::vec3 scalePivot = glm::vec3(x, y, 0) + glm::vec3(0, -height / 2, 0);
    glm::vec3 scales(scale_x, scale_y, scale_z);

    glm::vec3 rotRad = glm::radians(rotation_vec);

    glm::mat4 R(1.0f);
    R = glm::rotate(R, rotRad.z, glm::vec3(0, 0, 1));
    R = glm::rotate(R, rotRad.y, glm::vec3(0, 1, 0));
    R = glm::rotate(R, rotRad.x, glm::vec3(1, 0, 0));

    for (int i = 0; i < (int)current_fill_points.size(); ++i)
    {
        glm::vec3 p = current_fill_points[i];

        // ---- scale ----
        p -= scalePivot;
        p *= scales;
        p += scalePivot;

        // ---- rotate around pivot ----
        glm::vec4 hp = glm::vec4(p - rotation_pivot, 1.0f);
        hp = R * hp;
        p = glm::vec3(hp) + rotation_pivot;

        // ---- translate ----
        p += translate;

        current_fill_points[i] = p;
    }
}

void GraphObject::updatePoints()
{

    updateStrokePoints();
    updateFillPoints();

    translate = glm::vec3(0);
    scale_x = scale_y = scale_z = 1.0f;
    rotation_vec = glm::vec3(0);

    setStrokeData();
    uploadStrokeDataToShader();
    uploadFillDataToShader();
}

void GraphObject::scaleTo(float x_scale, float y_scale)
{
    this->scale_x = x_scale;
    this->scale_y = y_scale;
    this->scale_z = 1;
    updatePoints();
    // setStrokeData();
    // uploadStrokeDataToShader();
}

void GraphObject::scaleTo(float scale_ratio)
{
}

void GraphObject::setRotation(glm::vec3 rot_amount, glm::vec3 rot_pivot)
{
    rotation_vec = rot_amount;
    rotation_pivot = rot_pivot;
}

void GraphObject::rotate(glm::vec3 rotation_amount, glm::vec3 pivot)
{
    setRotation(rotation_amount, pivot);
    updatePoints();
}

void GraphObject::interpolate(int number)
{
    if (!is_initialized)
    {
        Init();
    }
    resolution = getSize() - 1;
    int interpolateNumber = std::max(resolution, number);

    std::cout << "Resolution is: " << resolution << " and : " << interpolateNumber << std::endl;

    int interpolationRatio = interpolateNumber / resolution;
    int interpolationQuetient = interpolateNumber % resolution;

    // showPoints(points);

    std::vector<glm::vec3> dupPoints;
    dupPoints.push_back(points[0]);
    for (int i = 0; i < getSize() - 1; ++i)
    {
        glm::vec3 norm = glm::normalize(points[i + 1] - points[i]);
        float length = glm::length(points[i + 1] - points[i]);
        // defines how many points to generate in between the points[i] and points[i+1]
        int inter = interpolationRatio + (interpolationQuetient > 0);
        interpolationQuetient--;

        float unitLength = length / (inter);
        for (int j = 1; j < inter; j++)
        {
            float curLength = unitLength * j;
            glm::vec3 pos = points[i] + norm * curLength;
            dupPoints.push_back(pos);
        }
        dupPoints.push_back(points[i + 1]);
    }
    points.clear();
    points = dupPoints;
    resolution = interpolateNumber;

    std::cout << "point size is: " << points.size() << std::endl;

    std::cout << "Fill size is: " << getFillSize() << std::endl;
    glGenVertexArrays(1, &FillVAO);
    glGenBuffers(1, &FillVBO);
    InitStrokeData();
    InitFillData();

    std::cout << "Fill size is: " << getFillSize() << std::endl;
}