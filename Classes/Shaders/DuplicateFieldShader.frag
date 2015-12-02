#ifdef GL_ES
varying highp vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif
uniform vec4 u_color;
uniform sampler2D u_normalMap;
uniform vec3 u_lightDirection;

varying float v_distance;

void main(void)
{
    vec4 ret = vec4(1.0, 1.0, 1.0, 1.0);

    // テクスチャカラーを取得
    ret = texture2D(CC_Texture0, v_texCoord) * u_color;

    // 法線マップを適用
    vec4 normalMap = texture2D(u_normalMap, v_texCoord);
    normalMap.xyz = (normalMap.xyz - 0.5) * 2.0;
    ret.rgb = ret.rgb * clamp(dot(normalize(normalMap.xyz), normalize(u_lightDirection)), 0.4, 1.0);  // TODO: (仮)

    // フォグ
    float fogRate = max(v_distance - 7500.0, 0.0) / 3000.0;
    ret.rgb = ret.rgb * (1.0 - fogRate) + vec3(0.9, 1.0, 1.0) * fogRate;

    gl_FragColor = ret;
}