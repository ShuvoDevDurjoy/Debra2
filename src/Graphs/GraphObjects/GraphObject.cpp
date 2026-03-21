#include <GraphEngine/Core/ShaderManager.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

inline void showPoints(std::vector<glm::vec3> pts)
{
    for (auto p : pts)
    {
        std::cout << "(" << p.x << "," << p.y << "," << p.z << ") , ";
    }
    std::cout << std::endl;
}

inline void showRange(std::vector<std::pair<int, int>> ranges)
{
    for (auto r : ranges)
    {
        std::cout << "(" << r.first << "," << r.second << ") , ";
    }
    std::cout << std::endl;
}

GraphObject::GraphObject()
    : StrokeVAO(0), StrokeVBO(0), FillVAO(0), FillVBO(0), CoverVAO(0), CoverVBO(0)
{
    layer = GraphApp::drawCount++;
}

void GraphObject::update(float dt)
{
    if (updaters.size())
    {
        int index = 0;
        for (void (*updater_function)(GraphObject *, float) : updaters)
        {
            if (updateStartTime[index] <= dt && ((updateEndTime[index] == -1.0f) || (dt <= updateEndTime[index])))
            {
                updater_function(this, dt - updateStartTime[index]);
            }
            index++;
        }
    }
    InitStrokeData();
}

void GraphObject::drawStroke(float dt)
{
    if (!stroke_data_initialized) return;

    glBindVertexArray(StrokeVAO);

    if (!point_sub_path_ranges.empty())
    {
        std::vector<GLint> firsts;
        std::vector<GLsizei> counts;
        firsts.reserve(point_sub_path_ranges.size());
        counts.reserve(point_sub_path_ranges.size());
        for (const auto &[first, count] : point_sub_path_ranges)
        {
            firsts.push_back(static_cast<GLint>(first));
            counts.push_back(static_cast<GLsizei>(count));
        }

        glMultiDrawArrays(GL_LINE_STRIP,
                          firsts.data(),
                          counts.data(),
                          static_cast<GLsizei>(firsts.size()));
    }
    else
    {
        // Single continuous path (or non-bezier path)
        glDrawArrays(GL_LINE_STRIP, drawStart, getSize());
    }
}

void GraphObject::drawFill(float dt)
{
    glBindVertexArray(StrokeVAO);
    if (!point_sub_path_ranges.empty())
    {
        std::vector<GLint> firsts;
        std::vector<GLsizei> counts;
        firsts.reserve(point_sub_path_ranges.size());
        counts.reserve(point_sub_path_ranges.size());
        for (const auto &[first, count] : point_sub_path_ranges)
        {
            firsts.push_back(static_cast<GLint>(first));
            counts.push_back(static_cast<GLsizei>(count));
        }
        glMultiDrawArrays(GL_TRIANGLE_FAN,
                          firsts.data(),
                          counts.data(),
                          static_cast<GLsizei>(firsts.size()));
    }
    else
    {
        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)getSize());
    }
}

void GraphObject::Init()
{

    if (is_initialized)
        return;

    generatePoints();

    glGenVertexArrays(1, &FillVAO);
    glGenBuffers(1, &FillVBO);
    glGenVertexArrays(1, &FillVAO);
    glGenBuffers(1, &FillVBO);

    // Initialize OpenGL Buffer and Array
    glGenVertexArrays(1, &StrokeVAO);
    glGenBuffers(1, &StrokeVBO);

    initializeStrokeShader();
    if (is_bezier_path && bezier_dirty)
    {
        build_points_from_bezier();
    }
    InitStrokeData();
    InitSubObject();
    // initialize the stroke data
    resolution = getSize() - 1;
    drawSize = getSize();
    drawStart = 0;
    is_initialized = true;
}

void GraphObject::InitStrokeData()
{
    if (stroke_dirty)
    {
        setStrokeData();
        applyColorToVertex();
        glBindVertexArray(StrokeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, StrokeVBO);
        uploadStrokeDataToShader();
    }
    stroke_dirty = false;
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
    ShaderManager *sm = ShaderManager::getInstance();

    if (!stroke_shader)
        this->stroke_shader = sm->getShader(vertexShaderPath, geometricShaderPath, fragmentShaderPath);
    if (!fill_shader)
        this->fill_shader = sm->getShader(fillVertexShaderPath, fillGeometricShaderPath, fillFragmentShaderPath);

    // Load stencil shaders for two-pass fill, now including geometry shader for smooth sweeping
    if (!stencil_shader)
        this->stencil_shader = sm->getShader(stencilVertexPath, "./shaders/stencil_fill/geometry.gs", stencilFragPath);
    if (!cover_shader)
        this->cover_shader = sm->getShader(stencilVertexPath, "./shaders/stencil_fill/geometry.gs", coverFragPath);
}
void GraphObject::initializeFillShader()
{
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    int buffer_size = getFillSize() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
}

void GraphObject::InitSubObject()
{
    for (GraphMathObject *subObj : subGraphObjects)
    {
        subObj->Init();
    }
}

void GraphObject::setStrokeData()
{
    if (points.empty())
        return;

    stroke_current_points.clear();
    stroke_prev_points.clear();
    stroke_next_points.clear();

    // If no sub-path ranges were defined (e.g., not a bezier path),
    // treat the whole thing as one range.
    if (point_sub_path_ranges.empty())
    {
        point_sub_path_ranges.push_back({0, (int)points.size()});
    }

    std::vector<glm::vec3> dup;
    std::vector<std::pair<int, int>> ranges;
    for (const auto &range : point_sub_path_ranges)
    {
        int start = range.first;
        int count = range.second;
        int end = start + count - 1;

        int range_start = dup.size();

        bool isClosed = false;
        if (count > 2 && isEqual(points[start], points[end]))
        {
            isClosed = true;
        }

        for (int i = start; i <= end; ++i)
        {
            // 1. Current Point

            // 2. Previous Point: If at start of sub-path, and closed, wrap around to point before end
            if (i == start)
            {
                if (isClosed)
                {
                    dup.push_back(points[end - 1]);
                }
                else
                {
                    dup.push_back(points[i]);
                }
            }

            dup.push_back(points[i]);

            // 3. Next Point: If at end of sub-path, and closed, wrap around to point after start
            if (i == end)
            {
                if (isClosed)
                {
                    dup.push_back(points[start + 1]);
                }
                else
                {
                    dup.push_back(points[i]);
                }
            }
        }

        ranges.push_back({range_start, count});
    }

    points = dup;
    point_sub_path_ranges = ranges;

    stroke_dirty = false;
    updateDimensions();
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

void GraphObject::setFillData()
{
    // Generate fine points from bezier curves to get a perfectly curved fill
    if (is_bezier_path)
    {
        build_points_from_bezier();
    }

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
    int n = getPointsSize();
    stroke_color_array.resize(n);

    // std::cout << "Graph Color size is: " << colors.size() << std::endl;

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

    // std::cout << "Graph size: " << getPointsSize() << std::endl;
    // std::cout << "Stroke color array size: " << stroke_color_array.size() << std::endl;
}

void GraphObject::applyUpdaterFunction(float dt)
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

    setDimension(minx, maxX, minY, maxY, 0, 0); // 2D updater for now
}

void GraphObject::makeSmooth()
{
    setSmooth(true);
    build_points_from_bezier();

    // Update dimensions after smooth points are generated
    if (!points.empty())
    {
        float minx = std::numeric_limits<float>::max(), maxX = -std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max(), maxY = -std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max(), maxZ = -std::numeric_limits<float>::max();

        for (const auto &p : points)
        {
            minx = std::min(minx, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
            minZ = std::min(minZ, p.z);
            maxZ = std::max(maxZ, p.z);
        }

        setDimension(minx, maxX, minY, maxY, minZ, maxZ);
        updatePoints();
    }
}

void GraphObject::UpdateGraphWithFunction(float dt)
{
    stroke_current_points.clear();
    stroke_prev_points.clear();
    stroke_next_points.clear();

    applyUpdaterFunction(dt);
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

    size_t buffer_size = getPointsSize() * sizeof(glm::vec3);
    if (StrokeVAO == 0)
        return;
    glBindVertexArray(StrokeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, StrokeVBO);

    // std::cout << "setting buffer size: " << buffer_size * 2 << std::endl;
    // std::cout << "Stroke color size: " << stroke_color_array.size() << std::endl;
    // std::cout << "Stroke color size: " << points.size() << std::endl;
    glBufferData(GL_ARRAY_BUFFER, 2 * buffer_size, NULL, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, points.data());
    glBufferSubData(GL_ARRAY_BUFFER, buffer_size, buffer_size, stroke_color_array.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(buffer_size));
    glEnableVertexAttribArray(3);

    stroke_data_initialized = true;
}

void GraphObject::uploadFillDataToShader()
{
    size_t buffer_size = getFillSize() * sizeof(glm::vec3);
    if (FillVAO == 0)
        return;
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, current_fill_points.data());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);
}

void GraphObject::setDimension(float minX, float maxX, float minY, float maxY)
{
    setDimension(minX, maxX, minY, maxY, 0, 0);
}

void GraphObject::setDimension(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
    x = minX;
    y = maxY;
    z = minZ;
    width = maxX - minX;
    height = maxY - minY;
    depth = maxZ - minZ;
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
    float minZ = FLT_MAX, maxZ = -FLT_MAX;

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
        minZ = std::min(minZ, p.z);
        maxZ = std::max(maxZ, p.z);
    }

    setDimension(minx, maxX, minY, maxY, minZ, maxZ);
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
    // updateFillPoints();

    translate = glm::vec3(0);
    scale_x = scale_y = scale_z = 1.0f;
    rotation_vec = glm::vec3(0);

    setStrokeData();
    uploadStrokeDataToShader();
    // uploadFillDataToShader();
    updateDimensions();
}

void GraphObject::linearInterpolate(int number)
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
}

std::vector<glm::vec3> GraphObject::linearInterpolate(const std::vector<glm::vec3> pts, int interpolation_count)
{
    int point_size = pts.size();

    // If already enough points, return original
    if (point_size >= interpolation_count)
        return pts;

    // If only one point, duplicate it
    if (point_size == 1)
        return std::vector<glm::vec3>(interpolation_count, pts[0]);

    int segments = point_size - 1;

    int interpolationRatio = (interpolation_count - 1) / segments;
    int interpolationQuotient = (interpolation_count - 1) % segments;

    std::vector<glm::vec3> dup_points;
    dup_points.reserve(interpolation_count);

    dup_points.push_back(pts[0]);

    for (int i = 0; i < segments; ++i)
    {
        glm::vec3 diff = pts[i + 1] - pts[i];
        float length = glm::length(diff);

        // Avoid normalize(0,0,0)
        glm::vec3 norm;
        if (length == 0.0f)
            norm = glm::vec3(0.0f);
        else
            norm = diff / length;

        int inter = interpolationRatio;

        if (interpolationQuotient > 0)
        {
            inter++;
            interpolationQuotient--;
        }

        // Ensure at least one division
        inter = std::max(inter, 1);

        float unitLength = (length == 0.0f) ? 0.0f : length / inter;

        for (int j = 1; j < inter; j++)
        {
            glm::vec3 pos = pts[i] + norm * (unitLength * j);
            dup_points.push_back(pos);
        }

        dup_points.push_back(pts[i + 1]);
    }

    return dup_points;
}

void GraphObject::functionalInterpolate(int number)
{
    if (!is_initialized)
    {
        Init();
    }
    resolution = getSize() - 1;
    int interpolateNumber = std::max(resolution, number);

    points.clear();
    float t = range.first;
    float p = abs(range.second - range.first) / interpolateNumber;
    float minx = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;

    for (int i = 0; i <= interpolateNumber; ++i)
    {
        glm::vec3 point = func(t, func_var);
        t += p;
        minx = std::min(minx, (point[0]));
        maxX = std::max(maxX, (point[0]));
        minY = std::min(minY, (point[1]));
        maxY = std::max(maxY, (point[1]));
        points.push_back(point);
    }
    resolution = interpolateNumber;
    setDimension(minx, maxX, minY, maxY, 0, 0); // 2D func for now
}

void GraphObject::alignPoints(GraphMathObject *target)
{
    // Ensure points are generated for bezier-based objects before alignment
    if (this->is_bezier_path && this->bezier_dirty)
        this->build_points_from_bezier();
    if (target->is_bezier_path && target->bezier_dirty)
        target->build_points_from_bezier();

    // If neither has points/ranges (like a parent Text object), skip alignment
    if (this->point_sub_path_ranges.empty() && target->point_sub_path_ranges.empty())
        return;

    auto extract_subpaths = [](GraphMathObject &g)
    {
        std::vector<std::vector<glm::vec3>> result;
        for (auto &r : g.point_sub_path_ranges)
        {
            int start = r.first;
            int count = r.second;
            std::vector<glm::vec3> sub;
            sub.reserve(count);
            for (int i = 0; i < count; i++)
                sub.push_back(g.points[start + i]);
            result.push_back(sub);
        }
        return result;
    };

    auto resample_curve = [](const std::vector<glm::vec3> &pts, int N)
    {
        if ((int)pts.size() == N)
            return pts;
        if (pts.empty())
            return std::vector<glm::vec3>(N, glm::vec3(0));
        if (pts.size() == 1)
            return std::vector<glm::vec3>(N, pts[0]);

        std::vector<glm::vec3> result;
        result.reserve(N);

        int S = (int)pts.size() - 1;
        int D = N - (int)pts.size();

        int base_add = D / S;
        int extra_add = D % S;

        for (int i = 0; i < S; i++)
        {
            result.push_back(pts[i]);
            int n_to_add = base_add + (i < extra_add ? 1 : 0);
            for (int j = 1; j <= n_to_add; j++)
            {
                float t = (float)j / (float)(n_to_add + 1);
                result.push_back(glm::mix(pts[i], pts[i + 1], t));
            }
        }
        result.push_back(pts.back());
        return result;
    };

    auto A_paths = extract_subpaths(*this);
    auto B_paths = extract_subpaths(*target);

    int maxPaths = std::max((int)A_paths.size(), (int)B_paths.size());

    // Growth strategy: if one has fewer subpaths, they "grow" out of the last point
    while ((int)A_paths.size() < maxPaths)
    {
        glm::vec3 p = A_paths.empty() ? glm::vec3(0) : A_paths.back().back();
        A_paths.push_back({p, p}); // Zero-length segment at the end
    }
    while ((int)B_paths.size() < maxPaths)
    {
        glm::vec3 p = B_paths.empty() ? glm::vec3(0) : B_paths.back().back();
        B_paths.push_back({p, p});
    }

    std::vector<glm::vec3> finalA, finalB;
    std::vector<std::pair<int, int>> finalRanges;
    int currentOffset = 0;

    for (int i = 0; i < maxPaths; i++)
    {
        int targetN = std::max((int)A_paths[i].size(), (int)B_paths[i].size());
        auto resA = resample_curve(A_paths[i], targetN);
        auto resB = resample_curve(B_paths[i], targetN);

        finalA.insert(finalA.end(), resA.begin(), resA.end());
        finalB.insert(finalB.end(), resB.begin(), resB.end());
        finalRanges.push_back({currentOffset, targetN});
        currentOffset += targetN;
    }

    this->points = std::move(finalA);
    this->point_sub_path_ranges = finalRanges;

    target->points = std::move(finalB);
    target->point_sub_path_ranges = finalRanges;

    // Synchronize secondary data
    this->applyColorToVertex();
    target->applyColorToVertex();
    this->setStrokeData();
    target->setStrokeData();
}

void GraphObject::interpolate(const GraphMathObject *target, float t)
{
    if (points.size() != target->points.size())
    {
        // Fallback or skip if sizes don't match (should be handled by alignPoints)
        return;
    }
    for (size_t i = 0; i < points.size(); i++)
    {
        points[i] = glm::mix(points[i], target->points[i], t);
    }
    updateDimensions();
}

void GraphObject::become(GraphMathObject *target)
{
    if (!target->is_initialized)
        target->Init();

    this->points = target->points;
    this->point_sub_path_ranges = target->point_sub_path_ranges;
    this->stroke_colors = target->stroke_colors;
    stroke_dirty = true;
    InitStrokeData();
}

glm::vec3 GraphObject::getPosition(Position pos)
{
    glm::vec3 position;
    switch (pos)
    {
    case LEFT:
        position = glm::vec3(x, y - height / 2.0f, 0);
        break;
    case RIGHT:
        position = glm::vec3(x + width, y - height / 2.0f, 0);
        break;
    case TOP_LEFT:
        position = glm::vec3(x, y, 0);
        break;
    case BOTTOM_LEFT:
        position = glm::vec3(x, y - height, 0);
        break;
    case TOP_RIGHT:
        position = glm::vec3(x + width, y, 0);
        break;
    case BOTTOM_RIGHT:
        position = glm::vec3(x + width, y - height, 0);
        break;
    case TOP:
        position = glm::vec3(x + width / 2.0f, y, 0);
        break;
    case BOTTOM:
        position = glm::vec3(x + width / 2.0f, y - height, 0);
        break;
    case CENTER:
        position = glm::vec3(x + width / 2.0f, y - height / 2.0f, 0);
        break;
    case NONE:
        position = glm::vec3(x, y, 0);
        break;
    }

    return position;
}

glm::vec3 GraphObject::getRelativePosition(glm::vec3 targetPosition, Position pos, glm::vec3 buffer)
{
    std::cout << "Position: " << std::endl;
    std::cout << targetPosition.x << ", " << targetPosition.y << ", " << targetPosition.z << std::endl;
    glm::vec3 objects_position = getPosition(pos);
    std::cout << objects_position.x << ", " << objects_position.y << ", " << objects_position.z << std::endl;
    glm::vec3 temp_pos = objects_position - getPosition(Position::NONE);
    std::cout << temp_pos.x << ", " << temp_pos.y << ", " << temp_pos.z << std::endl;
    temp_pos = targetPosition - temp_pos;
    std::cout << temp_pos.x << ", " << temp_pos.y << ", " << temp_pos.z << std::endl;

    return temp_pos - getPosition(Position::NONE) + buffer;
}

void GraphObject::nextTo(GraphObject *target, Position pos, float buffer)
{
    glm::vec3 position = target->getPosition(pos);

    switch (pos)
    {
    case LEFT:
        position = getRelativePosition(position, Position::RIGHT, glm::vec3(-buffer, 0, 0));
        break;
    case RIGHT:
        position = getRelativePosition(position, Position::LEFT, glm::vec3(buffer, 0, 0));
        break;
    case TOP:
        position = getRelativePosition(position, Position::BOTTOM, glm::vec3(0, buffer, 0));
        break;
    case BOTTOM:
        position = getRelativePosition(position, Position::TOP, glm::vec3(0, -buffer, 0));
        break;
    case TOP_LEFT:
        position = getRelativePosition(position, Position::BOTTOM_RIGHT, glm::vec3(-buffer, buffer, 0));
        break;
    case TOP_RIGHT:
        position = getRelativePosition(position, Position::BOTTOM_LEFT, glm::vec3(buffer, buffer, 0));
        break;
    case BOTTOM_LEFT:
        position = getRelativePosition(position, Position::TOP_RIGHT, glm::vec3(-buffer, -buffer, 0));
        break;
    case BOTTOM_RIGHT:
        position = getRelativePosition(position, Position::TOP_LEFT, glm::vec3(buffer, -buffer, 0));
        break;
    case CENTER:
        position = getRelativePosition(position, Position::CENTER, glm::vec3(0, 0, 0));
        break;
    case NONE:
        position = getRelativePosition(position, Position::NONE, glm::vec3(x, y, 0));
        break;
    }

    translate = position;
    // setTranslate(position);
    updatePoints();
}

void GraphObject::start_bezier_path(glm::vec3 start_point)
{
    // Fully reset the bezier path (clear the old one before starting a new object)
    if (bezier_points.empty())
    {
        bezier_sub_path_starts.clear();
    }
    // Delegate to base — it handles both first-time and subsequent sub-paths correctly
    GraphMathObject::start_bezier_path(start_point);
}

void GraphObject::close_path()
{
    if (!bezier_points.empty())
    {
        // Connect the last point to the first point of the current sub-path
        int start_index = bezier_sub_path_starts.back();
        // glm::vec3 start_point = bezier_points[bezier_sub_path_starts.back()];
        add_line_to(bezier_points[start_index]);
    }
}

void GraphObject::add_cubic_bezier_curve_to(glm::vec3 control1, glm::vec3 control2, glm::vec3 end_anchor)
{
    if (bezier_points.empty())
    {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    bezier_points.push_back(control1);
    bezier_points.push_back(control2);
    bezier_points.push_back(end_anchor);
}

void GraphObject::add_quadratic_bezier_curve_to(glm::vec3 control, glm::vec3 end_anchor)
{
    // Convert quadratic to cubic bezier for unified storage
    if (bezier_points.empty())
    {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    glm::vec3 start_anchor = bezier_points.back();

    // Control1 = Start + 2/3 * (Control - Start)
    glm::vec3 control1 = start_anchor + (2.0f / 3.0f) * (control - start_anchor);
    // Control2 = End + 2/3 * (Control - End)
    glm::vec3 control2 = end_anchor + (2.0f / 3.0f) * (control - end_anchor);

    add_cubic_bezier_curve_to(control1, control2, end_anchor);
}

void GraphObject::add_line_to(glm::vec3 end_anchor)
{
    if (bezier_points.empty())
    {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    glm::vec3 start_anchor = bezier_points.back();

    // For a straight line, controls are 1/3 and 2/3 along the line
    glm::vec3 control1 = start_anchor + (1.0f / 3.0f) * (end_anchor - start_anchor);
    glm::vec3 control2 = start_anchor + (2.0f / 3.0f) * (end_anchor - start_anchor);

    add_cubic_bezier_curve_to(control1, control2, end_anchor);
}

void GraphObject::subdivide_bezier_curves()
{
    // Delegate to base class — it handles sub-path-aware subdivision
    // and populates point_sub_path_ranges for glMultiDrawArrays.
    GraphMathObject::subdivide_bezier_curves();
}

void GraphObject::build_points_from_bezier()
{
    if (is_bezier_path && bezier_dirty)
    {
        subdivide_bezier_curves();
        bezier_dirty = false;
    }
    stroke_dirty = true;
}

std::vector<glm::vec3> GraphObject::getAllBezierPoints()
{
    std::vector<glm::vec3> all_pts = bezier_points;
    for (GraphMathObject *child : subGraphObjects)
    {
        std::vector<glm::vec3> child_pts = child->getAllBezierPoints();
        all_pts.insert(all_pts.end(), child_pts.begin(), child_pts.end());
    }
    return all_pts;
}

void GraphObject::setAllBezierPoints(const std::vector<glm::vec3> &pts)
{
    if (subGraphObjects.empty())
    {
        bezier_points = pts;
        is_bezier_path = !bezier_points.empty();
        build_points_from_bezier();
    }
    else
    {
        size_t offset = 0;
        for (GraphMathObject *child : subGraphObjects)
        {
            std::vector<glm::vec3> child_current = child->getAllBezierPoints();
            size_t child_count = child_current.size();
            if (offset + child_count <= pts.size())
            {
                std::vector<glm::vec3> child_subset(pts.begin() + offset, pts.begin() + offset + child_count);
                child->setAllBezierPoints(child_subset);
                offset += child_count;
            }
        }
    }
}
