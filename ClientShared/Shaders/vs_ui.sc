$input a_position, a_color0
$output v_color0

#include <bgfx_shader.sh>

uniform mat4 u_rect;

void main() {
    vec3 worldPos = mul(u_rect, vec4(a_position, 1.0)).xyz;
    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));
    v_color0 = a_color0;
}