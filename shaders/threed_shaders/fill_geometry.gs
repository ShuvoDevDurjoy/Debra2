#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 FragPos[];
in vec2 v_uv[];
in vec3 vNormal[];

out vec3 gFragPos;
out vec2 g_uv;
out vec3 gNormal;

void main() {
    vec3 edge1 = FragPos[1] - FragPos[0];
    vec3 edge2 = FragPos[2] - FragPos[0];
    vec3 flat_normal = vec3(0.0, 0.0, 1.0); // Default fallback
    
    if (length(cross(edge1, edge2)) > 0.0001) {
        flat_normal = normalize(cross(edge1, edge2));
    }
    
    for(int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        gFragPos = FragPos[i];
        g_uv = v_uv[i];
        
        // Use smooth vertex normal if available (calculated on CPU or VS), 
        // else fallback to flat triangle normal.
        if(length(vNormal[i]) > 0.001) {
            gNormal = vNormal[i]; // It was already transformed in VS
        } else {
            gNormal = flat_normal; 
        }
        
        EmitVertex();
    }
    EndPrimitive();
}
