attribute vec4 a_position;
attribute vec2 a_texCoord;
uniform vec3 u_currentRotation;
uniform vec3 u_currentPosition;

varying vec2 v_texCoord;
varying vec4 v_color;

#define FIELD_LENGTH 10000.0      // フィールドの1辺の長さ
#define ANGLE_INSIGHT 90.0        // 視界の広さ(方向ベクトルの両側に広がる)
#define DISTANCE_THRESHOLD 300.0  // 描画しないエリア閾値
#define POSITION_OFFSET 1000.0    // 視界の始点をどれだけ手前に引くか

bool isInsightXZ(out float s, out float t, const in vec3 a, const in vec3 b, const in vec3 p)
{
    // s*a + t*b からなる平行四辺形内に p が存在するかを調べる(クラメルの公式による)
    // detA == 0.0 は a, b が同一直線の場合のみなのでありえない
    float detA = a.x * b.z - a.z * b.x;
    float rcpDetA = 1.0 / detA;

    float detA1 = p.x * b.z - p.z * b.x;
    float detA2 = a.x * p.z - a.z * p.x;

    s = detA1 * rcpDetA;
    t = detA2 * rcpDetA;

    return 0.0 <= s && s <= FIELD_LENGTH && 0.0 <= t && t <= FIELD_LENGTH;
}

void checkMergin(const in float s, const in float t, const in vec3 a, const in vec3 b, const in vec3 p)
{
    // 頂点が視界の端付近の場合、頂点移動時にポリゴンが視界の前後にまたがり画面が乱れるため
    // 透明度を 0 にして描画しないようにする
    if (
        abs(s) < DISTANCE_THRESHOLD ||
        abs(t) < DISTANCE_THRESHOLD ||
        FIELD_LENGTH - DISTANCE_THRESHOLD < abs(s) ||
        FIELD_LENGTH - DISTANCE_THRESHOLD < abs(t)
    ) {
        v_color.a = 0.0;
    }
}

vec3 moveToInsightXZ(in vec3 position)
{
    // 進行方向
    float theta = radians(u_currentRotation.y);
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // 進行方向ベクトル
    vec3 direction = vec3(sinTheta, 0.0, cosTheta);

    // 視界の広さ(進行方向ベクトルの両側)
    float angle = ANGLE_INSIGHT;
    float phi = radians(angle / 2.0);
    float cosPhi = cos(phi);
    float sinPhi = sin(phi);

    // 視界単位ベクトル
    vec3 sightA = vec3(sinTheta * cosPhi + cosTheta * sinPhi, 0.0, cosTheta * cosPhi - sinTheta * sinPhi);
    vec3 sightB = vec3(sinTheta * cosPhi - cosTheta * sinPhi, 0.0, cosTheta * cosPhi + sinTheta * sinPhi);

    // 目標ベクトル
    vec3 target = position - (u_currentPosition - POSITION_OFFSET * direction);

    // 視界単位ベクトルによる位置表現係数
    float s, t;

    // その場から見えればそのまま返す
    bool isInsight = isInsightXZ(s, t, sightA, sightB, target);
    if (! isInsight) {
        // 視界の端の場合は描画しない
        checkMergin(s, t, sightA, sightB, target);
    }

    // 頂点を X方向に移動してみる
    float xMove = FIELD_LENGTH * -sign(target.x - u_currentPosition.x);
    vec3 targetXMoved = target + vec3(xMove, 0.0, 0.0);
    if (!isInsight && isInsightXZ(s, t, sightA, sightB, targetXMoved)) {
        // 見えるようになるなら移動する
        isInsight = true;
        position.x += xMove;
        // 視界の端なら描画しない
        checkMergin(s, t, sightA, sightB, targetXMoved);
    }

    // 頂点を Z方向に移動してみる
    float zMove = FIELD_LENGTH * -sign(target.z - u_currentPosition.z);
    vec3 targetZMoved = target + vec3(0.0, 0.0, zMove);
    if (!isInsight && isInsightXZ(s, t, sightA, sightB, targetZMoved)) {
        // 見えるようになるなら移動する
        isInsight = true;
        position.z += zMove;
        // 視界の端なら描画しない
        checkMergin(s, t, sightA, sightB, targetZMoved);
    }

    // 頂点を XZ方向に移動してみる
    vec3 targetXZMoved = target + vec3(xMove, 0.0, zMove);
    if (!isInsight && isInsightXZ(s, t, sightA, sightB, targetXZMoved)) {
        // 見えるようになるなら移動する
        isInsight = true;
        position.x += xMove;
        position.z += zMove;
        // 視界の端なら描画しない
        checkMergin(s, t, sightA, sightB, targetXZMoved);
    }

    return position;
}

void main()
{
    // 視界に入っていない頂点を、視界に入るように移動する
    vec4 pos = a_position;              // ローカル座標系で操作する
    v_color = vec4(1.0, 1.0, 1.0, 1.0); // 描画可否判定用カラーベクトル初期化
    pos.xz = moveToInsightXZ(vec3(pos.x, 0.0, pos.z)).xz;

    // 座標変換
    gl_Position = CC_MVPMatrix * pos;

    // テクスチャ座標
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}