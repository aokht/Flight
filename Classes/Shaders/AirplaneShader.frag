uniform vec4 u_color;
uniform vec3 u_lightDirection;
varying vec2 v_texCoord;
varying vec3 v_normal;

void main(void)
{
    vec4 ret = vec4(1.0, 1.0, 1.0, 1.0);

    // テクスチャカラーを取得
    ret = texture2D(CC_Texture0, v_texCoord) * u_color;

    // ライティング
    vec3 lightDirection = normalize(u_lightDirection);
    vec3 eyeDirection = vec3(0.0, 0.0, 1.0);  // 正面から
    vec3 normal = normalize(CC_NormalMatrix * v_normal);
    vec3 halfVector = normalize(lightDirection + eyeDirection);

    // diff
    float diff = clamp(dot(lightDirection, normal), 0.5, 1.0);
    // specular
    float spec = pow(clamp(dot(halfVector, normal), 0.0, 1.0), 40.0) * 0.5;
    // ambient
    float ambient = 0.0;

    ret.rgb = ret.rgb * diff + spec + ambient;

    gl_FragColor = ret;
}