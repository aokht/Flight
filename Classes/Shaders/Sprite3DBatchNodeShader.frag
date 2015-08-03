varying float v_visible;

void main(void)
{
    if (v_visible == 0.0) {
        discard;
    }

    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}