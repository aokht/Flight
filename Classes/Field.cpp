//
//  Field.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#include <iostream>
#include "Field.h"
#include "ExSprite3D.h"
#include "FieldDataSource.h"
#include "Airplane.h"
#include "Global.h"
#include "Sphere.h"
#include "MiniMap.h"

using namespace std;
using namespace cocos2d;

Field* Field::createById(int id, bool collisionMesh, bool subField)
{
    const FieldData FieldData = FieldDataSource::findById(id);

    if (FieldData.id == FieldData::NOT_FOUND) {
        return nullptr;
    }

    return Field::createWithData(FieldData, collisionMesh, subField);
}

Field* Field::createWithModelPath(const std::string& modelPath, bool collisionMesh)
{
    Field* field = new Field();
    if (field) {
        field->enableCollisionDetection(collisionMesh);
        if (field->initWithFile(modelPath)) {
            field->autorelease();
            return field;
        }
    }

    CC_SAFE_DELETE(field);
    return nullptr;
}

void Field::createWithModelPathAsync(const std::string& modelPath, const function<void(Field*, void*)>& callback, void* callbackparam, bool collisionMesh)
{
    Field* field = new Field();

    if (field) {
        if (field->loadFromCache(modelPath)) {
            field->autorelease();
            callback(field, callbackparam);
            return;
        }

        // 頂点情報はデフォルトで捨てられてしまうためシャドーコピーを作成する(キャッシュされていない場合)
        VertexBuffer::enableShadowCopy(true);
        IndexBuffer::enableShadowCopy(true);

        field->enableCollisionDetection(collisionMesh);
        field->initWithFileAsync(modelPath, [callback](ExSprite3D* sprite, void* param){
            Field* field = static_cast<Field*>(sprite);
            if (field->collisionDetectionEnabled()) {
                field->extractVertexInfo();
                field->buildCollisionMesh();
            }
            callback(field, param);
        }, callbackparam);
    } else {
        callback(nullptr, callbackparam);
    }
}

Field* Field::createWithData(const FieldData& data, bool collisionMesh, bool subField)
{
    Field* field = Field::createWithModelPath(data.filenameTerrain, collisionMesh);

    field->fieldId = data.id;
    field->fieldName = data.name;
    field->airplaneStartPosition = data.airplanePosition;
    field->airplaneStartRotation = data.airplaneRotation;
    field->otherAirplaneStartPosition = data.otherAirplanePosition;
    field->otherAirplaneStartRotation = data.otherAirplaneRotation;
    field->leaderboardHighScoreKey = data.leaderboardHighScoreKey;
    field->bgmIndex = data.bgmIndex;

    field->setupShaders(data);

    if (subField) {
        field->setupSubFields();
    }

    return field;
}

void Field::createWithDataAsync(const FieldData& data, const function<void(Field*, void*)>& callback, void* callbackparam, bool collisionMesh, bool subField)
{
    Field::createWithModelPathAsync(data.filenameTerrain, [data, subField, callback](Field* field, void* param){
        if (field) {
            field->fieldId = data.id;
            field->fieldName = data.name;
            field->airplaneStartPosition = data.airplanePosition;
            field->airplaneStartRotation = data.airplaneRotation;
            field->otherAirplaneStartPosition = data.otherAirplanePosition;
            field->otherAirplaneStartRotation = data.otherAirplaneRotation;
            field->leaderboardHighScoreKey = data.leaderboardHighScoreKey;
            field->bgmIndex = data.bgmIndex;

            field->setupShaders(data);

            if (subField) {
                field->setupSubFields();
            }
        }

        callback(field, param);
    }, callbackparam, collisionMesh);
}

void Field::setupSubFields()
{
    // FIXME: フィールドを無限に表示するための細工
    // シェーダでやろうとしたけどうまくいかなくて時間がないので頭悪いけど妥協
    for (int i = 0; i < 8; ++i) {
        Field* subField = Field::createById(this->getFieldId(), false, false);
        switch (i) {
            case 0:
                subField->setPosition3D(Vec3(-FIELD_LENGTH, 0.f,  FIELD_LENGTH));
                break;
            case 1:
                subField->setPosition3D(Vec3(          0.f, 0.f,  FIELD_LENGTH));
                break;
            case 2:
                subField->setPosition3D(Vec3( FIELD_LENGTH, 0.f,  FIELD_LENGTH));
                break;
            case 3:
                subField->setPosition3D(Vec3(-FIELD_LENGTH, 0.f,           0.f));
                break;
            case 4:
                subField->setPosition3D(Vec3( FIELD_LENGTH, 0.f,           0.f));
                break;
            case 5:
                subField->setPosition3D(Vec3(-FIELD_LENGTH, 0.f, -FIELD_LENGTH));
                break;
            case 6:
                subField->setPosition3D(Vec3(          0.f, 0.f, -FIELD_LENGTH));
                break;
            case 7:
                subField->setPosition3D(Vec3( FIELD_LENGTH, 0.f, -FIELD_LENGTH));
                break;
        }
        this->addChild(subField);
        this->subFieldList.push_back(subField);
    }
}

void Field::setupShaders(const FieldData& data)
{
    GLProgram* glProgram;

    // 法線マップの有無でシェーダを分ける
    if (! data.filenameTextureNormal.empty()) {
        glProgram = GLProgram::createWithFilenames("DuplicateFieldShader.vert", "DuplicateFieldShader.frag");
    } else {
        glProgram = GLProgram::createWithFilenames("DuplicateFieldShader.vert", "DuplicateFieldShaderNoNormal.frag");
    }

    GLProgramState* glProgramState = GLProgramState::create(glProgram);
    this->setGLProgramState(glProgramState);
    for (Node* child : _children) {
        child->setGLProgramState(glProgramState);
    }

    // メッシュ取得(obj形式では1つのみの前提)
    MeshVertexData* mesh = this->_meshVertexDatas.at(0);
    for (ssize_t i = 0, attributeCount = mesh->getMeshVertexAttribCount(), offset = 0; i < attributeCount; i++) {
        const MeshVertexAttrib& meshattribute = mesh->getMeshVertexAttrib(i);
        glProgramState->setVertexAttribPointer(
            s_attributeNames[meshattribute.vertexAttrib],
            meshattribute.size,
            meshattribute.type,
            GL_FALSE,
            mesh->getVertexBuffer()->getSizePerVertex(),
            (GLvoid*)offset
        );

        offset += meshattribute.attribSizeBytes;
    }

    // 法線マップがあれば指定
    if (! data.filenameTextureNormal.empty()) {
        Texture2D* normalTexture = Director::getInstance()->getTextureCache()->addImage(data.filenameTextureNormal);
        glProgramState->setUniformTexture("u_normalMap", normalTexture);
    }
}

// 1座標の周りに生成するスフィアの数
const int Field::sphereCountPerPoint = 12;

void Field::setupSpheres()
{
    FieldData data = FieldDataSource::findById(this->getFieldId());
    vector<vector<Sphere::SphereGroupInfo>> sphereGroupList = Sphere::getSphereGroupInfoList(data.filenameSphereLine);
    static const int sphereDivisor = 10.f;      // TODO: スフィア配置の分割数
    static const int sphereDistribution = 100;  // TODO: スフィア配置の分散値

    for (const vector<Sphere::SphereGroupInfo>& sphereGroup : sphereGroupList) {
        if (sphereGroup.empty()) {
            continue;
        }
        Sphere* sphere = Sphere::createWithType(sphereGroup[0].type);
        for (int i = 0, last = (int)sphereGroup.size() - 1; i < last; ++i) {
            const Vec3& p0 = sphereGroup[i].position;
            const Vec3& p1 = sphereGroup[i + 1].position;
            Vec3 distanceUnit = (p1 - p0) / (float)sphereDivisor;

            for (int j = 0; j < sphereDivisor; ++j) {
                for (int k = 0; k < sphereCountPerPoint; ++k) {
                    Vec3 dist(
                        rand() % sphereDistribution - sphereDistribution * 0.5,
                        rand() % sphereDistribution - sphereDistribution * 0.5,
                        rand() % sphereDistribution - sphereDistribution * 0.5
                    );
                    Vec3 position = p0 + distanceUnit * j + dist;
                    sphere->add(position);
                }
            }
        }

        this->addChild(sphere);
        sphereBatchList.push_back(sphere);

        if (sphereCountPerColor.find(sphere->getType()) == sphereCountPerColor.end()) {
            sphereCountPerColor[sphere->getType()] = 0;
        }
        sphereCountPerColor[sphere->getType()] += sphere->getNodeCount();
    }

    // sphereの状態を共有する FIXME: 絶対危険
    for (Field* subField : subFieldList) {
        subField->shareSphereList(sphereBatchList);
    }
}

void Field::shareSphereList(std::vector<Sphere*> sphereBatchList)
{
    for (Sphere* src : sphereBatchList) {
        Sphere* dst = Sphere::create(*src);
        Sphere::makeBatchNodeShared(*src, dst);
        this->addChild(dst);
        this->sphereBatchList.push_back(dst);
    }
}

void Field::setAirplane(Airplane *airplane)
{
    this->airplane = airplane;

    // フィールドを動かす
    this->setPosition3D(-airplane->getPosition3D());
    airplane->setPosition3D(Vec3::ZERO);
}

void Field::setMiniMap(MiniMap *miniMap)
{
    this->miniMap = miniMap;
    this->miniMap->setDivisor(sphereCountPerPoint);

    for (Sphere* sphereBatch : sphereBatchList) {
        this->miniMap->addSphereBatch(sphereBatch);
    }
    this->miniMap->step(0.f, this->getAirplanePosition(), this->airplane->getRotation3D());
}

void Field::step(float dt)
{
    // 単位時間あたりの前進距離パラメータ
    float distance_per_dt = this->airplane->getSpeed();
    float distance = distance_per_dt * dt;

    const Vec3& rotation = this->airplane->getRotation3D();
    float diff_rad_x = CC_DEGREES_TO_RADIANS(rotation.x);
    float diff_rad_y = CC_DEGREES_TO_RADIANS(rotation.y);

    Vec3 pos = this->getPosition3D();
    // XY軸回転を左右の進行距離に変換
    pos.x -= distance * sin(diff_rad_y) * cos(diff_rad_x);
    pos.z -= distance * cos(diff_rad_y) * cos(diff_rad_x);
    // X軸回転を上下の進行距離に変換
    pos.y -= distance * sin(-diff_rad_x);


    // XZ方向は、端まで行ったら逆端に移動
    float adjust = FIELD_LENGTH * 0.5f;
    pos.x = fmod(pos.x + adjust, adjust * 2.f) - adjust;
    if (pos.x < -adjust) {
        pos.x += adjust * 2.f;
    }

    pos.z = fmod(pos.z + adjust, adjust * 2.f) - adjust;
    if (pos.z < -adjust) {
        pos.z += adjust * 2.f;
    }

    // Y方向は最大より上には行けないようにする
    pos.y = max(pos.y, (float)-FIELD_HEIGHT);

    this->setPosition3D(pos);

    this->miniMap->step(dt, this->getAirplanePosition(), this->airplane->getRotation3D());
}

void Field::checkSphereCollision(vector<AchievedSphereInfo>* achievedSphereInfoListPerFrame)
{
    const Vec3& airplanePosition = this->getAirplanePosition();
    float distance = 150.f;  // TODO: 当たり判定距離

    for (auto batchNode = sphereBatchList.begin(), lastBatchNode = sphereBatchList.end(); batchNode != lastBatchNode; ++batchNode) {
        const vector<Sprite3DBatchNode::NodeStatus>& sphereList = (*batchNode)->getNodeStatusList();
        for (auto s = sphereList.begin(), last = sphereList.end(); s != last; ++s) {
            const Vec3 diff = s->position -  airplanePosition;
            if (abs(diff.x) < distance && abs(diff.y) < distance && abs(diff.z) < distance &&  s->isVisible) {
                (*batchNode)->setNodeVisible((int)std::distance(sphereList.begin(), s), false);

                AchievedSphereInfo achievedSphereInfo({
                    (int)std::distance(sphereBatchList.begin(), batchNode),
                    (int)std::distance(sphereList.begin(), s),
                    (*batchNode)->getType(),
                    1  // TODO: peerId とか
                });
                achievedSphereList.push_back(achievedSphereInfo);
                achievedSphereInfoListPerFrame->push_back(achievedSphereInfo);
            }
        }
    }
}

const vector<AchievedSphereInfo>& Field::getAchievedSphereInfoList() const
{
    return achievedSphereList;
}

int Field::getSphereCount() const
{
    int count = 0;
    for (Sprite3DBatchNode* batchNode : sphereBatchList) {
        count += batchNode->getNodeCount();
    }

    return count;
}

int Field::getRemainingSphereCount() const
{
    int count = 0;
    for (Sprite3DBatchNode* batchNode : sphereBatchList) {
        count += batchNode->getRemainingNodeCount();
    }

    return count;
}

const map<Sphere::Type, int>& Field::getSphereCountPerColor() const
{
    return sphereCountPerColor;
}

Vec3 Field::getAirplanePosition() const
{
    return -this->getPosition3D();
}

void Field::setOtherAirplane(int peerId, Airplane *airplane)
{
    this->otherAirplaneList[peerId] = airplane;
    airplane->setCameraMask((unsigned short)CameraFlag::USER1);
    this->addChild(airplane);

    // FIXME: 頭悪い
    for (Field* subField : subFieldList) {
        Airplane* subAirplane = Airplane::createById(airplane->getAirplaneId());
        subAirplane->setPosition3D(airplane->getPosition3D());
        subField->setOtherAirplane(peerId, subAirplane);
    }
}

void Field::setOtherAirplaneAchievedSphere(int peerId, int sphereCount, const AchievedSphereInfo* sphereInfoList)
{
    for (int i = 0; i < sphereCount; ++i) {
        const AchievedSphereInfo& sphereInfo = sphereInfoList[i];

        // batchId と sphereId のバリデーション
        if (sphereInfo.batchId < 0 || sphereBatchList.size() <= sphereInfo.batchId) {
            return;
        }
        Sprite3DBatchNode* batchNode = sphereBatchList[sphereInfo.batchId];

        if (sphereInfo.sphereId < 0 || batchNode->getNodeCount() <= sphereInfo.sphereId) {
            return;
        }

        batchNode->setNodeVisible(sphereInfo.sphereId, false);
        otherAirplaneAchievedSphereList[peerId].push_back(sphereInfo);
    }
}

const map<int, vector<AchievedSphereInfo>>& Field::getOtherAirplaneAchievedSphereList() const
{
    return otherAirplaneAchievedSphereList;
}

void Field::setOtherAirplaneInfo(int peerId, const Vec3& position, const Vec3& rotation)
{
    Airplane* targetAirplane = this->otherAirplaneList[peerId];

    targetAirplane->setPosition3D(position);
    targetAirplane->setRotation3D(rotation);

    // FIXME: 頭悪い
    for (Field* subField: subFieldList) {
        subField->setOtherAirplaneInfo(peerId, position, rotation);
    }
}

const map<int, Airplane*>& Field::getOtherAirplaneList() const
{
    return this->otherAirplaneList;
}


#pragma mark -
#pragma mark パラメータ

int Field::getFieldId() const
{
    return fieldId;
}

const string& Field::getFieldName() const
{
    return fieldName;
}

const Vec3& Field::getAirplaneStartPosition() const
{
    return airplaneStartPosition;
}

const Vec3& Field::getAirplaneStartRotation() const
{
    return airplaneStartRotation;
}

const Vec3& Field::getOtherAirplaneStartPosition() const
{
    return otherAirplaneStartPosition;
}

const Vec3& Field::getOtherAirplaneStartRotation() const
{
    return otherAirplaneStartRotation;
}

const string& Field::getLeaderboardHighScoreKey() const
{
    return leaderboardHighScoreKey;
}

int Field::getBGMIndex() const
{
    return bgmIndex;
}