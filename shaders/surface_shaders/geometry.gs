// #version 330 core

// layout(triangles) in;
// layout(triangle_strip, max_vertices = 3) out;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// uniform float u_progress;
// uniform float triangle_count;

// in VS_OUT {
//     vec3 pos;   // object-space position
// } gs_in[];

// out vec3 Normal;
// out vec3 FragPos;

// out float pProgress;
// out float progress;

// void main()
// {
//     // Object-space positions
//     vec3 p0 = gs_in[0].pos;
//     vec3 p1 = gs_in[1].pos;
//     vec3 p2 = gs_in[2].pos;

//     float triangleID = float(gl_PrimitiveIDIn);
//     float prevProgress = (triangleID * 1.0) / triangle_count;
//     float nextProgress = (triangleID * 1.0 + 1.0) / triangle_count;

//     if(u_progress <= prevProgress) return;

//     float localProgress = clamp((u_progress - prevProgress) / (nextProgress - prevProgress), 0.0, 1.0);

//     // Edge vectors
//     vec3 e0 = p1 - p0;
//     vec3 e1 = p2 - p0;

//     // Face normal (object space)
//     vec3 normal = normalize(cross(e0, e1));

//     // Transform normal to world space (important!)
//     mat3 normalMatrix = transpose(inverse(mat3(model)));
//     vec3 worldNormal = normalize(normalMatrix * normal);

//     // Emit triangle
//     for (int i = 0; i < 3; ++i)
//     {
//         if(i <= 1){
//             pProgress = 0;
//         }
//         else{
//             pProgress = 1;
//         }
//         vec4 worldPos = model * vec4(gs_in[i].pos, 1.0);
//         gl_Position = projection * view * worldPos;

//         Normal = worldNormal;
//         FragPos = worldPos.xyz;

//         EmitVertex();
//     }

//     EndPrimitive();
// }


#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColor;

uniform float u_progress;
uniform int triangle_count;

in VS_OUT {
    vec3 pos;
    vec3 color;
} gs_in[];

out vec3 Normal;
out vec3 FragPos;
// This tells the fragment shader where this triangle sits in the 0.0 - 1.0 timeline
out float v_triangleStart; 
out float v_triangleEnd;
// This identifies which vertex we are at (0.0 to 1.0) to grow 'inside' the triangle
out float v_vertexInterp; 
flat out float triangleID;
out vec3 obj_color;

uniform float stroke_line_width;


uniform int vertices_increment_count;

void main()
{
    int row = gl_PrimitiveIDIn / vertices_increment_count ;
    int row_count = triangle_count / vertices_increment_count;
    float prevProgress = float(row) / float(row_count);
    float nextProgress = float(row + 1) / float(row_count);

    // Calculate Normal
    vec3 e0 = gs_in[1].pos - gs_in[0].pos;
    vec3 e1 = gs_in[2].pos - gs_in[0].pos;
    vec3 normal = normalize(cross(e0, e1));
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 worldNormal = normalize(normalMatrix * normal);

    vec3 dir0 = normalize(gs_in[1].pos - gs_in[0].pos);
    vec3 dir1 = normalize(gs_in[2].pos - gs_in[1].pos);

    float line_width = (stroke_line_width * 0.05);

    vec3 p0 = gs_in[0].pos * 1.0 + dir0 * line_width + dir1 * line_width;
    vec3 p1 = gs_in[1].pos * 1.0 - dir0 * line_width + dir1 * line_width;
    vec3 p2 = gs_in[2].pos * 1.0 - dir1 * line_width - dir0 * line_width;


    vec4 worldPos = model * vec4(p0, 1.0);
    gl_Position = projection * view * worldPos;
    obj_color = gs_in[0].color;
    FragPos = worldPos.xyz;
    Normal = worldNormal;
    v_triangleStart = prevProgress;
    v_triangleEnd = nextProgress;
    v_vertexInterp = 0; 
    EmitVertex();

    worldPos = model * vec4(p1, 1.0);
    gl_Position = projection * view * worldPos;
    obj_color = gs_in[1].color;
    FragPos = worldPos.xyz;
    Normal = worldNormal;
    v_triangleStart = prevProgress;
    v_triangleEnd = nextProgress;
    v_vertexInterp = 0; 
    EmitVertex();

    worldPos = model * vec4(p2, 1.0);
    gl_Position = projection * view * worldPos;
    obj_color = gs_in[2].color;
    FragPos = worldPos.xyz;
    Normal = worldNormal;
    v_triangleStart = prevProgress;
    v_triangleEnd = nextProgress;
    v_vertexInterp = 1; 
    EmitVertex();
    
    EndPrimitive();
}