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

    return airplane;
}

void Airplane::createByDataAsync(const AirplaneData &airplaneData, const function<void(Airplane*, void*)>& callback, void* callbackparam)
{
    Airplane::createWithFilenameAsync(airplaneData.filename, [callback, airplaneData](Airplane* airplane, void* param){
        airplane->airplaneId = airplaneData.id;
        airplane->airplaneName = airplaneData.name;
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

    this->rotationStep = Vec3(0, 0, 0);

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

    this->rotationStep = Vec3(0, 0, 0);
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

    // TODO: 単位時間あたりの回転可能量パラメータ
    float param_rotation_diff = dt * 70.f;

    Vec3 currentRotation = this->spriteAirplane->getRotation3D();

    // 必要回転量に合わせて回転速度にバイアスをかける
    float bias_x = 0.10 * (abs((rotationStep - currentRotation).x) / (2 * param_rotation_diff));
    float bias_z = 0.10 * (abs((rotationStep - currentRotation).z) / (2 * param_rotation_diff));

    float rotation_diff_x = param_rotation_diff * bias_x;
    float rotation_diff_z = param_rotation_diff * bias_z;

    float param_max_z = 40.f;  // TODO: 最大回転可能量(Z軸)
    float param_max_x = 15.f;  // TODO: 最大回転可能量(X軸)
    // X軸(上下)回転
    float diff_x = min(rotation_diff_x, abs(currentRotation.x - rotationStep.x)) * (rotationStep.x > currentRotation.x ? 1.f : -1.f);
    if (abs(currentRotation.x + diff_x) > param_max_x) {
        diff_x = (param_max_x - abs(currentRotation.x)) * (diff_x < 0.f ? -1.f : 1.f);
    }
    // Z軸(左右)回転
    float diff_z = min(rotation_diff_z, abs(currentRotation.z - rotationStep.z)) * (rotationStep.z > currentRotation.z ? 1.f : -1.f);
    if (abs(currentRotation.z + diff_z) > param_max_z) {
        diff_z = (param_max_z - abs(currentRotation.z)) * (diff_z < 0.f ? -1.f : 1.f);
    }

    this->spriteAirplane->setRotation3D(currentRotation + Vec3(diff_x, 0.f, diff_z));


    // 進行方向を変える (Node に対する操作)

    // TODO: 単位時間あたりの回転量パラメータ
    float param_direction_diff_x = dt * 1.0f;
    float param_direction_diff_z = dt * 1.0f;

    // 機体の傾きに応じて進行方向を変える
    Vec3 spriteRotation = this->spriteAirplane->getRotation3D();
    // Z軸の傾きを Y軸の進行方向として扱う
    Vec3 direction(spriteRotation.x * param_direction_diff_x, spriteRotation.z * param_direction_diff_z, 0.f);
    this->setRotation3D(this->getRotation3D() + direction);
}

void Airplane::setRotationToDefault(float dt)
{
    // Z軸(左右)のスプライトの回転量を 0 に向かわせる
    // X軸(上下)は機体自体の回転量を 0 に近づける
    float rotation_x = -this->getRotation3D().x;
    if (abs(rotation_x) > 180.f) {
        rotation_x = (360.f - abs(rotation_x)) * (rotation_x < 0.f ? 1.f : -1.f);
    }

    this->rotationStep = Vec3(rotation_x * 0.5, 0, 0);
    if (abs(this->getRotation3D().x) < 0.1f) {
        this->rotationStep.x = 0.f;
    }
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
    float param_max_y = 40.f;  // TODO: 最大回転可能量(Y軸)
    float param_max_x = 15.f;  // TODO: 最大回転可能量(X軸)
    this->rotationStep.x = min(abs(this->rotationStart.x + diff_y), param_max_x) * (this->rotationStart.x + diff_y < 0.f ? -1.f : 1.f);
    this->rotationStep.z = min(abs(this->rotationStart.z - diff_x), param_max_y) * (this->rotationStart.z - diff_x < 0.f ? -1.f : 1.f);
}

void Airplane::onInputEnded(const cocos2d::Vec2& diff)
{
    // 開始ベクトルと開始回転量をリセット
    this->rotationStart.setZero();
    this->spriteRotationStart.setZero();
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
    return this->rotationStep;
}