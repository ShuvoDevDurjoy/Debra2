#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>
#include <glm/glm.hpp>
#include <GraphEngine/Math/Var.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class FunctionGraph : public GraphObject
{
public:
    glm::vec3 (*func)(float, Var);
    Var func_var;
    std::array<float, 3> range;
    float adaptive_tolerance = 0.05f;
    int adaptive_max_depth = 10;
    float bezier_tension = 1.0f / 6.0f;

public:
    FunctionGraph(glm::vec3 (*f)(float, Var), std::array<float, 3> range = {-10.0f * M_PI, 10.0f * M_PI, 0.5f}, Var v = Var(0), bool make_smooth = true)
    {
        this->func = f;
        this->range = range;
        this->func_var = v;
        setStrokeJoinStyle(StrokeJoinStyle::ROUND);
        if (make_smooth)
            makeSmooth();
    }

private:
    struct SamplePoint
    {
        float t;
        glm::vec3 point;
    };

    float chordError(const SamplePoint &left, const SamplePoint &mid, const SamplePoint &right) const
    {
        glm::vec3 chord = right.point - left.point;
        float chord_len_sq = glm::dot(chord, chord);
        if (chord_len_sq <= 1e-8f)
            return glm::distance(mid.point, left.point);

        float u = glm::clamp(glm::dot(mid.point - left.point, chord) / chord_len_sq, 0.0f, 1.0f);
        glm::vec3 projection = left.point + u * chord;
        return glm::distance(mid.point, projection);
    }

    void appendAdaptiveSamples(float t0,
                               const SamplePoint &p0,
                               float t1,
                               const SamplePoint &p1,
                               int depth,
                               std::vector<SamplePoint> &samples)
    {
        float tm = 0.5f * (t0 + t1);
        SamplePoint pm{tm, func(tm, func_var)};

        bool should_split =
            depth < adaptive_max_depth &&
            std::abs(t1 - t0) > 1e-5f &&
            chordError(p0, pm, p1) > adaptive_tolerance;

        if (should_split)
        {
            appendAdaptiveSamples(t0, p0, tm, pm, depth + 1, samples);
            appendAdaptiveSamples(tm, pm, t1, p1, depth + 1, samples);
            return;
        }

        if (samples.empty() || glm::distance(samples.back().point, p0.point) > 1e-7f)
            samples.push_back(p0);
        samples.push_back(p1);
    }

    std::vector<SamplePoint> buildAdaptiveSamples()
    {
        std::vector<SamplePoint> samples;
        if (!func)
            return samples;

        const float t_start = std::min(range[0], range[1]);
        const float t_end = std::max(range[0], range[1]);
        const float base_step = std::max(1e-4f, std::abs(range[2]));
        const int coarse_segments = std::max(1, static_cast<int>(std::ceil((t_end - t_start) / base_step)));

        float seg_start = t_start;
        SamplePoint left{seg_start, func(seg_start, func_var)};

        for (int i = 0; i < coarse_segments; ++i)
        {
            float seg_end = (i == coarse_segments - 1) ? t_end : std::min(t_end, seg_start + base_step);
            SamplePoint right{seg_end, func(seg_end, func_var)};
            appendAdaptiveSamples(seg_start, left, seg_end, right, 0, samples);
            seg_start = seg_end;
            left = right;
        }

        return samples;
    }

    void buildBezierFromAnchors(const std::vector<glm::vec3> &anchors)
    {
        bezier_points.clear();
        bezier_sub_path_starts.clear();
        points.clear();
        point_sub_path_ranges.clear();

        if (anchors.empty())
            return;

        start_bezier_path(anchors.front());
        if (anchors.size() == 1)
            return;

        if (!is_smooth || anchors.size() < 3)
        {
            for (size_t i = 1; i < anchors.size(); ++i)
                add_line_to(anchors[i]);
            return;
        }

        auto tangent_at = [&](size_t idx) -> glm::vec3
        {
            if (idx == 0)
                return anchors[1] - anchors[0];
            if (idx + 1 == anchors.size())
                return anchors[idx] - anchors[idx - 1];
            return anchors[idx + 1] - anchors[idx - 1];
        };

        for (size_t i = 0; i + 1 < anchors.size(); ++i)
        {
            glm::vec3 start = anchors[i];
            glm::vec3 end = anchors[i + 1];
            glm::vec3 out_tangent = tangent_at(i);
            glm::vec3 in_tangent = tangent_at(i + 1);

            glm::vec3 control1 = start + out_tangent * bezier_tension;
            glm::vec3 control2 = end - in_tangent * bezier_tension;
            add_cubic_bezier_curve_to(control1, control2, end);
        }
    }

    void updateBounds(const std::vector<glm::vec3> &anchors)
    {
        if (anchors.empty())
            return;

        float minx = std::numeric_limits<float>::max(), maxX = -std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max(), maxY = -std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max(), maxZ = -std::numeric_limits<float>::max();

        for (const auto &p : anchors)
        {
            minx = std::min(minx, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
            minZ = std::min(minZ, p.z);
            maxZ = std::max(maxZ, p.z);
        }

        setDimension(minx, maxX, minY, maxY, minZ, maxZ);
    }

public:
    void generatePoints() override
    {
        if (!func)
            return;

        std::vector<SamplePoint> samples = buildAdaptiveSamples();
        std::vector<glm::vec3> anchors;
        anchors.reserve(samples.size());
        for (const auto &sample : samples)
            anchors.push_back(sample.point);

        buildBezierFromAnchors(anchors);
        updateBounds(anchors);
    }
};
