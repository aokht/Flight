#extension GL_EXT_draw_instanced : require

attribute vec4 a_position;
attribute vec3 a_normal;
uniform vec3 u_offset[720];
uniform float u_visible[240]; // TODO: u_offset とまとめるとか
varying float v_visible;
varying vec3 v_normal;
varying vec4 v_position;

void main(void)
{
    v_position = CC_MVPMatrix * (a_position + vec4(u_offset[gl_InstanceIDEXT], 0.0));

    v_visible = u_visible[gl_InstanceIDEXT];

    // ライティング用
    v_normal = a_normal;

    gl_Position = v_position;
}