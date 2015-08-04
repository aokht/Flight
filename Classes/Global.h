//
//  Global.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/12.
//
//

#ifndef __Flight__Global__
#define __Flight__Global__

#include <iostream>
#include "cocos2d.h"
#include "SceneData.h"
#include "GameScene.h"

std::ostream& operator<<(std::ostream& stream, const cocos2d::Vec2& vec);
std::ostream& operator<<(std::ostream& stream, const cocos2d::Vec3& vec);
std::ostream& operator<<(std::ostream& stream, const cocos2d::Rect& rec);
std::ostream& operator<<(std::ostream& stream, const cocos2d::AABB& aabb);

std::ostream& operator<<(std::ostream& stream, const SceneData& data);
std::ostream& operator<<(std::ostream& stream, const GameScene::GameScore& score);
std::ostream& operator<<(std::ostream& stream, const NetworkPacket::Type& type);
std::ostream& operator<<(std::ostream& stream, const SceneData::Mode& mode);
std::ostream& operator<<(std::ostream& stream, const NetworkPacket& packet);
std::ostream& operator<<(std::ostream& stream, const SelectSceneNetworkPacket& packet);
std::ostream& operator<<(std::ostream& stream, const AirplaneInfoNetworkPacket& packet);
std::ostream& operator<<(std::ostream& stream, const AchievedSphereInfo& sphereInfo);


const float EPSILON = 0.00000011920928955078125f; //floatの限界精度(1/2^23)

#define FIELD_LENGTH 10000.f  // フィールドの1辺の長さ
#define FIELD_HEIGHT  2500.f  // フィールド高さ

#define PLAY_SECONDS 120.f  // 1プレイの長さ

#define DUMMY_PEER_ID 1  // 複数人対戦したいけど無理なのでとりあえず

#endif /* defined(__Flight__Global__) */