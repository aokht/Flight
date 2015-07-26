//
//  ExSprite3D.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#include <iostream>
#include "ExSprite3D.h"
#include "ExVertexIndexBuffer.h"
#include "CollisionMesh.h"
#include "Global.h"

using namespace std;
using namespace cocos2d;

#pragma mark -
#pragma mark Life Cycle

ExSprite3D::ExSprite3D() :
    _collisionDetectionEnabled(false),
    collisionMesh(nullptr)
{
}

ExSprite3D::~ExSprite3D()
{
    if (_collisionDetectionEnabled) {
        this->collisionMesh->release();
    }
}

ExSprite3D* ExSprite3D::create(const std::string &modelPath)
{
    if (modelPath.length() < 4)
        CCASSERT(false, "invalid filename for ExSprite3D");

    auto sprite = new (std::nothrow) ExSprite3D();
    if (sprite && sprite->initWithFile(modelPath))
    {
        sprite->_contentSize = sprite->getBoundingBox().size;
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool ExSprite3D::initWithFile(const string &path)
{
    // 頂点情報はデフォルトで捨てられてしまうためシャドーコピーを作成する(キャッシュされていない場合)
    auto spritedata = Sprite3DCache::getInstance()->getSpriteData(path);
    if (! spritedata) {
        VertexBuffer::enableShadowCopy(true);
        IndexBuffer::enableShadowCopy(true);
    }

    if (! Sprite3D::initWithFile(path)) {
        return false;
    }

    // TODO: Cacheとか
    if (collisionDetectionEnabled()) {
        this->extractVertexInfo();
        this->buildCollisionMesh();
    }

    return true;
}

#pragma mark -
#pragma mark Collision Detection

void ExSprite3D::extractVertexInfo()
{
    positionList.clear();
    normalList.clear();
    triangleList.clear();

    // メッシュ情報を取得(.objファイルでは1つだけの想定)
    CCASSERT(this->_meshVertexDatas.size() == 1, "invalid meshVertexData size");
    MeshVertexData* meshVertexData = this->_meshVertexDatas.at(0);

    // 頂点情報を取得
    const ExVertexBuffer* vertexBuffer = (const ExVertexBuffer*)meshVertexData->getVertexBuffer();
    const vector<unsigned char>& rawVertexList = vertexBuffer->getShadowCopy();
    // unsigned char -> float 変換 orz
    const float* vertexListArray = reinterpret_cast<const float*>(rawVertexList.data());

    // 各座標の有無と、面あたりの要素数をカウント (頂点3座標、法線3座標、テクスチャ2座標)
    bool hasPosition = false, hasNormal = false, hasTexture = false;
    int sizePerMesh = 0;
    for (ssize_t i = 0, last = meshVertexData->getMeshVertexAttribCount(); i < last; ++i) {
        const MeshVertexAttrib& attrib = meshVertexData->getMeshVertexAttrib(i);

        hasPosition |= (attrib.vertexAttrib == GLProgram::VERTEX_ATTRIB_POSITION);
        hasNormal   |= (attrib.vertexAttrib == GLProgram::VERTEX_ATTRIB_NORMAL);
        hasTexture  |= (attrib.vertexAttrib == GLProgram::VERTEX_ATTRIB_TEX_COORD);
        sizePerMesh += attrib.size;
    }

    size_t vertexCount = rawVertexList.size() / sizeof(float);
    size_t positionCount = vertexCount / sizePerMesh;

    positionList.reserve(positionCount);
    normalList.reserve(positionCount);

    for (int i = 0; i < vertexCount; i += sizePerMesh) {
        int j = 0;

        if (hasPosition) {
            positionList.push_back(Vec3(vertexListArray[i + j + 0], vertexListArray[i + j + 1], vertexListArray[i + j + 2]));
            j += 3;
        }
        if (hasNormal) {
            normalList.push_back(Vec3(vertexListArray[i + j + 0], vertexListArray[i + j + 1], vertexListArray[i + j + 2]));
            j += 3;
        }
        if (hasTexture) {
            // 6, 7 はテクスチャ座標 (使わない)
            j += 2;
        }
    }

    // 面情報を取得
    // 全て三角形のはず https://github.com/cocos2d/cocos2d-x/blob/cocos2d-x-3.6/cocos/3d/CCObjLoader.cpp#L215-L228
    // TODO: 一旦全てのグループのメッシュデータを1つにまとめる

    // 領域確保
    int allIndexCount = 0;
    for (ssize_t i = 0, last = meshVertexData->getMeshIndexDataCount(); i < last; ++i) {
        allIndexCount += meshVertexData->getMeshIndexDataByIndex((int)i)->getIndexBuffer()->getIndexNumber();
    }
    // 全て三角形のはず
    CCASSERT(allIndexCount % 3 == 0, "invalid number of allIndexCount");
    triangleList.reserve(allIndexCount / 3);

    for (ssize_t i = 0, last = meshVertexData->getMeshIndexDataCount(); i < last; ++i) {
        MeshIndexData* meshIndexData = meshVertexData->getMeshIndexDataByIndex((int)i);
        const ExIndexBuffer* indexBuffer = (const ExIndexBuffer*)meshIndexData->getIndexBuffer();
        const vector<unsigned char>& rawIndexList = indexBuffer->getShadowCopy();
        // unsigned char -> unsigned short 変換 orz
        const unsigned short* indexListArray = reinterpret_cast<const unsigned short*>(rawIndexList.data());

        // 全て三角形のはず
        size_t indexCount = rawIndexList.size() / sizeof(unsigned short);
        CCASSERT(indexCount % 3 == 0, "invalid number of indexCount");

        for (size_t i = 0; i < indexCount; i += 3) {
            triangleList.push_back({ indexListArray[i + 0], indexListArray[i + 1], indexListArray[i + 2] });
        }
    }
}

void ExSprite3D::buildCollisionMesh()
{
    this->collisionMesh = CollisionMesh::create(positionList, triangleList);
    this->collisionMesh->retain();
}

bool ExSprite3D::getIntersection(int* triangleIndex, float *tOut, const Vec3& begin, const Vec3& vec) const
{
    CCASSERT(this->collisionMesh, "CollisionMesh is not initialized");
    return this->collisionMesh->getIntersection(triangleIndex, tOut, begin, vec, positionList, triangleList);
}

bool ExSprite3D::isIntersect(const cocos2d::Vec3 &begin, const cocos2d::Vec3 &vec) const
{
    // dummy
    int index;
    float t;

    return this->getIntersection(&index, &t, begin, vec);
}

bool ExSprite3D::collisionDetectionEnabled() const
{
    return _collisionDetectionEnabled;
}

void ExSprite3D::enableCollisionDetection(bool flag)
{
    _collisionDetectionEnabled = flag;
}