attribute vec4 a_position;
attribute vec2 a_texCoord;
varying float v_distance;
varying vec2 v_texCoord;

void main()
{
    // 座標変換
    vec4 position = CC_MVMatrix * a_position;

    // テクスチャ座標
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;

    // 距離(フォグ計算用)
    v_distance = length(position.xyz);

    gl_Position = CC_PMatrix * position;
}