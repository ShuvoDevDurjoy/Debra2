#include <GraphEngine/Scene/Scene.hpp>
#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>
#include <GraphEngine/Core/ShaderManager.hpp>

void ThreeDObject::Init()
{
    if (is_initialized) return;

    // Default to a single sub-surface if none are specified yet
    if (sub_surface_ranges.empty()) {
        sub_surface_ranges.push_back({0, (int)points.size()});
    }

    if (!getSize()) {
        generatePoints();
    }

    if (!getSize()) {
        std::cerr << "ThreeDObject: No points in the graph to initialize" << std::endl;
        return;
    }

    // Initialize Fill VAO, VBO, EBO
    glGenVertexArrays(1, &FillVAO);
    glGenBuffers(1, &FillVBO);
    glGenBuffers(1, &FillEBO);
    InitFillData();

    // Initialize Stroke VAO, VBO, EBO
    glGenVertexArrays(1, &StrokeVAO);
    glGenBuffers(1, &StrokeVBO);
    glGenBuffers(1, &StrokeEBO);
    InitStrokeData();

    is_initialized = true;
}

void ThreeDObject::setPoints(std::vector<glm::vec3> points)
{
    this->points = points;
}

void ThreeDObject::generatePoints()
{
    if (graph_func == nullptr) {
        return;
    }
    
    points.clear();
    normals.clear();
    float increment_rho = (rho_range.second - rho_range.first) / std::max(1.0f, resolution.first - 1.0f);
    float increment_theta = (theta_range.second - theta_range.first) / std::max(1.0f, resolution.second - 1.0f);
    
    for (int i = 0; i < resolution.first; ++i) {
        float rho = rho_range.first + i * increment_rho;
        for (int j = 0; j < resolution.second; ++j) {
            float theta = theta_range.first + j * increment_theta;
            glm::vec3 point = graph_func(rho, theta, graph_var);
            points.push_back(point);

            float eps = 0.001f;
            glm::vec3 p_rho = graph_func(rho + eps, theta, graph_var);
            glm::vec3 p_theta = graph_func(rho, theta + eps, graph_var);
            glm::vec3 dp_drho = p_rho - point;
            glm::vec3 dp_dtheta = p_theta - point;
            
            glm::vec3 n(0.0f);
            if(glm::length(dp_drho) > 0.0001f && glm::length(dp_dtheta) > 0.0001f) {
                n = glm::normalize(glm::cross(dp_drho, dp_dtheta));
            }
            if(glm::length(n) < 0.001f) {
                n = glm::vec3(0,0,1); // fallback
            }
            normals.push_back(n);
        }
    }

    sub_surface_ranges.clear();
    sub_surface_ranges.push_back({0, (int)resolution.first * (int)resolution.second});
    updateDimensions();
}

void ThreeDObject::InitStrokeData() {
    setStrokeData();
    initializeStrokeShader();
    uploadStrokeDataToShader();
}

void ThreeDObject::InitFillData()
{
    setFillData();
    initializeFillShader();
    uploadFillDataToShader();
}

void ThreeDObject::initializeStrokeShader() {
    ShaderManager* sm = ShaderManager::getInstance();
    if (!stroke_shader) {
        stroke_shader = sm->getShader(vertexShaderPath, geometricShaderPath, fragmentShaderPath);
    }
}

void ThreeDObject::initializeFillShader()
{
    ShaderManager* sm = ShaderManager::getInstance();
    if (!fill_shader) {
        fill_shader = sm->getShader(fillVertexShaderPath, fillGeometricShaderPath, fillFragmentShaderPath);
    }
}

void ThreeDObject::uploadStrokeDataToShader() {
    if (StrokeVAO == 0) return;
    glBindVertexArray(StrokeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, StrokeVBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, StrokeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, stroke_indices.size() * sizeof(uint32_t), stroke_indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void ThreeDObject::uploadFillDataToShader()
{
    if (FillVAO == 0) return;
    glBindVertexArray(FillVAO);

    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    if (normals.size() == points.size()) {
        if (NormalsVBO == 0) glGenBuffers(1, &NormalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(1);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FillEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fill_indices.size() * sizeof(uint32_t), fill_indices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void ThreeDObject::setStrokeData()
{
    int R = (int)resolution.first;
    int T = (int)resolution.second;

    if (sub_surface_ranges.size() == 1 && sub_surface_ranges[0].second == R * T) {
        stroke_indices.clear();
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < T; j++) {
                int p00 = i * T + j;
                int p10 = (i + 1) * T + j;
                int p01 = i * T + (j + 1);

                if (i < R - 1) {
                    stroke_indices.push_back(p00);
                    stroke_indices.push_back(p10);
                } else if (is_periodic_rho) {
                    stroke_indices.push_back(p00);
                    stroke_indices.push_back(j); // Wrap to row 0
                }
                
                if (j < T - 1) {
                    stroke_indices.push_back(p00);
                    stroke_indices.push_back(p01);
                } else if (is_periodic_theta) {
                    stroke_indices.push_back(p00);
                    stroke_indices.push_back(i * T); // Wrap to start of row
                }
            }
        }
    } else {
        // If constructed via sub paths, we just draw wireframes from the fill indices.
        for (size_t i = 0; i < fill_indices.size(); i += 3) {
            stroke_indices.push_back(fill_indices[i]);
            stroke_indices.push_back(fill_indices[i+1]);

            stroke_indices.push_back(fill_indices[i+1]);
            stroke_indices.push_back(fill_indices[i+2]);

            stroke_indices.push_back(fill_indices[i+2]);
            stroke_indices.push_back(fill_indices[i]);
        }
    }
}

void ThreeDObject::setFillData()
{
    if (sub_surface_ranges.size() == 1 && sub_surface_ranges[0].second == resolution.first * resolution.second) {
        fill_indices.clear();
        
        int R = (int)resolution.first;
        int T = (int)resolution.second;

        // Triangulate grid
        for (int i = 0; i < R - 1; i++) {
            for (int j = 0; j < T - 1; j++) {
                int p00 = i * T + j;
                int p10 = (i + 1) * T + j;
                int p01 = i * T + (j + 1);
                int p11 = (i + 1) * T + (j + 1);

                fill_indices.push_back(p00); fill_indices.push_back(p01); fill_indices.push_back(p10);
                fill_indices.push_back(p10); fill_indices.push_back(p01); fill_indices.push_back(p11);
            }

            // Wrap around Theta (columns)
            if (is_periodic_theta) {
                int p0_last = i * T + (T - 1);
                int p1_last = (i + 1) * T + (T - 1);
                int p0_first = i * T + 0;
                int p1_first = (i + 1) * T + 0;

                fill_indices.push_back(p0_last); fill_indices.push_back(p0_first); fill_indices.push_back(p1_last);
                fill_indices.push_back(p1_last); fill_indices.push_back(p0_first); fill_indices.push_back(p1_first);
            }
        }

        // Wrap around Rho (rows)
        if (is_periodic_rho) {
            for (int j = 0; j < T - 1; j++) {
                int p_last0 = (R - 1) * T + j;
                int p_last1 = (R - 1) * T + (j + 1);
                int p_first0 = 0 * T + j;
                int p_first1 = 0 * T + (j + 1);

                fill_indices.push_back(p_last0); fill_indices.push_back(p_last1); fill_indices.push_back(p_first0);
                fill_indices.push_back(p_first0); fill_indices.push_back(p_last1); fill_indices.push_back(p_first1);
            }
            
            // Corner case: Wrap both
            if (is_periodic_theta) {
                int p_last_last = (R - 1) * T + (T - 1);
                int p_last_first = (R - 1) * T + 0;
                int p_first_last = 0 * T + (T - 1);
                int p_first_first = 0 * T + 0;

                fill_indices.push_back(p_last_last); fill_indices.push_back(p_last_first); fill_indices.push_back(p_first_last);
                fill_indices.push_back(p_first_last); fill_indices.push_back(p_last_first); fill_indices.push_back(p_first_first);
            }
        }
    }
}

int ThreeDObject::getSize()
{
    return (int)points.size();
}

void ThreeDObject::addPoints(glm::vec3 point)
{
    points.push_back(point);
}

void ThreeDObject::setFillProgress(float progress)
{
    fillProgress = progress;
}

void ThreeDObject::updatePoints() {
    // 1. Compute transformation matrices
    glm::mat4 delta(1.0f);
    delta = glm::translate(delta, translate);
    delta = glm::translate(delta, rotation_pivot);
    delta = glm::rotate(delta, glm::radians(rotation_vec.z), glm::vec3(0, 0, 1));
    delta = glm::rotate(delta, glm::radians(rotation_vec.y), glm::vec3(0, 1, 0));
    delta = glm::rotate(delta, glm::radians(rotation_vec.x), glm::vec3(1, 0, 0));
    delta = glm::translate(delta, -rotation_pivot);
    delta = glm::scale(delta, scale_factor);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(delta)));

    // 2. Transform points and normals on CPU
    for (auto& p : points) {
        p = glm::vec3(delta * glm::vec4(p, 1.0f));
    }
    for (auto& n : normals) {
        n = glm::normalize(normalMatrix * n);
    }

    // 3. Re-upload data to GPU
    uploadStrokeDataToShader();
    uploadFillDataToShader();

    // 4. Reset transform members (BAKING complete)
    translate = glm::vec3(0.0f);
    rotation_vec = glm::vec3(0.0f);
    scale_factor = glm::vec3(1.0f);
    
    // 5. Propagate to sub-objects
    for (auto subObj : subGraphObjects) {
        subObj->updatePoints();
    }
    updateDimensions();
}

void ThreeDObject::drawStroke(float dt){
    if (stroke_indices.empty()) return;
    glBindVertexArray(StrokeVAO);
    // Render lines; a geometry shader will expand them to thick squares (quads).
    glDrawElements(GL_LINES, stroke_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ThreeDObject::drawFill(float dt){
    if (fill_indices.empty()) return;
    glBindVertexArray(FillVAO);
    glDrawElements(GL_TRIANGLES, fill_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ThreeDObject::update(float dt){
    if (updaters.size())
    {
        int index = 0;
        for (void (*updater_function)(ThreeDObject *, float) : updaters)
        {
            if (updateStartTime[index] <= dt && ((updateEndTime[index] == -1.0f) || (dt <= updateEndTime[index])))
            {
                updater_function(this, dt - updateStartTime[index]);
            }
            index++;
        }
    }
}

void ThreeDObject::applyUpdaterFunction(float dt){}

void ThreeDObject::updateStrokePoints() {}
void ThreeDObject::updateFillPoints() {}

void ThreeDObject::interpolate(const GraphMathObject *target, float t)
{
    const ThreeDObject *t3d = dynamic_cast<const ThreeDObject *>(target);
    if (!t3d)
        return;

    size_t n = points.size();
    if (n != t3d->points.size())
        return;

    if (m_original_points.size() != n)
    {
        m_original_points = points;
    }

    for (size_t i = 0; i < n; i++)
    {
        points[i] = glm::mix(m_original_points[i], t3d->points[i], t);
    }

    if (normals.size() == n && t3d->normals.size() == n)
    {
        if (m_original_normals.size() != n)
        {
            m_original_normals = normals;
        }
        for (size_t i = 0; i < n; i++)
        {
            normals[i] = glm::normalize(glm::mix(m_original_normals[i], t3d->normals[i], t));
        }
    }

    uploadStrokeDataToShader();
    uploadFillDataToShader();
}

void ThreeDObject::alignPoints(GraphMathObject *target)
{
    ThreeDObject *t3d = dynamic_cast<ThreeDObject *>(target);
    if (!t3d)
        return;

    if (!t3d->is_initialized)
        t3d->Init();
    if (!this->is_initialized)
        this->Init();

    auto extract_subsurfaces = [](ThreeDObject &g)
    {
        std::vector<std::vector<glm::vec3>> pts;
        std::vector<std::vector<glm::vec3>> nms;
        
        // Smarter extraction: if it's a single range that matches a grid, split it into rows
        bool is_grid = (g.sub_surface_ranges.size() == 1 && 
                       g.sub_surface_ranges[0].second == (int)g.resolution.first * (int)g.resolution.second);
        
        if (is_grid) {
            for (int i = 0; i < (int)g.resolution.first; i++) {
                int start = i * (int)g.resolution.second;
                int count = (int)g.resolution.second;
                std::vector<glm::vec3> sub_pts;
                std::vector<glm::vec3> sub_nms;
                for (int j = 0; j < count; j++) {
                    sub_pts.push_back(g.points[start + j]);
                    if (!g.normals.empty()) sub_nms.push_back(g.normals[start + j]);
                }
                pts.push_back(sub_pts);
                nms.push_back(sub_nms);
            }
        } else {
            for (auto &r : g.sub_surface_ranges)
            {
                int start = r.first;
                int count = r.second;
                std::vector<glm::vec3> sub_pts;
                std::vector<glm::vec3> sub_nms;
                for (int i = 0; i < count; i++) {
                    sub_pts.push_back(g.points[start + i]);
                    if (!g.normals.empty()) sub_nms.push_back(g.normals[start + i]);
                }
                pts.push_back(sub_pts);
                nms.push_back(sub_nms);
            }
        }
        return std::make_pair(pts, nms);
    };

    auto resample_cloud = [](const std::vector<glm::vec3> &pts, int N)
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

    auto resample_normals = [](const std::vector<glm::vec3> &nms, int N)
    {
        if ((int)nms.size() == N)
            return nms;
        if (nms.empty())
            return std::vector<glm::vec3>(N, glm::vec3(0, 0, 1));
        if (nms.size() == 1)
            return std::vector<glm::vec3>(N, nms[0]);

        std::vector<glm::vec3> result;
        result.reserve(N);

        int S = (int)nms.size() - 1;
        int D = N - (int)nms.size();

        int base_add = D / S;
        int extra_add = D % S;

        for (int i = 0; i < S; i++)
        {
            result.push_back(nms[i]);
            int n_to_add = base_add + (i < extra_add ? 1 : 0);
            for (int j = 1; j <= n_to_add; j++)
            {
                float t = (float)j / (float)(n_to_add + 1);
                result.push_back(glm::normalize(glm::mix(nms[i], nms[i + 1], t)));
            }
        }
        result.push_back(nms.back());
        return result;
    };

    auto resample_surfaces = [](const std::vector<std::vector<glm::vec3>>& src_pts, 
                                const std::vector<std::vector<glm::vec3>>& src_nms, 
                                int M_target)
    {
        int M_src = (int)src_pts.size();
        std::vector<std::vector<glm::vec3>> out_pts;
        std::vector<std::vector<glm::vec3>> out_nms;
        out_pts.reserve(M_target);
        out_nms.reserve(M_target);

        if (M_src == 0) {
            std::vector<glm::vec3> empty_row_p; // filled later
            std::vector<glm::vec3> empty_row_n;
            for(int i=0; i<M_target; ++i) { out_pts.push_back(empty_row_p); out_nms.push_back(empty_row_n); }
            return std::make_pair(out_pts, out_nms);
        }

        if (M_src == 1) {
            for(int i=0; i<M_target; ++i) { out_pts.push_back(src_pts[0]); out_nms.push_back(src_nms[0]); }
            return std::make_pair(out_pts, out_nms);
        }

        // Resample the rows (surfaces)
        for (int i = 0; i < M_target; i++)
        {
            float t = (M_target > 1) ? (float)i / (float)(M_target - 1) : 0.0f;
            float idx_f = t * (M_src - 1);
            int idx = (int)idx_f;
            float local_t = idx_f - (float)idx;

            if (idx >= M_src - 1) {
                out_pts.push_back(src_pts[M_src - 1]);
                out_nms.push_back(src_nms[M_src - 1]);
            } else {
                const auto& r1 = src_pts[idx];
                const auto& r2 = src_pts[idx+1];
                const auto& n1 = src_nms[idx];
                const auto& n2 = src_nms[idx+1];
                
                // Interpolate row r1 and r2 (must match point count for row-resampling,
                // but we'll do row-level mix here and then point-level resample later).
                // Actually, let's keep it simple: out_pts[i] = r1 mixed with r2 if same size.
                if (r1.size() == r2.size() && !r1.empty()) {
                    std::vector<glm::vec3> row_p;
                    std::vector<glm::vec3> row_n;
                    for(size_t j=0; j<r1.size(); ++j) {
                        row_p.push_back(glm::mix(r1[j], r2[j], local_t));
                        row_n.push_back(glm::normalize(glm::mix(n1[j], n2[j], local_t)));
                    }
                    out_pts.push_back(row_p);
                    out_nms.push_back(row_n);
                } else {
                    out_pts.push_back(src_pts[idx]); // Fallback
                    out_nms.push_back(src_nms[idx]);
                }
            }
        }
        return std::make_pair(out_pts, out_nms);
    };

    auto dataA = extract_subsurfaces(*this);
    auto dataB = extract_subsurfaces(*t3d);

    int maxSurfaces = std::max((int)dataA.first.size(), (int)dataB.first.size());

    auto resA = resample_surfaces(dataA.first, dataA.second, maxSurfaces);
    auto resB = resample_surfaces(dataB.first, dataB.second, maxSurfaces);

    auto& A_pts_list = resA.first;
    auto& A_nms_list = resA.second;
    auto& B_pts_list = resB.first;
    auto& B_nms_list = resB.second;

    std::vector<glm::vec3> finalA_pts, finalB_pts;
    std::vector<glm::vec3> finalA_nms, finalB_nms;
    int currentOffset = 0;
    int targetN = 0;

    // Determine max points per surface across all surface-pairs
    for (int i = 0; i < maxSurfaces; i++) {
        int n_pair = std::max((int)A_pts_list[i].size(), (int)B_pts_list[i].size());
        if (n_pair > targetN) targetN = n_pair;
    }
    if (targetN == 0) targetN = 1;

    for (int i = 0; i < maxSurfaces; i++)
    {
        auto resA_p = resample_cloud(A_pts_list[i], targetN);
        auto resB_p = resample_cloud(B_pts_list[i], targetN);
        auto resA_n = resample_normals(A_nms_list[i], targetN);
        auto resB_n = resample_normals(B_nms_list[i], targetN);

        finalA_pts.insert(finalA_pts.end(), resA_p.begin(), resA_p.end());
        finalB_pts.insert(finalB_pts.end(), resB_p.begin(), resB_p.end());
        finalA_nms.insert(finalA_nms.end(), resA_n.begin(), resA_n.end());
        finalB_nms.insert(finalB_nms.end(), resB_n.begin(), resB_n.end());
        
        currentOffset += targetN;
    }

    this->points = std::move(finalA_pts);
    this->normals = std::move(finalA_nms);
    this->resolution = {(float)maxSurfaces, (float)targetN};
    this->sub_surface_ranges.clear();
    this->sub_surface_ranges.push_back({0, (int)maxSurfaces * (int)targetN});

    t3d->points = std::move(finalB_pts);
    t3d->normals = std::move(finalB_nms);
    t3d->resolution = {(float)maxSurfaces, (float)targetN};
    t3d->sub_surface_ranges.clear();
    t3d->sub_surface_ranges.push_back({0, (int)maxSurfaces * (int)targetN});

    // Force regeneration of grid-based indices using the new resolution
    this->ThreeDObject::setFillData();
    this->ThreeDObject::setStrokeData();
    t3d->ThreeDObject::setFillData();
    t3d->ThreeDObject::setStrokeData();

    // Reset original points for interpolation
    m_original_points = this->points;
    m_original_normals = this->normals;

    this->uploadStrokeDataToShader();
    this->uploadFillDataToShader();
    t3d->uploadStrokeDataToShader();
    t3d->uploadFillDataToShader();
}

void ThreeDObject::become(GraphMathObject *target)
{
    ThreeDObject *t3d = dynamic_cast<ThreeDObject *>(target);
    if (!t3d)
        return;

    if (!t3d->is_initialized)
        t3d->Init();

    this->points = t3d->points;
    this->normals = t3d->normals;
    this->fill_indices = t3d->fill_indices;
    this->stroke_indices = t3d->stroke_indices;
    this->sub_surface_ranges = t3d->sub_surface_ranges;

    m_original_points.clear(); // Reset stored state
    m_original_normals.clear();

    uploadStrokeDataToShader();
    uploadFillDataToShader();
}