attribute vec4 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;

void main(void)
{
    gl_Position = CC_MVPMatrix * a_position;
    v_normal = a_normal;
}