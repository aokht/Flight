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
#include "SphereBatch.h"
#include "ExMesh.h"

using namespace std;
using namespace cocos2d;

Field* Field::createById(int id, bool collisionMesh)
{
    const FieldData FieldData = FieldDataSource::findById(id);

    if (FieldData.id == FieldData::NOT_FOUND) {
        return nullptr;
    }

    return Field::createWithData(FieldData, collisionMesh);
}

Field* Field::createWithData(const FieldData& data, bool collisionMesh)
{
    Field* field = new Field();

    if (field) {
        field->enableCollisionDetection(collisionMesh);

        if (field->initWithFile(data.filenameTerrain)) {
            field->autorelease();
        }
    } else {
        delete field;
        field = nullptr;
        return nullptr;
    }

    field->fieldId = data.id;
    field->fieldName = data.name;

    field->setupShaders(data);
    field->setupSpheres(data);

    // シェーダでフィールドをつなげて見せるため
    // カメラにクリッピングされないように AABB を実際の 2倍程度に大きくごまかしておく
    ((ExMesh*)field->getMesh())->setAABB(AABB(Vec3(-FIELD_LENGTH, -FIELD_LENGTH, -FIELD_LENGTH), Vec3(FIELD_LENGTH, FIELD_LENGTH, FIELD_LENGTH)));

    return field;
}

void Field::setupShaders(const FieldData& data)
{
    GLProgram* glProgram = GLProgram::createWithFilenames("FieldShader.vert", "FieldShader.frag");
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

    // uniform をセット
    glProgramState->setUniformCallback("u_currentPosition", [this](GLProgram* program, Uniform* uniform) {
        const Vec3& p = this->getAirplanePosition();
        program->setUniformLocationWith3f(uniform->location, p.x, p.y, p.z);
    });
    glProgramState->setUniformCallback("u_currentRotation", [this](GLProgram* program, Uniform* uniform) {
        const Vec3& r = this->airplane->getRotation3D();
        program->setUniformLocationWith3f(uniform->location, r.x, r.y, r.z);
    });
}

void Field::setupSpheres(const FieldData& data)
{
    vector<vector<Vec3>> sphereGroupList = Sphere::getSphereGroupPositionList(data.filenameSphereLine);

#if SPRITE3DBATCHNODE_ENABLED
    SphereBatch* sphereBatch = SphereBatch::create();
    for (vector<Vec3> sphereGroup : sphereGroupList) {
        for (int i = 0, last = sphereGroup.size() - 1; i < last; ++i) {
            Vec3 p0 = sphereGroup[i];
            Vec3 p1 = sphereGroup[i + 1];
            float distance = p1.distance(p0);

            for (float j = 0.f; j < distance; j += 200.f) {
                for (int k = 0; k < 9; ++k) {
                    Vec3 position = p0 + (p1 - p0) / distance * j + Vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50);
                    sphereBatch->addSphere(position);
                }
            }
        }
    }
    sphereBatch->buildBatch();
    sphereBatch->setPosition3D(Vec3(3500, 0, 3000));
    this->addChild(sphereBatch);
#else
    for (vector<Vec3> sphereGroup : sphereGroupList) {
        for (int i = 0, last = sphereGroup.size() - 1; i < last; ++i) {
            Vec3 p0 = sphereGroup[i];
            Vec3 p1 = sphereGroup[i + 1];
            float distance = p1.distance(p0);
            for (float j = 0.f; j < distance; j += 200.f) {
                Vec3 position = p0 + (p1 - p0) / distance * j;
                Sprite3D* diamond = Sprite3D::create("objects/diamond.obj");
                diamond->setPosition3D(position);
                diamond->setScale(0.5);
                this->addChild(diamond);
                sphereList.push_back(diamond);
            }
        }
    }
#endif
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

    float adjust = 5000.f;
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