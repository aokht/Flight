uniform vec4 u_sphereColor;
varying vec3 v_normal;

void main(void)
{
    vec3 eyeDirection = vec3(0.0, 0.0, -1.0);
    vec3 invEye = normalize(CC_NormalMatrix * eyeDirection);
    vec3 lightDirection = vec3(0.0, 10.0, -1.0);
    vec3 invLight = normalize(CC_NormalMatrix * lightDirection);
    vec3 halfVector = normalize(invLight + invEye);

    // diff
    float diff = clamp(dot(invLight, v_normal), 0.5, 1.0);
    // specular
    float spec = pow(clamp(dot(halfVector, v_normal), 0.1, 1.0), 100.0);
    // ambient
    float ambient = 0.25;

    gl_FragColor = vec4(u_sphereColor.rgb * diff + spec + ambient, 0.95);
}