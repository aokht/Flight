#ifdef GL_ES
varying highp vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif
uniform vec4 u_color;
uniform sampler2D u_normalMap;
varying vec4 v_color;

void main(void)
{
    vec4 ret = vec4(1.0, 1.0, 1.0, 1.0);

    // テクスチャカラーを取得
    ret = texture2D(CC_Texture0, v_texCoord) * u_color;

    // 法線マップを適用
    vec4 normalMap = texture2D(u_normalMap, v_texCoord);
    normalMap.xyz = normalMap.xyz - 0.5;
    ret.xyz = ret.xyz * max(0.1, dot(normalMap.rgb, vec3(1.0, 1.0, 1.0))) * 2.0 + 0.1;  // TODO: (仮)

    // 頂点シェーダによる描画禁止頂点を含むポリゴン全体を破棄する
    if (v_color.a < 0.999) {
        discard;
    }

    gl_FragColor = ret;
}