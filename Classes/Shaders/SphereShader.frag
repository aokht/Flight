uniform vec4 u_sphereColor;
uniform vec3 u_cameraPosition;
uniform vec3 u_cameraEye;
varying float v_visible;
varying vec3 v_normal;
varying vec4 v_position;

void main(void)
{
    if (v_visible == 0.0) {
        discard;
    }

    vec3 eyeDirection = u_cameraEye - u_cameraPosition;
    vec3 invEye = normalize(CC_NormalMatrix * eyeDirection);
    vec3 lightDirection = v_position.xyz + vec3(0.0, 100.0, 0.0);  // 真上から
    vec3 invLight = normalize(CC_NormalMatrix * lightDirection);
    vec3 halfVector = normalize(invLight + invEye);

    // diff
    float diff = clamp(dot(invLight, v_normal), 0.5, 1.0);
    // specular
    float spec = pow(clamp(dot(halfVector, v_normal), 0.1, 1.0), 10.0);
    // ambient
    float ambient = 0.25;

    gl_FragColor = vec4(u_sphereColor.rgb * diff + spec + ambient, 1.0);
}