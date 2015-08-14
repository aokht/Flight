//
//  MiniMap.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/10.
//
//

#include "MiniMap.h"
#include "Sphere.h"
#include "Global.h"

using namespace std;
using namespace cocos2d;

typedef Sprite3DBatchNode::NodeStatus NodeStatus;

#pragma mark -
#pragma mark MiniMap

void MiniMap::onEnter()
{
    Node::onEnter();

    Sprite* background = this->getChildByName<Sprite*>("MiniMapBackground");
    CCASSERT(background, "MiniMapBackground in GameScene is not found");
    this->setContentSize(background->getContentSize());

    this->backgroundNode = background->getChildByName<Node*>("BackgroundNode");
    CCASSERT(backgroundNode, "BackgroundNode in GameScene is not found");
    this->backgroundNode->setContentSize(background->getContentSize());
    this->backgroundNode->setScale(0.7);
}

void MiniMap::setDivisor(int divisor)
{
    this->divisor = divisor;
}

void MiniMap::addSphereBatch(Sphere* sphereBatch)
{
    this->sphereBatchList.push_back(sphereBatch);

    vector<Sprite*> spriteList;
    const vector<NodeStatus>& nodeStatusList = sphereBatch->getNodeStatusList();
    for (size_t i = 0, last = nodeStatusList.size(); i < last; ++i) {
        // divisor個にひとつ、マップ上に表示する
        if (i % divisor != 0) {
            continue;
        }

        Sprite* sphereSprite = this->createMiniMapSprite(sphereBatch->getType());
        sphereSprite->setPosition(this->convertCoordinateToMiniMap(nodeStatusList[i].position));

        this->backgroundNode->addChild(sphereSprite);
        spriteList.push_back(sphereSprite);
    }

    this->sphereSpriteList.push_back(spriteList);
    this->sphereSpriteVisibilityList.push_back(vector<bool>(spriteList.size(), true));
}

void MiniMap::step(float dt, const Vec3& airplanePosition, const Vec3& airplaneRotation)
{
    static const float mapSize =this->getContentSize().width; // 正方形想定
    static const float mapSizeHalf = mapSize * 0.5f;

    // 回転と移動(アンカーポイントを飛行機の位置にし、中心に移動して回転)
    this->backgroundNode->setAnchorPoint(this->convertCoordinateToMiniMap(airplanePosition) / mapSize);
    this->backgroundNode->setPosition(Vec2(mapSizeHalf, mapSizeHalf));
    this->backgroundNode->setRotation(airplaneRotation.y);

    // 現在のアンカーポイントの座標
    const Vec2& anchorPointInPoints = this->backgroundNode->getAnchorPointInPoints();

    for (int i = 0, sphereBatchSize = (int)sphereBatchList.size(); i < sphereBatchSize; ++i) {
        Sphere* sphereBatch = sphereBatchList[i];

        const vector<Sprite3DBatchNode::NodeStatus>& nodeStatusList = sphereBatch->getNodeStatusList();
        for (int j = 0, divide = divisor - 1, nodeListSize = (int)nodeStatusList.size(); j < nodeListSize; ++j, --divide) {
            int sphereBatchIndex = i;
            int sphereIndex = j / divisor;

            // 既に消えているならスキップ
            if (j % divisor == 0 && sphereSpriteVisibilityList[sphereBatchIndex][sphereIndex] == false) {
                j += divisor - 1;
                divide = divisor;
                continue;
            }

            // マップの範囲から外れたスフィアを反対側に移動
            if (j % divisor == 0) {
                Sprite* sphere = sphereSpriteList[sphereBatchIndex][sphereIndex];
                sphere->setPosition(this->convertCoordinateByCurrentAnchorPoint(sphere->getPosition()));
            }

            // divisor個のうち、ひとつでも見えているものがあればスキップ
            if (nodeStatusList[j].isVisible) {
                j += divisor - (j % divisor) - 1;
                divide = divisor;
                continue;
            }

            // divisor個全てが見えなければ消す
            if (divide == 0) {
                // sphereBatch のインデックスと、sphere のインデックス
                sphereSpriteList[sphereBatchIndex][sphereIndex]->setVisible(false);
                sphereSpriteVisibilityList[sphereBatchIndex][sphereIndex] = false;
                // リセット
                divide = divisor;
            }
        }
    }
}

void MiniMap::setOpponentPlayerStatus(const Vec3& airplanePosition, const Vec3& airplaneRotation)
{
    if (! this->opponentPlayerNode) {
        this->opponentPlayerNode = Sprite::create("ui/miniMapEnemy.png");
        if (! opponentPlayerNode) {
            return;
        }
        this->backgroundNode->addChild(opponentPlayerNode);
    }

    Vec2 position = this->convertCoordinateByCurrentAnchorPoint(this->convertCoordinateToMiniMap(airplanePosition));
    this->opponentPlayerNode->setPosition(position);
    this->opponentPlayerNode->setRotation(-airplaneRotation.y);
}

Vec2 MiniMap::convertCoordinateByCurrentAnchorPoint(cocos2d::Vec2 position)
{
    // 現在のアンカーポイントに基いて、はみ出ている部分を反対側に移動する変換を行う

    static const float mapSize =this->getContentSize().width; // 正方形想定
    static const float mapSizeHalf = mapSize * 0.5f;
    const Vec2& anchorPointInPoints = this->backgroundNode->getAnchorPointInPoints();

    position -= anchorPointInPoints;

    if (abs(position.x) > mapSizeHalf) {
        position.x += mapSize * sign(-position.x);
    }
    if (abs(position.y) > mapSizeHalf) {
        position.y += mapSize * sign(-position.y);
    }

    position += anchorPointInPoints;

    return position;
}

Sprite* MiniMap::createMiniMapSprite(Sphere::Type type)
{
    static const string miniMapSpriteFile = "ui/miniMapSphere.png";

    Rect rect;
    switch (type) {
        case Sphere::Type::BLUE:
            rect = Rect( 0.f, 0.f, 32.f, 32.f);
            break;
        case Sphere::Type::YELLOW:
            rect = Rect(32.f, 0.f, 32.f, 32.f);
            break;
        case Sphere::Type::RED:
            rect = Rect(64.f, 0.f, 32.f, 32.f);
            break;
        default:
            CCASSERT(false, "Invalid sphere type");
    }

    Sprite* sprite = Sprite::create(miniMapSpriteFile, rect);
    sprite->setScale(0.25f);
    return sprite;
}

Vec2 MiniMap::convertCoordinateToMiniMap(const cocos2d::Vec3& position)
{
    // 正規化(Y座標は使わない)
    Vec3 positionNormal = position / FIELD_LENGTH;
    positionNormal.x = -positionNormal.x;
    positionNormal.z = positionNormal.z;

    return Vec2(
        this->getContentSize().width * positionNormal.x,
        this->getContentSize().height * positionNormal.z
    );
}

MiniMap::MiniMap() :
    divisor(1),
    opponentPlayerNode(nullptr)
{
}

MiniMap::~MiniMap()
{
}

#pragma mark -
#pragma mark MiniMapReader

MiniMapReader* MiniMapReader::instance = nullptr;

MiniMapReader* MiniMapReader::getInstance()
{
    if (! instance) {
        instance = new MiniMapReader();
    }

    return instance;
}

void MiniMapReader::purge()
{
    CC_SAFE_DELETE(instance);
}

Node* MiniMapReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
    MiniMap* node = MiniMap::create();
    setPropsWithFlatBuffers(node, nodeOptions);
    return node;
}