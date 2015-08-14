//
//  MiniMap.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/10.
//
//

#ifndef __Flight__MiniMap__
#define __Flight__MiniMap__

#include "cocos2d.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "Sphere.h"

class MiniMap : public cocos2d::Node
{
public:
    CREATE_FUNC(MiniMap);

    void setDivisor(int divisor);
    void addSphereBatch(Sphere* sphereBatch);
    void step(float dt, const cocos2d::Vec3& airplanePosition, const cocos2d::Vec3& airplaneRotation);
    void setOpponentPlayerStatus(const cocos2d::Vec3& airplanePosition, const cocos2d::Vec3& airplaneRotation);

protected:
    cocos2d::Node* backgroundNode;
    cocos2d::Sprite* opponentPlayerNode;

    int divisor;  // マップ上のスフィアと、実際のスフィアの数の比
    std::vector<Sphere*> sphereBatchList;
    std::vector<std::vector<cocos2d::Sprite*>> sphereSpriteList;
    std::vector<std::vector<bool>> sphereSpriteVisibilityList;

    void onEnter() override;

    static cocos2d::Sprite* createMiniMapSprite(Sphere::Type type);
    cocos2d::Vec2 convertCoordinateToMiniMap(const cocos2d::Vec3& position);
    cocos2d::Vec2 convertCoordinateByCurrentAnchorPoint(cocos2d::Vec2 position);

private:
    MiniMap();
    ~MiniMap();
};

class MiniMapReader : public cocostudio::NodeReader
{
public:
    static MiniMapReader* getInstance();
    static void purge();
    cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);

protected:
    static MiniMapReader* instance;
};

#endif /* defined(__Flight__MiniMap__) */
