//
//  Airplane.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#include "Airplane.h"
#include "Global.h"

using namespace std;
using namespace cocos2d;

bool Airplane::init()
{
    if (! Node::init()) {
        return false;
    }

    this->spriteAirplane = Sprite3D::create("airplanes/boss.obj");
    if (! this->spriteAirplane) {
        return false;
    }


    this->rotationStep = Vec3(0, 0, 0);

    return true;
}

void Airplane::onEnter()
{
    Node::onEnter();

    spriteAirplane->setTexture("airplanes/boss.png");
    spriteAirplane->setScale(0.12);
    spriteAirplane->setPosition3D(Vec3(0, 0, 0));
    this->addChild(spriteAirplane);
}

void Airplane::setCameraToAirplane(Camera* camera)
{
    // 後方斜め上から見下ろす
    camera->setPosition3D(Vec3(0, 1, -5));
    camera->lookAt(this->spriteAirplane->getPosition3D());
    this->addChild(camera);
}

#pragma mark -
#pragma mark 姿勢制御

void Airplane::step(float dt)
{
    this->rotate(dt);
    this->goForward(dt);
}

void Airplane::rotate(float dt)
{
    // 機体を傾ける (Sprite に対する操作)

    // TODO: 単位時間あたりの回転可能量パラメータ
    float param_rotation_diff = dt * 50.f;

    Vec3 currentRotation = this->spriteAirplane->getRotation3D();
    // X軸(上下)回転
    float diff_x = min(param_rotation_diff, abs(currentRotation.x - rotationStep.x)) * (rotationStep.x > currentRotation.x ? 1.f : -1.f);
    // Z軸(左右)回転
    float diff_z = min(param_rotation_diff, abs(currentRotation.z - rotationStep.z)) * (rotationStep.z > currentRotation.z ? 1.f : -1.f);

    this->spriteAirplane->setRotation3D(currentRotation + Vec3(diff_x, 0.f, diff_z));


    // 進行方向を変える (Node に対する操作)

    // TODO: 単位時間あたりの回転可能量パラメータ
    float param_direction_diff = dt * 1.f;

    // 機体の傾きに応じて進行方向を変える
    Vec3 spriteRotation = this->spriteAirplane->getRotation3D();
    // Z軸の傾きを Y軸の進行方向として扱う
    Vec3 direction(spriteRotation.x, spriteRotation.z, 0.f);
    this->setRotation3D(this->getRotation3D() + direction * param_direction_diff);
}

void Airplane::goForward(float dt)
{

    // TODO: 単位時間あたりの前進距離パラメータ
    float distance_per_dt = 2500.f;
    float distance = distance_per_dt * dt;

    const Vec3& rotation = this->getRotation3D();
    float diff_rad_x = CC_DEGREES_TO_RADIANS(rotation.x);
    float diff_rad_y = CC_DEGREES_TO_RADIANS(rotation.y);

    Vec3 pos = this->getPosition3D();
    // Y軸回転を左右の進行距離に変換
    pos.x += distance * sin(diff_rad_y);
    pos.z += distance * cos(diff_rad_y);
    // X軸回転を上下の進行距離に変換
    pos.y += distance * sin(-diff_rad_x);

    cout << rotationStep << ", " << rotation << ", " << pos << endl;
    this->setPosition3D(pos);
}

void Airplane::setRotationToDefault(float dt)
{
    this->rotationStep.setZero();
}

#pragma mark -
#pragma mark 入力応答

void Airplane::onInputBegan()
{
    this->rotationStart = this->rotationStep;
    this->spriteRotationStart = spriteAirplane->getRotation3D();
}

void Airplane::onInputMoved(const cocos2d::Vec2& diff)
{
    float ux_px_to_theta_x = 0.5f; // TODO: 1px のドラッグに対して何度傾けるか(X)
    float ux_px_to_theta_y = 0.2f; // TODO: 1px のドラッグに対して何度傾けるか(Y)
    float diff_x = diff.x * ux_px_to_theta_x;
    float diff_y = diff.y * ux_px_to_theta_y;

    // 入力開始時の回転量に差分を加算

    // XY平面の Y軸(縦)方向 → XYZ空間のX軸(上下)回転に変換
    //   　〃　 X軸(横)方向 →    〃　　Z軸(左右)逆回転に変換
    float param_max_y = 30.f;  // TODO: 最大回転可能量(Y軸)
    float param_max_x = 10.f;  // TODO: 最大回転可能量(X軸)
    this->rotationStep.x = min(abs(this->rotationStart.x + diff_y), param_max_x) * (this->rotationStart.x + diff_y < 0.f ? -1.f : 1.f);
    this->rotationStep.z = min(abs(this->rotationStart.z - diff_x), param_max_y) * (this->rotationStart.z - diff_x < 0.f ? -1.f : 1.f);
}

void Airplane::onInputEnded(const cocos2d::Vec2& diff)
{
    // 開始ベクトルと開始回転量をリセット
    this->rotationStart.setZero();
    this->spriteRotationStart.setZero();
}