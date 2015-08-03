#extension GL_EXT_draw_instanced : require

attribute vec4 a_position;
uniform vec3 u_offset[600];
uniform float u_visible[200]; // TODO: u_offset とまとめるとか
varying float v_visible;

void main(void)
{
    gl_Position = CC_MVPMatrix * (a_position + vec4(u_offset[gl_InstanceIDEXT], 0.0));

    v_visible = u_visible[gl_InstanceIDEXT];
}