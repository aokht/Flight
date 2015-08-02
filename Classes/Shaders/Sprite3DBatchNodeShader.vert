#extension GL_EXT_draw_instanced : require

attribute vec4 a_position;
uniform vec3 u_offset[999];

void main(void)
{
    gl_Position = CC_MVPMatrix * (a_position + vec4(u_offset[gl_InstanceIDEXT], 0.0));
}