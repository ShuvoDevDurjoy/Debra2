#version 330 core

// Stencil pass 1: writes only to stencil buffer, no color output.
// Increments stencil value for CW triangles, decrements for CCW
// (even-odd or non-zero winding rule handled by OpenGL stencil ops).
void main() {
    // No color output needed for stencil pass
}
