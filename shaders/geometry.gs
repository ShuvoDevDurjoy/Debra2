f// layout(lines) in;
// layout(triangle_strip, max_vertices=4) out;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// in VS_OUT{
//     vec3 point;
// }gs_in[];

// void main() {

//     mat4 pvm = projection * view * model;
//     vec4 P = gl_in[0].gl_Position;

//     vec3 point0 = gs_in[0];
//     vec3 point1 = gs_in[1];
    

//     gl_Position = P + vec4(-pointSize, -pointSize, 0, 0);
//     EmitVertex();

//     gl_Position = P + vec4( pointSize, -pointSize, 0, 0);
//     EmitVertex();

//     gl_Position = P + vec4(-pointSize,  pointSize, 0, 0);
//     EmitVertex();

//     gl_Position = P + vec4( pointSize,  pointSize, 0, 0);
//     EmitVertex();

//     EndPrimitive();
// }



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


// #version 330 core

// layout(triangles) in;
// layout(triangle_strip, max_vertices = 7) out;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
// uniform vec3 objectColor;

// uniform float u_progress;
// uniform int triangle_count;

// in VS_OUT {
//     vec3 pos;
//     vec3 color;
// } gs_in[];

// out vec3 Normal;
// out vec3 FragPos;
// // This tells the fragment shader where this triangle sits in the 0.0 - 1.0 timeline
// out float v_triangleStart; 
// out float v_triangleEnd;
// // This identifies which vertex we are at (0.0 to 1.0) to grow 'inside' the triangle
// out float v_vertexInterp; 
// flat out float triangleID;
// out vec3 obj_color;
// flat out vec3 controlCorner;


// flat out vec3 corner1, corner2, corner3;

// uniform int vertices_increment_count;
// flat out vec3 c;

// uniform float stroke_line_width;

// void main()
// {
//     int row = gl_PrimitiveIDIn / vertices_increment_count ;
//     int row_count = triangle_count / vertices_increment_count;
//     float prevProgress = float(row) / float(row_count);
//     float nextProgress = float(row + 1) / float(row_count);

//     // Calculate Normal
//     vec3 e0 = gs_in[1].pos - gs_in[0].pos;
//     vec3 e1 = gs_in[2].pos - gs_in[0].pos;
//     vec3 normal = normalize(cross(e0, e1));
//     mat3 normalMatrix = transpose(inverse(mat3(model)));
//     vec3 worldNormal = normalize(normalMatrix * normal);

//     corner1 = (model * vec4(gs_in[0].pos, 1.0)).xyz;
//     corner2 = (model * vec4(gs_in[1].pos, 1.0)).xyz;
//     corner3 = (model * vec4(gs_in[2].pos, 1.0)).xyz;

//     vec3 dir0 = normalize(gs_in[0].pos - gs_in[1].pos);
//     vec3 dir1 = normalize(gs_in[2].pos - gs_in[1].pos);


//     mat4 pv = projection * view * model;

//     vec4 p0 = vec4(gs_in[0].pos + dir1 * stroke_line_width * 0.05, 1.0);
//     vec4 p1 = vec4(gs_in[0].pos, 1.0);
//     vec4 p2 = vec4(gs_in[1].pos + dir0 * stroke_line_width * 0.05 + dir1 * stroke_line_width * 0.05, 1.0);
//     vec4 p3 = vec4(gs_in[1].pos, 1.0);
//     vec4 p4 = vec4(gs_in[2].pos + dir0 * stroke_line_width * 0.05, 1.0);
//     vec4 p5 = vec4(gs_in[2].pos, 1.0);

//     obj_color = gs_in[0].color;
//     c = p1.xyz;
//     FragPos = p0.xyz;
//     gl_Position = pv * p0;
//     EmitVertex();

//     obj_color = gs_in[0].color;
//     c = p2.xyz;
//     FragPos = p1.xyz;
//     gl_Position = pv * p1;
//     EmitVertex();

//     obj_color = gs_in[1].color;
//     c = p2.xyz;
//     FragPos = p2.xyz;
//     gl_Position = pv * p2;
//     EmitVertex();

//     obj_color = gs_in[1].color;
//     c = p1.xyz;
//     FragPos = p3.xyz;
//     gl_Position = pv * p3;
//     EmitVertex();

//     obj_color = gs_in[1].color;
//     c = p2.xyz;
//     FragPos = p2.xyz;
//     gl_Position = pv * p2;
//     EmitVertex();

//     obj_color = gs_in[2].color;
//     c = p5.xyz;
//     FragPos = p4.xyz;
//     gl_Position = pv * p4;
//     EmitVertex();

//     obj_color = gs_in[2].color;
//     c = p2.xyz;
//     FragPos = p5.xyz;
//     gl_Position = pv * p5;
//     EmitVertex();
//     EndPrimitive();
// }


#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 7) out;

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
out vec3 FragPos; // Now World Space
out vec3 obj_color;
flat out vec3 corner1, corner2, corner3;

uniform int vertices_increment_count;
uniform int u_layer;

void main()
{
    // Calculate Normal in World Space
    vec3 e0 = gs_in[1].pos - gs_in[0].pos;
    vec3 e1 = gs_in[2].pos - gs_in[0].pos;
    vec3 normal = normalize(cross(e0, e1));
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * normal);

    // World Space Corners for SDF math
    corner1 = (model * vec4(gs_in[0].pos, 1.0)).xyz;
    corner2 = (model * vec4(gs_in[1].pos, 1.0)).xyz;
    corner3 = (model * vec4(gs_in[2].pos, 1.0)).xyz;

    // Calculate object-space perpendiculars for flat strokes
    vec3 v0 = gs_in[0].pos;
    vec3 v1 = gs_in[1].pos;
    vec3 v2 = gs_in[2].pos;

    vec3 edge01 = normalize(v1 - v0);
    vec3 edge12 = normalize(v2 - v1);
    vec3 edge20 = normalize(v0 - v2);

    // Assuming the shape is in the XY plane by default (normal is Z)
    // For a flat shape, we want the offset to be in the plane.
    // We can use the cross product of the edge and the face normal.
    vec3 faceNormal = normalize(cross(v1 - v0, v2 - v0));
    vec3 perp01 = normalize(cross(edge01, faceNormal));
    vec3 perp12 = normalize(cross(edge12, faceNormal));
    vec3 perp20 = normalize(cross(edge20, faceNormal));

    float w = stroke_line_width * 0.5;

    float zBias = float(u_layer) * 0.001 + 0.0001; 

    // Edge 0-1
    vec3 p[4];
    p[0] = v0 + perp01 * w;
    p[1] = v0;
    p[2] = v1 + perp01 * w;
    p[3] = v1;
    
    for(int i = 0; i < 4; i++) {
        obj_color = gs_in[i/2].color;
        FragPos = (model * vec4(p[i], 1.0)).xyz;
        gl_Position = projection * view * vec4(FragPos, 1.0);
        gl_Position.z -= zBias * gl_Position.w;
        EmitVertex();
    }
    EndPrimitive();

    // Edge 1-2
    p[0] = v1 + perp12 * w;
    p[1] = v1;
    p[2] = v2 + perp12 * w;
    p[3] = v2;
    for(int i = 0; i < 4; i++) {
        obj_color = gs_in[i/2 + 1 < 3 ? i/2+1 : 2].color;
        FragPos = (model * vec4(p[i], 1.0)).xyz;
        gl_Position = projection * view * vec4(FragPos, 1.0);
        gl_Position.z -= zBias * gl_Position.w;
        EmitVertex();
    }
    EndPrimitive();

    // Edge 2-0
    p[0] = v2 + perp20 * w;
    p[1] = v2;
    p[2] = v0 + perp20 * w;
    p[3] = v0;
    for(int i = 0; i < 4; i++) {
        obj_color = gs_in[i/2 == 0 ? 2 : 0].color;
        FragPos = (model * vec4(p[i], 1.0)).xyz;
        gl_Position = projection * view * vec4(FragPos, 1.0);
        gl_Position.z -= zBias * gl_Position.w;
        EmitVertex();
    }
    EndPrimitive();
}