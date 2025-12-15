#version 330 core

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

// **********************************************
// **** NEW GLOBAL OUTPUT DECLARATIONS ADDED ****
// **********************************************
out vec3 FragPos;
out vec3 Normal;
flat out int vertexId;
// **********************************************

in VS_OUT {
    vec3 FragPos;   
    vec3 Normal;
    flat int vertexId;
} gs_in[];

const float MAX_Y_JUMP = 30;

flat out int is_discontineous = 0;

void main()
{
    vec4 p0 = gl_in[0].gl_Position; 
    vec4 p1 = gl_in[1].gl_Position; 

    float jump_magnitude = abs(gs_in[1].FragPos.y - gs_in[0].FragPos.y);

    if (gl_in[1].FragPos.y < 5)
    {
        // Emit Vertex 1 (V_i)
        // Now FragPos, Normal, vertexId refer to the global 'out' variables
        FragPos = gs_in[0].FragPos;
        Normal = gs_in[0].Normal;
        vertexId = gs_in[0].vertexId;
        gl_Position = p0;
        EmitVertex();

        // Emit Vertex 2 (V_i+1)
        FragPos = gs_in[1].FragPos;
        Normal = gs_in[1].Normal;
        vertexId = gs_in[1].vertexId;
        gl_Position = p1;
        EmitVertex();

        is_discontineous = 0;

        EndPrimitive();
    }
    else{
        is_discontineous = 1;
    }
}