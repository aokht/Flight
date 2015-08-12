//
//  Airplane.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#include "Airplane.h"
#include "Global.h"
#include "AirplaneDataSource.h"

using namespace std;
using namespace cocos2d;

Airplane* Airplane::createById(int id)
{
    const AirplaneData airplaneData = AirplaneDataSource::findById(id);

    if (airplaneData.id == AirplaneData::NOT_FOUND) {
        return nullptr;
    }

    return createByData(airplaneData);
}

void Airplane::createByIdAsync(int id, const function<void(Airplane*, void*)>& callback, void* callbackparam)
{
    const AirplaneData airplaneData = AirplaneDataSource::findById(id);

    if (airplaneData.id == AirplaneData::NOT_FOUND) {
        return callback(nullptr, callbackparam);
    }

    createByDataAsync(airplaneData, callback, callbackparam);
}

Airplane* Airplane::createByData(const AirplaneData &airplaneData)
{
    Airplane* airplane = Airplane::createWithFilename(airplaneData.filename);

    airplane->airplaneId = airplaneData.id;
    airplane->airplaneName = airplaneData.name;
    airplane->speed = airplaneData.speed;
    airplane->rotationMax = airplaneData.rotationMax;
    airplane->rotationSpeed = airplaneData.rotationSpeed;
    airplane->thrusterSoundIndex = airplaneData.thrusterSoundIndex;

    return airplane;
}

void Airplane::createByDataAsync(const AirplaneData &airplaneData, const function<void(Airplane*, void*)>& callback, void* callbackparam)
{
    Airplane::createWithFilenameAsync(airplaneData.filename, [callback, airplaneData](Airplane* airplane, void* param){
        if (airplane) {
            airplane->airplaneId = airplaneData.id;
            airplane->airplaneName = airplaneData.name;
            airplane->speed = airplaneData.speed;
            airplane->rotationMax = airplaneData.rotationMax;
            airplane->rotationSpeed = airplaneData.rotationSpeed;
            airplane->thrusterSoundIndex = airplaneData.thrusterSoundIndex;
        }

        callback(airplane, param);
    }, callbackparam);
}

Airplane* Airplane::createWithFilename(const std::string &filename)
{
    Airplane* airplane = new Airplane();

    if (airplane && airplane->initWithFilename(filename)) {
        airplane->autorelease();
        return airplane;
    } else {
        delete airplane;
        airplane = nullptr;
        return nullptr;
    }
}

void Airplane::createWithFilenameAsync(const std::string &filename, const function<void(Airplane*, void*)>& callback, void* callbackparam)
{
    Airplane* airplane = new Airplane();

    if (airplane) {
        airplane->initWithFilenameAsync(filename, [callback](Airplane* a, void* param){
            a->autorelease();
            callback(a, param);
        }, callbackparam);
    } else {
        CC_SAFE_DELETE(airplane);
        callback(nullptr, callbackparam);
    }
}

bool Airplane::initWithFilename(const string& filename)
{
    if (! Node::init()) {
        return false;
    }

    this->spriteAirplane = Sprite3D::create(filename);
    if (! this->spriteAirplane) {
        return false;
    }
    this->addChild(spriteAirplane);

    this->setCascadeOpacityEnabled(true);
    spriteAirplane->setCascadeOpacityEnabled(true);

    this->rotationTarget = Vec3(0, 0, 0);

    return true;
}

void Airplane::initWithFilenameAsync(const string& filename, const function<void(Airplane*, void*)>& callback, void* callbackparam)
{
    if (! Node::init()) {
        callback(nullptr, callbackparam);
    }

    Sprite3D::createAsync(filename, [this, callback](Sprite3D* airplane, void* param){
        this->spriteAirplane = airplane;
        this->addChild(airplane);
        callback(this, param);
    }, callbackparam);

    this->rotationTarget = Vec3(0, 0, 0);
}

void Airplane::setCameraToAirplane(Camera* camera)
{
    // 後方斜め上から前方を見る
    camera->setPosition3D(Vec3(0, 1, -5));
    camera->lookAt(this->spriteAirplane->getPosition3D() + Vec3(0.f, 0.f, 10000.f));
    this->addChild(camera);
}

#pragma mark -
#pragma mark 姿勢制御

void Airplane::step(float dt)
{
    this->rotate(dt);
    // 前進は Field を動かすことによって行う
}

void Airplane::rotate(float dt)
{
    if (dt == 0.f) {
        // たまにある(0割が起きるのではじく)
        return;
    }

    // 機体を傾ける (Sprite に対する操作)

    // 最大回転可能量
    const Vec3& rotationMax = this->getRotationMax();
    // 単位時間あたりの回転可能量
    const Vec3& rotationSpeed = this->getRotationSpeed();
    // 現在の回転量
    Vec3 currentRotation = this->spriteAirplane->getRotation3D();

    // 必要回転量に合わせて回転速度にバイアスをかける(必要回転量が少ないほど遅くなる)
    float bias_x = min(abs(rotationTarget.x - currentRotation.x), rotationMax.x * 2.f) / rotationMax.x * 2.f;
    float bias_z = min(abs(rotationTarget.z - currentRotation.z), rotationMax.z * 2.f) / rotationMax.z * 2.f;

    Vec3 rotationDiff = rotationSpeed * dt;
    rotationDiff.x *= bias_x;
    rotationDiff.z *= bias_z;

    // X軸(上下)回転
    float diff_x = min(rotationDiff.x, abs(rotationTarget.x - currentRotation.x)) * sign(rotationTarget.x - currentRotation.x);
    if (abs(currentRotation.x + diff_x) > rotationMax.x) {
        diff_x = (rotationMax.x - abs(currentRotation.x)) * sign(diff_x);
    }
    // Z軸(左右)回転
    float diff_z = min(rotationDiff.z, abs(rotationTarget.z - currentRotation.z)) * sign(rotationTarget.z - currentRotation.z);
    if (abs(currentRotation.z + diff_z) > rotationMax.z) {
        diff_z = (rotationMax.z - abs(currentRotation.z)) * sign(diff_z);
    }

    this->spriteAirplane->setRotation3D(currentRotation + Vec3(diff_x, 0.f, diff_z));


    // 進行方向を変える (Node に対する操作)

    // 機体の傾きに応じて進行方向を変える
    const Vec3& spriteRotation = this->spriteAirplane->getRotation3D();
    // Z軸の傾きを Y軸の進行方向として扱う
    Vec3 direction(spriteRotation.x * dt, spriteRotation.z * dt, 0.f);
    this->setRotation3D(this->getRotation3D() + direction);
}

void Airplane::setRotationToDefault(float dt)
{
    // Z軸(左右)は、スプライトの回転量を 0 に向かわせる
    float rotationZ = 0.f;

    // X軸(上下)は、機体自体の回転量を 0 に向かわせる
    float currentRotationX = this->getRotation3D().x;
    float rotationX = -currentRotationX;  // スプライトの目標回転量を機体回転量の反対側にする
    if (abs(rotationX) > 180.f) {  // 反転していたときの対応
        rotationX = (360.f - abs(rotationX)) * sign(-rotationX);
    }
    rotationX = min(abs(rotationX), rotationMax.x) * sign(rotationX);

    // 完全に反対側を目標にすると 0度付近で上下の揺れが起きるので、近づいたら目標回転量を半分にする
    if (abs(currentRotationX) < rotationMax.x * 2.f) {
        rotationX *= 0.5f;
    }

    this->rotationTarget = Vec3(rotationX, 0, rotationZ);
}

Vec3 Airplane::getRotation3D() const
{
    // 360度を超えないようにする
    Vec3 r = Node::getRotation3D();
    return Vec3(fmod(r.x, 360.f), fmod(r.y, 360.f), fmod(r.z, 360.f));
}

Vec3 Airplane::getDirection() const
{
    const Vec3& r = this->getRotation3D();
    float diff_rad_x = CC_DEGREES_TO_RADIANS(r.x);
    float diff_rad_y = CC_DEGREES_TO_RADIANS(r.y);

    Vec3 direction;
    direction.x = sin(diff_rad_y);
    direction.z = cos(diff_rad_y);
    direction.y = sin(diff_rad_x);

    return direction;
}

#pragma mark -
#pragma mark 入力応答

void Airplane::onInputBegan()
{
    // 入力開始時の目標回転量
    this->rotationStart = this->rotationTarget;
}

void Airplane::onInputMoved(const cocos2d::Vec2& diff)
{
    const Vec3& rotationMax = this->getRotationMax();

    // XY平面(画面上)の Y軸(縦)方向移動量 → XYZ空間のX軸(上下)回転量に変換
    //     　〃　      X軸(横)方向移動量 →    〃　　Z軸(左右)回転量に変換
    float rotationDiffX = diff.y * rotationMax.x * 2.f; // 最大回転量の2倍の範囲
    float rotationDiffZ = diff.x * rotationMax.z * 2.f; // 最大回転量の2倍の範囲

    // 入力開始時の目標回転量に差分を加算
    this->rotationTarget.x = this->rotationStart.x + rotationDiffX;
    this->rotationTarget.z = this->rotationStart.z - rotationDiffZ;

    // 最大回転量を超えないようにする
    if (abs(rotationTarget.x) > rotationMax.x) {
        this->rotationTarget.x = rotationMax.x * sign(rotationTarget.x);
    }
    if (abs(rotationTarget.z) > rotationMax.z) {
        this->rotationTarget.z = rotationMax.z * sign(rotationTarget.z);
    }
}

void Airplane::onInputEnded(const cocos2d::Vec2& diff)
{
    // 入力開始時の目標回転量をリセット
    this->rotationStart.setZero();
}

#pragma mark -
#pragma mark パラメータ

int Airplane::getAirplaneId() const
{
    return airplaneId;
}

const string& Airplane::getAirplaneName() const
{
    return airplaneName;
}

#pragma mark -
#pragma mark デバッグ用

Vec3 Airplane::getSpriteRotation() const
{
    return this->spriteAirplane->getRotation3D();
}

Vec3 Airplane::getRotationTarget() const
{
    return this->rotationTarget;
}

float Airplane::getSpeed() const
{
    return speed;
}

const Vec3& Airplane::getRotationSpeed() const
{
    return rotationSpeed;
}

const Vec3& Airplane::getRotationMax() const
{
    return rotationMax;
}

int Airplane::getThrusterSoundIndex() const
{
    return thrusterSoundIndex;
}