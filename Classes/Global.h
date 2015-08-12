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
    "edm_01_loop.wav",
    "edm_02_loop.wav",
    "shooting_05_loop.wav",
    "Lazy Day.wav",
    "Coming Home.wav",
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
    "ALARM Submarine Fast (loop stereo).wav",
    "ALARM Submarine Slow (loop stereo).wav",
    "EXPLOSION Medium Smooth with Faint Echo Rumble (stereo).wav",
    "FLYBY Sci-Fi 01 (mono).wav",
    "PROPELLER ENGINE Loop 01 (loop mono).wav",
    "PROPELLER ENGINE Loop 02 (loop mono).wav",
    "THRUSTER Afterburner Power Distorted (loop mono).wav",
    "UI Click Distinct (mono).wav",
    "UI SCI-FI Tone Bright Wet 09 (stereo).wav",
    "UI SCI-FI Confirm Dry (stereo).wav",
    "FLYBY Sci-Fi 01 (mono)_short.wav"
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
    TAP_IMPORTANT = 8,
    GET_SPHERE = 9,
    GAMEEND_FLYBY = 10,
    SE_LAST,
};

#endif /* defined(__Flight__Global__) */