attribute vec4 a_position;
attribute vec2 a_texCoord;
uniform vec3 u_currentRotation;
uniform vec3 u_currentPosition;

varying vec2 v_texCoord;
varying vec4 v_color;

#define FIELD_LENGTH 10000.0
#define SQUARED_COS_60D 0.70  // cos(60d)^2
#define ANGLE_THRESHOLD 0.1
#define DISTANCE_THRESHOLD 300.0
#define NEAR_CLIP 5000.0
#define FAR_CLIP 8000.0

void checkMergin(float squaredCosPhi, float distance)
{
    if (abs(squaredCosPhi - SQUARED_COS_60D) < ANGLE_THRESHOLD) {
        v_color.a = 0.0;
    }

//    float distance = distance(position, origin);
    //if (abs(distance - NEAR_CLIP) < DISTANCE_THRESHOLD || abs(FAR_CLIP - distance) < DISTANCE_THRESHOLD) {
    if (abs(distance) < DISTANCE_THRESHOLD) {
        v_color.a = 0.0;
    }
}

vec3 moveToInsightXZ(in vec3 position)
{
    float theta = radians(u_currentRotation.y);
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // 進行方向ベクトル
    vec3 direction = vec3(sinTheta, 0.0, cosTheta);
    vec3 normal = vec3(cosTheta, 0.0, -sinTheta);
    // 視野の始点(始点を後ろに引きつつ角度を狭めて、台形っぽい視野を作る)
    vec3 origin = u_currentPosition - NEAR_CLIP * direction;
    // 目標ベクトル
    vec3 target = position - origin;

    // その場から頂点が見えている部分はそのまま
    float squaredCosPhi = pow(dot(direction, normalize(target)), 2.0);
    if (squaredCosPhi > SQUARED_COS_60D && NEAR_CLIP < length(target)) {
        return position;
    }

    // 頂点をX方向に移動してみて見えるかどうか確認
    float xDistance = target.x - u_currentPosition.x;
    float xMove = FIELD_LENGTH * -sign(xDistance);
    if (abs(xDistance) > DISTANCE_THRESHOLD) {
        vec3 targetXMoved = target + vec3(xMove, 0.0, 0.0);
        float squaredCosPhiXMoved = pow(dot(direction, normalize(targetXMoved)), 2.0);
        if (squaredCosPhiXMoved > SQUARED_COS_60D && length(targetXMoved) < NEAR_CLIP * 3.0) {
            checkMergin(squaredCosPhi, xDistance);
            // 見えるなら移動
            position.x += xMove;
            // 移動後の座標が視界の端なら表示しない
            checkMergin(squaredCosPhiXMoved, length(position) - NEAR_CLIP * 3.0);
            if (normal.x * normalize(position - u_currentPosition).z - normal.z * normalize(position - u_currentPosition).x < 0.0) {
                v_color.a = 0.0;
            }
            return position;
        }
    }

    // 頂点をZ方向に移動してみて見えるかどうか確認
    float zDistance = target.z - u_currentPosition.z;
    float zMove = FIELD_LENGTH * -sign(zDistance);
    if (abs(zDistance) > DISTANCE_THRESHOLD) {
        vec3 targetZMoved = target + vec3(0.0, 0.0, zMove);
        float squaredCosPhiZMoved = pow(dot(direction, normalize(targetZMoved)), 2.0);
        if (squaredCosPhiZMoved > SQUARED_COS_60D && length(targetZMoved) < NEAR_CLIP * 3.0) {
            checkMergin(squaredCosPhi, zDistance);
            // 見えるなら移動
            position.z += zMove;
            // 移動後の座標が視界の端なら表示しない
            checkMergin(squaredCosPhiZMoved, length(targetZMoved) - NEAR_CLIP * 3.0);
            if (normal.x * normalize(position - u_currentPosition).z - normal.z * normalize(position - u_currentPosition).x < 0.0) {
                v_color.a = 0.0;
            }
            return position;
        }
    }


    // 頂点を XZ方向に移動してみて見えるかどうか確認
    float squaredCosPhiXZMoved = pow(dot(direction, normalize(target + vec3(xMove, 0.0, zMove))), 2.0);
    if (squaredCosPhiXZMoved > SQUARED_COS_60D) {
        checkMergin(squaredCosPhi, DISTANCE_THRESHOLD + 1.0);
        // 見えるなら移動
        position.x += xMove;
        position.z += zMove;
        checkMergin(squaredCosPhi, DISTANCE_THRESHOLD + 1.0);
        if (normal.x * normalize(position - u_currentPosition).z - normal.z * normalize(position - u_currentPosition).x < 0.0) {
            v_color.a = 0.0;
        }
        return position;
    }

    // どれかには当てはまるはず
    v_color.a = 0.0;
    return vec3(0.0, 0.0, 0.0);
}

void main(void)
{
    vec4 pos = a_position;
    v_color = vec4(1.0, 1.0, 1.0, 1.0);

    vec3 movePos = moveToInsightXZ(vec3(pos.x, 0.0, pos.z));
    pos.xz = movePos.xz;

    gl_Position = CC_MVPMatrix * pos;

    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}