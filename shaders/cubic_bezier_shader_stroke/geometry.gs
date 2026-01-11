// #version 330 core

// layout(lines) in;
// layout(triangle_strip, max_vertices = 5) out;

// uniform mat4 model, view, projection;

// void main(){
//     mat4 pvm = projection * view * model;
//     gl_Position = pvm * vec4(-20, 0, 0, 1);
//     EmitVertex();
//     gl_Position = pvm * vec4(0, 20, 0, 1);
//     EmitVertex();
//     gl_Position = pvm * vec4(20, 0, 0, 1);
//     EmitVertex();
//     EndPrimitive();
// }

// #version 330 core
// layout(lines) in;
// layout(triangle_strip, max_vertices = 3) out;

// uniform mat4 model, view, projection;

// out vec2 vUV; // Pass this to Fragment Shader

// void main(){
//     mat4 pvm = projection * view * model;

//     // Vertex 1: Start of Curve
//     vUV = vec2(0.0, 0.0);
//     gl_Position = pvm * vec4(-20, 0, 0, 1);
//     EmitVertex();

//     // Vertex 2: Control Point (Apex of the triangle)
//     vUV = vec2(0.5, 0.0);
//     gl_Position = pvm * vec4(0, 20, 0, 1);
//     EmitVertex();

//     // Vertex 3: End of Curve
//     vUV = vec2(1.0, 1.0);
//     gl_Position = pvm * vec4(20, 0, 0, 1);
//     EmitVertex();

//     EndPrimitive();
// }


// #version 330 core
// layout(lines) in; // Triggered once for our hardcoded test
// layout(triangle_strip, max_vertices = 4) out;

// uniform mat4 projection, view, model;

// out vec2 uv_coords;
// out vec2 uv_b2; 
// out float uv_stroke_width_pass;
// out float uv_anti_alias_width_pass;

// out float has_prev, has_next, bevel_start, bevel_end;
// out float angle_from_prev, angle_to_next, bezier_degree;

// void main() {
//     mat4 pvm = projection * view * model;
    
//     // Hardcoded test points
//     vec2 p0 = vec2(-40, -10);
//     vec2 p1 = vec2(-20, 0);   // Start Point
//     vec2 p2 = vec2(0, 20);    // Control Point
//     vec2 p3 = vec2(20, 0);    // End Point
//     vec2 p4 = vec2(40, -10);

//     // 1. Calculate Joint Angles
//     vec2 v01 = normalize(p1 - p0);
//     vec2 v12 = normalize(p2 - p1);
//     vec2 v23 = normalize(p3 - p2);
//     vec2 v34 = normalize(p4 - p3);

//     angle_from_prev = acos(clamp(dot(v01, v12), -1.0, 1.0));
//     angle_to_next = acos(clamp(dot(v23, v34), -1.0, 1.0));

//     // 2. Local Coordinate System (P1 to P3 baseline)
//     vec2 baseline = p3 - p1;
//     float d = length(baseline);
//     vec2 unit_x = baseline / d;
//     vec2 unit_y = vec2(unit_x.y, -unit_x.x);

//     // Map P2 into the UV space
//     vec2 rel_p2 = p2 - p1;
//     uv_b2 = vec2(dot(rel_p2, unit_x) / d, dot(rel_p2, unit_y) / d);
    
//     bezier_degree = 2.0; // Quadratic
//     float stroke_width = 0.2;
//     float aa_width = 1.0;
    
//     uv_stroke_width_pass = stroke_width;
//     uv_anti_alias_width_pass = aa_width;
    
//     has_prev = 1.0; has_next = 1.0; bevel_start = 1.0; bevel_end = 1.0;

//     // 3. Emit Bounding Quad
//     // We expand the box to ensure the "hump" of the Bezier isn't clipped
//     float min_y = min(0.0, uv_b2.y) - 5.0;
//     float max_y = max(0.0, uv_b2.y) + 5.0;

//     // Bottom Left
//     uv_coords = vec2(-0.2, min_y);
//     gl_Position = pvm * vec4(p1 - unit_x * (d*0.2) + unit_y * min_y, 0.0, 1.0);
//     EmitVertex();

//     // Top Left
//     uv_coords = vec2(-0.2, max_y);
//     gl_Position = pvm * vec4(p1 - unit_x * (d*0.2) + unit_y * max_y, 0.0, 1.0);
//     EmitVertex();

//     // Bottom Right
//     uv_coords = vec2(1.2, min_y);
//     gl_Position = pvm * vec4(p1 + unit_x * (d*1.2) + unit_y * min_y, 0.0, 1.0);
//     EmitVertex();

//     // Top Right
//     uv_coords = vec2(1.2, max_y);
//     gl_Position = pvm * vec4(p1 + unit_x * (d*1.2) + unit_y * max_y, 0.0, 1.0);
//     EmitVertex();

//     EndPrimitive();
// }

#version 330 core
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 uv;          // local uv: x ∈ [0,1], y ∈ [-H,+H]
out vec2 b2;          // quadratic control point in uv-space
out float height;     // half-height of quad
out float stroke_w;
out float aa_w;

in VS_OUT {
    vec3 cP;
    vec3 pP;
    vec3 nP;
} gs_in[];

void main()
{
    mat4 pvm = projection * view * model;

    // ----- Corner points -----
    vec2 p0 = gs_in[0].cP.xy;
    vec2 p2 = gs_in[1].cP.xy;
    vec2 p4 = gs_in[1].nP.xy;

    vec2 p1 = (p0 + p2) * 0.5;
    vec2 p3 = (p2 + p4) * 0.5;

    // ----- Local frame (p1 → p3) -----
    vec2 base = p3 - p1;
    float L = length(base);
    vec2 ux = base / L;
    vec2 uy = vec2(-ux.y, ux.x);

    // ----- Control point in local space -----
    vec2 rel = p2 - p1;
    float cx = dot(rel, ux) / L;
    float cy = dot(rel, uy);

    // ----- Quad height (must cover curve fully) -----
    height = abs(cy);
    // height += 0.5; // safety padding

    // ----- UV control point (normalized) -----
    b2 = vec2(cx, cy / height);

    // ----- Stroke params -----
    stroke_w = 0.5;
    aa_w     = 0.01;

    // ----- Emit bounding quad -----
    vec2 A = p1;
    vec2 B = p3;

    // Bottom-left
    uv = vec2(0.0, -height);
    gl_Position = pvm * vec4(A - uy * height, 0.0, 1.0);
    EmitVertex();

    // Top-left
    uv = vec2(0.0, height);
    gl_Position = pvm * vec4(A + uy * height, 0.0, 1.0);
    EmitVertex();

    // Bottom-right
    uv = vec2(1.0, -height);
    gl_Position = pvm * vec4(B - uy * height, 0.0, 1.0);
    EmitVertex();

    // Top-right
    uv = vec2(1.0, height);
    gl_Position = pvm * vec4(B + uy * height, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}


// void main()
// {
//     mat4 pvm = projection * view * model;

//     // ---- Test points (world space) ----
//     vec2 dd0 = gs_in[0].cP.xy;
//     vec2 dd1 = gs_in[1].cP.xy;
//     vec2 dd2 = gs_in[1].nP.xy;


//     vec2 p0 = dd0;
//     vec2 p1 = (dd0 + dd1) / 2;
//     vec2 p2 = dd1;
//     vec2 p3 = (dd1 + dd2) / 2;
//     vec2 p4 = (dd2); 

//     // ---- Signed angle for beveling ----
//     vec2 v01 = normalize(p1 - p0);
//     vec2 v12 = normalize(p2 - p1);

//     float cross = v01.x * v12.y - v01.y * v12.x;
//     float dotv  = dot(v01, v12);
//     angle_from_prev = atan(cross, dotv);
//     bevel_start = 1.0;

//     // ---- Local frame (p1 → p3) ----
//     vec2 baseline = p3 - p1;
//     float d = length(baseline);

//     vec2 ux = baseline / d;
//     vec2 uy = vec2(-ux.y, ux.x);

//     // ---- Map control point into local UV space ----
//     vec2 rel_p2 = p2 - p1;
//     float bx = dot(rel_p2, ux) / d;   // usually ~0.5
//     float by = dot(rel_p2, uy) / d;

//     uv_b2 = vec2(bx, by);

//     // ---- Stroke params ----
//     uv_stroke_width_pass = 0.00015;
//     uv_anti_alias_width_pass = 0.01;

//     // ---- Conservative bounds ----
//     float pad = abs(dot(rel_p2, uy));
//     float min_y = -pad;
//     float max_y =  pad;
//     uv_height = max_y - min_y;

//     uv_min_y = min_y;


//     float npad = dot(rel_p2, uy);

//     // ---- Emit quad ----
//     vec2 left  = p1;
//     vec2 right = p1 + ux * d;

//     // Bottom Left
//     uv_coords = vec2(0.0, min_y);
//     gl_Position = pvm * vec4(left + uy * min_y, 0.0, 1.0);
//     EmitVertex();

//     float elevation = 1;
//     // Top Left
//     uv_coords = vec2(0.0, max_y  );
//     gl_Position = pvm * vec4(left + uy * max_y, 0.0, 1.0);
//     EmitVertex();

//     // Bottom Right
//     uv_coords = vec2(1.0, min_y);
//     gl_Position = pvm * vec4(right + uy * min_y, 0.0, 1.0);
//     EmitVertex();

//     // Top Right
//     uv_coords = vec2(1.0, max_y);
//     gl_Position = pvm * vec4(right + uy * max_y, 0.0, 1.0);
//     EmitVertex();

//     EndPrimitive();
// }

// void main()
// {
//     mat4 pvm = projection * view * model;

//     // ---- Test points (world space) ----
//     vec2 p0 = vec2(-40, -10);
//     vec2 p1 = vec2(-20,   0);
//     vec2 p2 = vec2(  0,  20);
//     vec2 p3 = vec2( 20,   0);
//     vec2 p4 = vec2( 40, -10);

//     // ---- Signed angle for beveling ----
//     vec2 v01 = normalize(p1 - p0);
//     vec2 v12 = normalize(p2 - p1);

//     float cross = v01.x * v12.y - v01.y * v12.x;
//     float dotv  = dot(v01, v12);
//     angle_from_prev = atan(cross, dotv);
//     bevel_start = 1.0;

//     // ---- Local frame (p1 → p3) ----
//     vec2 baseline = p3 - p1;
//     float d = length(baseline);

//     vec2 ux = baseline / d;
//     vec2 uy = vec2(-ux.y, ux.x);

//     // ---- Map control point into local UV space ----
//     vec2 rel_p2 = p2 - p1;
//     float bx = dot(rel_p2, ux) / d;   // usually ~0.5
//     float by = dot(rel_p2, uy) / d;

//     uv_b2 = vec2(bx, by);

//     // ---- Stroke params ----
//     uv_stroke_width_pass = 0.00015;
//     uv_anti_alias_width_pass = 0.01;

//     // ---- Conservative bounds ----
//     float pad = abs(dot(rel_p2, uy));
//     float min_y = -pad;
//     float max_y =  pad;
//     uv_height = max_y - min_y;

//     // ---- Emit quad ----
//     vec2 left  = p1;
//     vec2 right = p1 + ux * d;

//     // Bottom Left
//     uv_coords = vec2(0.0, 0);
//     gl_Position = pvm * vec4(left, 0.0, 1.0);
//     EmitVertex();

//     float elevation = 1;
//     // Top Left
//     uv_coords = vec2(0.0, max_y + elevation );
//     gl_Position = pvm * vec4(left + uy * max_y, 0.0, 1.0);
//     EmitVertex();

//     // Bottom Right
//     uv_coords = vec2(1.0, 0);
//     gl_Position = pvm * vec4(right, 0.0, 1.0);
//     EmitVertex();

//     // Top Right
//     uv_coords = vec2(1.0, max_y + elevation);
//     gl_Position = pvm * vec4(right + uy * max_y, 0.0, 1.0);
//     EmitVertex();

//     EndPrimitive();
// }
