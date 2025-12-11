$input v_color0

#include <bgfx_shader.sh>

uniform vec4 u_rectColor;

void main() {
    gl_FragData[0] = u_rectColor;
}