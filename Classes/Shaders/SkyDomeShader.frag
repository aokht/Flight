#ifdef GL_ES
varying highp vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif
uniform vec4 u_color;
varying float v_distance;

void main(void)
{
    vec4 ret = vec4(1.0, 1.0, 1.0, 1.0);

    // テクスチャカラーを取得
    ret = texture2D(CC_Texture0, v_texCoord) * u_color;

    // フォグ
    float fogRate = max(v_distance - 7500.0, 0.0) / 3000.0;
    ret.rgb = ret.rgb * (1.0 - fogRate) + vec3(0.9, 1.0, 1.0) * fogRate;

    gl_FragColor = ret;
}