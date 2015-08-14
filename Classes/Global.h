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

float sign(float num);  // 符号を返す

const float EPSILON = 0.00000011920928955078125f; //floatの限界精度(1/2^23)

#define FIELD_LENGTH 10000.f  // フィールドの1辺の長さ
#define FIELD_HEIGHT  2500.f  // フィールド高さ

#define PLAY_SECONDS 120.f       // 1プレイの長さ
#define PLAY_SECONDS_MULTI 60.f  // 1プレイの長さ(マルチ用)

#define DUMMY_PEER_ID 1  // 複数人対戦したいけど無理なのでとりあえず


static const char* BGM_LIST[] = {
    "stage1.dat",
    "stage2.dat",
    "stage3.dat",
    "SubScreen.dat",
    "MainTitle.dat",
};
enum BGM_INDEX {
    BGM_STAGE1 = 0,
    BGM_STAGE2 = 1,
    BGM_STAGE3 = 2,
    BGM_SUBTITLE = 3,
    BGM_MAINTITLE = 4,
    BGM_LAST,
};

static const char* SE_LIST[] = {
    "CountDown1.dat",
    "CountDown2.dat",
    "Explosion.dat",
    "FlyBy.dat",
    "Airplane1.dat",
    "Airplane2.dat",
    "Airplane3.dat",
    "Tap.dat",
    "",
    "GetCoins.dat",
    "FlyBy_short.dat"
};
enum SE_INDEX {
    COUNTDOWN_1 = 0,
    COUNTDOWN_2 = 1,
    EXPLOSION = 2,
    TITLE_FLYBY = 3,
    AIRPLANE_1 = 4,
    AIRPLANE_2 = 5,
    AIRPLANE_3 = 6,
    TAP_NORMAL = 7,
    TAP_IMPORTANT = 7,  // 音見つからないので一旦タップ音全部同じにする
    GET_SPHERE = 9,
    GAMEEND_FLYBY = 10,
    SE_LAST,
};

#endif /* defined(__Flight__Global__) */