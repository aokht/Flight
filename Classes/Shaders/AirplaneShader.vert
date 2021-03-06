attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;
varying vec2 v_texCoord;
varying vec3 v_normal;

void main(void)
{
    // 座標変換
    gl_Position = CC_MVPMatrix * a_position;

    // テクスチャ座標
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;

    // ライティング用
    v_normal = a_normal;
}