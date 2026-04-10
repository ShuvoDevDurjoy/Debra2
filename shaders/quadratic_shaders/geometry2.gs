#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 uViewportSize;
uniform float uAntiAliasWidth = 1.0;
uniform float uProgress = 1.0;
uniform int uSegmentCount = 0;
uniform int u_layer = 0;
uniform int uStrokeJoinStyle = 0;  // 0=auto, 1=round, 2=bevel, 3=miter

in VS_OUT {
    vec3 point;
    vec3 prev_point;
    vec3 next_point;
    vec3 unit_normal;
    float stroke_width;
    vec4 color;
} gs_in[];

out vec4 color;
out float uv_stroke_width;
out float uv_anti_alias_width;
out float has_prev;
out float has_next;
out float bevel_start;
out float bevel_end;
out float angle_from_prev;
out float angle_to_next;
out float bezier_degree;
out vec2 uv_coords;
out vec2 uv_b2;
out float segment_progress_limit;

const float AUTO_JOINT = 0.0;
const float ROUND_JOINT = 1.0;
const float BEVEL_JOINT = 2.0;
const float MITER_JOINT = 3.0;

float cross2d(vec2 v, vec2 w)
{
    return v.x * w.y - w.x * v.y;
}

vec3 get_view_direction()
{
    // camera forward direction in world space
    return normalize(-vec3(view[2]));
}

mat3 get_xy_to_uv(vec2 b0, vec2 b1)
{
    mat3 shift = mat3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        -b0.x, -b0.y, 1.0
    );

    float sf = length(b1 - b0);
    vec2 I = (b1 - b0) / sf;
    vec2 J = vec2(-I.y, I.x);
    mat3 rotate = mat3(
        I.x, J.x, 0.0,
        I.y, J.y, 0.0,
        0.0, 0.0, 1.0
    );
    return (1.0 / sf) * rotate * shift;
}

float get_reduced_control_points(in vec2 points[3], out vec2 new_points[3])
{
    const float length_threshold = 1e-6;
    const float angle_threshold = 5e-2;

    vec2 p0 = points[0];
    vec2 p1 = points[1];
    vec2 p2 = points[2];
    vec2 v01 = p1 - p0;
    vec2 v12 = p2 - p1;

    float len01 = length(v01);
    float len12 = length(v12);
    bool distinct_01 = len01 > length_threshold;
    bool distinct_12 = len12 > length_threshold;
    int n_uniques = int(distinct_01) + int(distinct_12);

    bool aligned = false;
    if (distinct_01 && distinct_12)
    {
        float dot_prod = clamp(dot(normalize(v01), normalize(v12)), -1.0, 1.0);
        aligned = acos(dot_prod) < angle_threshold;
    }

    bool quadratic = (n_uniques == 2) && !aligned;
    bool linear = (n_uniques == 1) || ((n_uniques == 2) && aligned);

    if (quadratic)
    {
        new_points[0] = p0;
        new_points[1] = p1;
        new_points[2] = p2;
        return 2.0;
    }
    if (linear)
    {
        new_points[0] = p0;
        new_points[1] = 0.5 * (p0 + p2);
        new_points[2] = p2;
        return 1.0;
    }

    new_points[0] = p0;
    new_points[1] = p0;
    new_points[2] = p0;
    return 0.0;
}

float angle_between_vectors(vec2 v1, vec2 v2)
{
    float v1_norm = length(v1);
    float v2_norm = length(v2);
    if (v1_norm <= 1e-6 || v2_norm <= 1e-6)
        return 0.0;

    float dp = dot(v1, v2) / (v1_norm * v2_norm);
    float angle = acos(clamp(dp, -1.0, 1.0));
    float sn = sign(cross2d(v1, v2));
    return sn * angle;
}

bool find_intersection(vec2 p0, vec2 v0, vec2 p1, vec2 v1, out vec2 intersection)
{
    float det = -v0.x * v1.y + v1.x * v0.y;
    if (abs(det) < 1e-6)
        return false;

    float t = cross2d(p0 - p1, v1) / det;
    intersection = p0 + v0 * t;
    return true;
}

void create_joint(
    float angle,
    vec2 unit_tan,
    float buff,
    vec2 static_c0,
    out vec2 changing_c0,
    vec2 static_c1,
    out vec2 changing_c1
) {
    float shift;
    if (abs(angle) < 1e-3)
    {
        shift = 0.0;
    }
    else if (float(uStrokeJoinStyle) == MITER_JOINT)
    {
        shift = buff * (-1.0 - cos(angle)) / sin(angle);
    }
    else
    {
        shift = buff * (1.0 - cos(angle)) / sin(angle);
    }

    changing_c0 = static_c0 - shift * unit_tan;
    changing_c1 = static_c1 + shift * unit_tan;
}

int get_corners(vec2 controls[3], int degree, float stroke_widths[3], out vec2 corners[5])
{
    vec2 p0 = controls[0];
    vec2 p1 = controls[1];
    vec2 p2 = controls[2];

    vec2 v10 = normalize(p0 - p1);
    vec2 v12 = normalize(p2 - p1);
    vec2 v01 = -v10;
    vec2 v21 = -v12;

    vec2 p0_perp = vec2(-v01.y, v01.x);
    vec2 p2_perp = vec2(-v12.y, v12.x);

    float aaw = uAntiAliasWidth;
    float buff0 = 0.5 * stroke_widths[0] + aaw;
    float buff2 = 0.5 * stroke_widths[2] + aaw;
    float aaw0 = (1.0 - has_prev) * aaw;
    float aaw2 = (1.0 - has_next) * aaw;

    vec2 c0 = p0 - buff0 * p0_perp + aaw0 * v10;
    vec2 c1 = p0 + buff0 * p0_perp + aaw0 * v10;
    vec2 c2 = p2 + buff2 * p2_perp + aaw2 * v12;
    vec2 c3 = p2 - buff2 * p2_perp + aaw2 * v12;

    if (has_prev > 0.5)
        create_joint(angle_from_prev, v01, buff0, c0, c0, c1, c1);
    if (has_next > 0.5)
        create_joint(angle_to_next, v21, buff2, c3, c3, c2, c2);

    if (degree == 1)
    {
        corners[0] = c0;
        corners[1] = c1;
        corners[2] = c3;
        corners[3] = c2;
        corners[4] = vec2(0.0);
        return 4;
    }

    float orientation = sign(cross2d(v01, v12));
    if (orientation > 0.0)
    {
        corners[0] = c0;
        corners[1] = c1;
        corners[2] = p1;
        corners[3] = c2;
        corners[4] = c3;
    }
    else
    {
        corners[0] = c1;
        corners[1] = c0;
        corners[2] = p1;
        corners[3] = c3;
        corners[4] = c2;
    }

    vec2 intersection;
    if (find_intersection(corners[0], v01, corners[4], v21, intersection))
        corners[2] = intersection;

    return 5;
}

void set_adjacent_info(
    vec2 c0,
    vec2 tangent,
    int degree,
    vec2 adj[3],
    out float bevel,
    out float angle
) {
    bool linear_adj = abs(angle_between_vectors(adj[1] - adj[0], adj[2] - adj[1])) < 1e-3;
    angle = angle_between_vectors(c0 - adj[1], tangent);

    bool one_linear = (degree == 1) || linear_adj;
    bool should_bevel = (
        (float(uStrokeJoinStyle) == AUTO_JOINT && one_linear) ||
        float(uStrokeJoinStyle) == BEVEL_JOINT
    );
    bevel = should_bevel ? 1.0 : 0.0;
}

void find_joint_info(vec2 controls[3], vec2 prev[3], vec2 next[3], int degree)
{
    float tol = 1e-4;
    has_prev = float(distance(prev[2], controls[0]) < tol);
    has_next = float(distance(next[0], controls[2]) < tol);
    bevel_start = 0.0;
    bevel_end = 0.0;
    angle_from_prev = 0.0;
    angle_to_next = 0.0;

    if (has_prev > 0.5)
    {
        vec2 tangent = controls[1] - controls[0];
        set_adjacent_info(controls[0], tangent, degree, prev, bevel_start, angle_from_prev);
    }
    if (has_next > 0.5)
    {
        vec2 tangent = controls[1] - controls[2];
        set_adjacent_info(controls[2], tangent, degree, next, bevel_end, angle_to_next);
        angle_to_next *= -1.0;
    }
}

vec2 clip_to_screen(vec4 clip_pos)
{
    vec2 ndc = clip_pos.xy / clip_pos.w;
    return (ndc * 0.5 + 0.5) * uViewportSize;
}

vec4 screen_to_clip(vec2 screen_pos, float clip_z, float clip_w)
{
    vec2 ndc = (screen_pos / uViewportSize) * 2.0 - 1.0;
    return vec4(ndc * clip_w, clip_z, clip_w);
}

void main()
{
    if (uSegmentCount <= 0 || uProgress <= 0.0)
        return;

    float segment_start = float(gl_PrimitiveIDIn) / float(uSegmentCount);
    float segment_end = float(gl_PrimitiveIDIn + 1) / float(uSegmentCount);
    if (uProgress <= segment_start)
        return;

    segment_progress_limit = 1.0;
    if (uProgress < segment_end)
    {
        segment_progress_limit = clamp(
            (uProgress - segment_start) / max(segment_end - segment_start, 1e-6),
            0.0,
            1.0
        );
    }

    vec4 clip_controls_src[3];
    vec4 clip_prev_src[3];
    vec4 clip_next_src[3];
    vec2 current_points[3];
    vec2 prev_points[3];
    vec2 next_points[3];
    float stroke_widths[3];

    mat4 pvm = projection * view * model;

    vec3 normal_world = normalize(mat3(model) * gs_in[1].unit_normal);
    vec3 view_dir = get_view_direction();
    float facing = abs(dot(normal_world, view_dir));

    for (int i = 0; i < 3; ++i)
    {
        clip_controls_src[i] = pvm * vec4(gs_in[i].point, 1.0);
        clip_prev_src[i] = pvm * vec4(gs_in[i].prev_point, 1.0);
        clip_next_src[i] = pvm * vec4(gs_in[i].next_point, 1.0);

        current_points[i] = clip_to_screen(clip_controls_src[i]);
        prev_points[i] = clip_to_screen(clip_prev_src[i]);
        next_points[i] = clip_to_screen(clip_next_src[i]);
        stroke_widths[i] = gs_in[i].stroke_width * facing;
    }

    vec2 controls[3];
    vec2 prev[3];
    vec2 next[3];
    bezier_degree = get_reduced_control_points(current_points, controls);
    if (bezier_degree == 0.0)
        return;

    get_reduced_control_points(prev_points, prev);
    get_reduced_control_points(next_points, next);

    int degree = int(bezier_degree);
    find_joint_info(controls, prev, next, degree);

    vec2 corners[5];
    int n_corners = get_corners(controls, degree, stroke_widths, corners);

    int index_map[5] = int[5](0, 0, 1, 2, 2);
    if (n_corners == 4)
        index_map[2] = 2;

    float scale_factor = length(controls[1] - controls[0]);
    if (scale_factor <= 1e-6)
        return;

    mat3 xy_to_uv = get_xy_to_uv(controls[0], controls[1]);
    uv_anti_alias_width = uAntiAliasWidth / scale_factor;
    uv_b2 = (xy_to_uv * vec3(controls[2], 1.0)).xy;

    float z_bias = float(u_layer) * 1e-6 + 1e-6;

    for (int i = 0; i < n_corners; ++i)
    {
        uv_coords = (xy_to_uv * vec3(corners[i], 1.0)).xy;
        uv_stroke_width = stroke_widths[index_map[i]] / scale_factor;
        color = gs_in[index_map[i]].color;

        vec4 clip_ref = clip_controls_src[index_map[i]];
        gl_Position = screen_to_clip(corners[i], clip_ref.z - z_bias * clip_ref.w, clip_ref.w);
        EmitVertex();
    }

    EndPrimitive();
}
