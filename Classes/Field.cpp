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
#include "Sprite3DBatchNode.h"
#include "ExMesh.h"

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

Field* Field::createWithModelPath(const std::string& modelPath)
{
    Field* field = new Field();
    if (field && field->initWithFile(modelPath)) {
        field->autorelease();
        return field;
    }
    CC_SAFE_DELETE(field);
    return nullptr;
}

Field* Field::createWithData(const FieldData& data, bool collisionMesh, bool subField)
{
    Field* field = Field::createWithModelPath(data.filenameTerrain);

    field->fieldId = data.id;
    field->fieldName = data.name;

    field->setupShaders(data);
    field->setupSpheres(data);

    // フィールドを無限に表示するための細工
    // シェーダでやろうとしたけどうまくいかなくて時間がないので頭悪いけど妥協
    if (subField) {
        for (int i = 0; i < 8; ++i) {
            Field* subField = Field::createWithData(data, false, false);
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
            field->addChild(subField);
        }
    }

    return field;
}

void Field::setupShaders(const FieldData& data)
{
    GLProgram* glProgram = GLProgram::createWithFilenames("DuplicateFieldShader.vert", "DuplicateFieldShader.frag");
    GLProgramState* glProgramState = GLProgramState::create(glProgram);
    this->setGLProgramState(glProgramState);

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

    // 法線マップがあれば指定 TODO: シェーダを分ける
    if (! data.filenameTextureNormal.empty()) {
        Texture2D* normalTexture = Director::getInstance()->getTextureCache()->addImage(data.filenameTextureNormal);
        glProgramState->setUniformTexture("u_normalMap", normalTexture);
    }
}

void Field::setupSpheres(const FieldData& data)
{
    vector<vector<Vec3>> sphereGroupList = Sphere::getSphereGroupPositionList(data.filenameSphereLine);
    static const int sphereDivisor = 10.f;      // TODO: スフィア配置の分割数
    static const int sphereDistribution = 100;  // TODO: スフィア配置の分散値

    for (const vector<Vec3>& sphereGroup : sphereGroupList) {
        Sprite3DBatchNode* sprite3DBatchNode = Sprite3DBatchNode::create("objects/diamond.obj");
        for (int i = 0, last = (int)sphereGroup.size() - 1; i < last; ++i) {
            const Vec3& p0 = sphereGroup[i];
            const Vec3& p1 = sphereGroup[i + 1];
            Vec3 distanceUnit = (p1 - p0) / (float)sphereDivisor;

            for (int j = 0; j < sphereDivisor; ++j) {
                for (int k = 0; k < 9; ++k) {
                    Vec3 dist(
                        rand() % sphereDistribution - sphereDistribution * 0.5,
                        rand() % sphereDistribution - sphereDistribution * 0.5,
                        rand() % sphereDistribution - sphereDistribution * 0.5
                    );
                    sprite3DBatchNode->add(p0 + distanceUnit * j + dist);
                }
            }
        }

        this->addChild(sprite3DBatchNode);
    }
}

void Field::setAirplaneToField(Airplane *airplane)
{
    this->airplane = airplane;
}

void Field::step(float dt)
{
    // TODO: 単位時間あたりの前進距離パラメータ
    float distance_per_dt = 400.f;
    float distance = distance_per_dt * dt;

    const Vec3& rotation = this->airplane->getRotation3D();
    float diff_rad_x = CC_DEGREES_TO_RADIANS(rotation.x);
    float diff_rad_y = CC_DEGREES_TO_RADIANS(rotation.y);

    Vec3 pos = this->getPosition3D();
    // Y軸回転を左右の進行距離に変換
    pos.x -= distance * sin(diff_rad_y);
    pos.z -= distance * cos(diff_rad_y);
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

    this->setPosition3D(pos);
}

int Field::getSphereCollisionCount()
{
    int coinCount = 0;
    const Vec3& airplanePosition = this->getAirplanePosition();
    float distance = 200;  // TODO: 当たり判定距離
    for (auto s = sphereList.begin(), last = sphereList.end(); s != last; ++s) {
        const Vec3 diff = (*s)->getPosition3D() -  airplanePosition;
        if (abs(diff.x) < distance && abs(diff.y) < distance && abs(diff.z) < distance &&  (*s)->isVisible()) {
            (*s)->setVisible(false);
            coinCount++;
        }
     }

    return coinCount;
}

int Field::getSphereCount() const
{
    return sphereList.size();
}

Vec3 Field::getAirplanePosition() const
{
    return -this->getPosition3D();
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