#version 330 core

in vec2 uv_coords;
in vec2 uv_b2;
in float uv_stroke_width;
in vec4 color;
in float uv_anti_alias_width;
in float has_prev;
in float has_next;
in float bevel_start;
in float bevel_end;
in float angle_from_prev;
in float angle_to_next;
in float bezier_degree;
in float segment_progress_limit;

out vec4 FragColor;

float cross2d(vec2 v, vec2 w)
{
    return v.x * w.y - w.x * v.y;
}

float modify_distance_for_endpoints(vec2 p, float dist, float t)
{
    float buff = 0.5 * uv_stroke_width - uv_anti_alias_width;

    if (t == 0.0)
    {
        if (has_prev == 0.0)
            return max(dist, -p.x + buff);

        if (bevel_start == 1.0)
        {
            float a = angle_from_prev;
            mat2 rot = mat2(
                cos(a), sin(a),
                -sin(a), cos(a)
            );
            float bevel_d = max(abs(p.y), abs((rot * p).y));
            return max(min(dist, bevel_d), dist / 2.0);
        }
    }
    else if (t == 1.0)
    {
        vec2 v21 = (bezier_degree == 2.0) ? vec2(1.0, 0.0) - uv_b2 : vec2(-1.0, 0.0);
        float len_v21 = length(v21);
        if (len_v21 <= 1e-6)
        {
            v21 = -uv_b2;
            len_v21 = max(length(v21), 1e-6);
        }

        float perp_dist = dot(p - uv_b2, v21) / len_v21;
        if (has_next == 0.0)
            return max(dist, -perp_dist + buff);

        if (bevel_end == 1.0)
        {
            float a = -angle_to_next;
            mat2 rot = mat2(
                cos(a), sin(a),
                -sin(a), cos(a)
            );
            vec2 v21_unit = v21 / len_v21;
            float bevel_d = max(
                abs(cross2d(p - uv_b2, v21_unit)),
                abs(cross2d((rot * (p - uv_b2)), v21_unit))
            );
            return max(min(dist, bevel_d), dist / 2.0);
        }
    }

    return dist;
}

vec2 bezier(float t, vec2 b2)
{
    if (t == 0.0)
        return vec2(0.0, 0.0);
    if (t == 1.0)
        return b2;

    return vec2(
        2.0 * t * (1.0 - t) + b2.x * t * t,
        b2.y * t * t
    );
}

float cube_root(float x)
{
    return sign(x) * pow(abs(x), 1.0 / 3.0);
}

int cubic_solve(float a, float b, float c, float d, out float roots[3])
{
    b = b / a;
    c = c / a;
    d = d / a;

    float p = c - b * b / 3.0;
    float q = b * (2.0 * b * b - 9.0 * c) / 27.0 + d;
    float p3 = p * p * p;
    float disc = q * q + 4.0 * p3 / 27.0;
    float offset = -b / 3.0;

    if (disc >= 0.0)
    {
        float z = sqrt(disc);
        float u = cube_root((-q + z) / 2.0);
        float v = cube_root((-q - z) / 2.0);
        roots[0] = offset + u + v;
        return 1;
    }

    float u = sqrt(-p / 3.0);
    float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
    float m = cos(v);
    float n = sin(v) * 1.732050808;

    float all_roots[3] = float[3](
        offset + u * (n - m),
        offset - u * (n + m),
        offset + u * (m + m)
    );

    int n_valid_roots = 0;
    for (int i = 0; i < 3; ++i)
    {
        float r = all_roots[i];
        if (3.0 * r * r + 2.0 * b * r + c > 0.0)
        {
            roots[n_valid_roots] = r;
            n_valid_roots++;
        }
    }
    return n_valid_roots;
}

float dist_to_line(vec2 p, vec2 b2)
{
    float denom = max(b2.x, 1e-6);
    float t = clamp(p.x / denom, 0.0, 1.0);
    float dist;
    if (t == 0.0)
        dist = length(p);
    else if (t == 1.0)
        dist = distance(p, b2);
    else
        dist = abs(p.y);

    return modify_distance_for_endpoints(p, dist, t);
}

float dist_to_point_on_curve(vec2 p, float t, vec2 b2)
{
    t = clamp(t, 0.0, 1.0);
    return modify_distance_for_endpoints(p, length(p - bezier(t, b2)), t);
}

float min_dist_to_curve(vec2 p, vec2 b2, float degree)
{
    if (degree == 1.0)
        return dist_to_line(p, b2);

    float xm2 = uv_b2.x - 2.0;
    float y = uv_b2.y;
    float a = xm2 * xm2 + y * y;
    float b = 3.0 * xm2;
    float c = -(p.x * xm2 + p.y * y) + 2.0;
    float d = -p.x;

    float roots[3];
    int n = cubic_solve(a, b, c, d, roots);
    float d0 = dist_to_point_on_curve(p, roots[0], b2);
    if (n <= 1)
        return d0;

    float d1 = dist_to_point_on_curve(p, roots[1], b2);
    return min(d0, d1);
}

void main()
{
    if (uv_stroke_width <= 0.0)
        discard;

    if (segment_progress_limit <= 0.0)
        discard;

    float progress_alpha = 1.0;
    if (segment_progress_limit < 1.0)
    {
        progress_alpha = smoothstep(
            segment_progress_limit + uv_anti_alias_width,
            segment_progress_limit - uv_anti_alias_width,
            uv_coords.x
        );
        if (progress_alpha <= 0.0)
            discard;
    }

    float dist_to_curve = min_dist_to_curve(uv_coords, uv_b2, bezier_degree);
    float signed_dist = abs(dist_to_curve) - 0.5 * uv_stroke_width;

    FragColor = color;
    FragColor.a *= progress_alpha * smoothstep(0.5, -0.5, signed_dist / max(uv_anti_alias_width, 1e-6));

    if (FragColor.a <= 0.0)
        discard;
}
