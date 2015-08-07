//
//  SceneData.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/25.
//
//

#ifndef Flight_SceneData_h
#define Flight_SceneData_h

#include <iostream>
#include "Sphere.h"

class SceneData
{
public:
    enum Mode {
        NONE = 0,
        SINGLE = 1,
        MULTI_MASTER = 2,
        MULTI_SLAVE  = 3,
        LAST,      // DO NOT USE
    };
    Mode mode;

    int stageId = 0;
    int airplaneId = 0;
    int targetAirplaneId = 0;

    SceneData(Mode mode = NONE, int stageId = 0, int airplaneId = 0, int targetAirplaneId = 0) :
        mode(mode),
        stageId(stageId),
        airplaneId(airplaneId),
        targetAirplaneId(targetAirplaneId)
    {
    }
};

struct AchievedSphereInfo {
    int batchId;
    int sphereId;
    Sphere::Type color;  // 冗長だけどよく使うので速度重視
    int achiever;
};

struct NetworkPacket
{
    enum Type {
        NONE = 0,  // DO NOT USE
        SELECTSCENE = 1,
        AIRPLANE_INFO = 2,
        GAME_SCORE = 3,
        LAST,      // DO NOT USE
    };
    const Type type;
    int peerId; // TODO: 使っていない

    NetworkPacket(Type type) :
        type(type),
        peerId(1)
    {
    };
};

struct SelectSceneNetworkPacket : public NetworkPacket
{
    const SceneData::Mode mode;
    const int stageId;
    const int airplaneId;

    SelectSceneNetworkPacket(SceneData::Mode mode, int stageId, int airplaneId) :
        NetworkPacket(Type::SELECTSCENE),
        mode(mode),
        stageId(stageId),
        airplaneId(airplaneId)
    {
    };
};

static const int MAX_SPHEREINFO_LENGTH = 10;
struct AirplaneInfoNetworkPacket : public NetworkPacket
{
    struct position_t {
        const float x;
        const float y;
        const float z;
    };
    struct rotation_t {
        const float x;
        const float y;
        const float z;
    };

    const position_t position;
    const rotation_t rotation;
    const int achievedSphereInfoCount;
    AchievedSphereInfo achievedSphereInfoList[MAX_SPHEREINFO_LENGTH];

    AirplaneInfoNetworkPacket(const cocos2d::Vec3& position, const cocos2d::Vec3& rotation, const std::vector<AchievedSphereInfo>& achievedSphereInfoList) :
        NetworkPacket(Type::AIRPLANE_INFO),
        position({ position.x, position.y, position.z }),
        rotation({ rotation.x, rotation.y, rotation.z }),
        achievedSphereInfoCount(std::min((int)achievedSphereInfoList.size(), MAX_SPHEREINFO_LENGTH))
    {
        for (int i = 0; i < achievedSphereInfoCount; ++i) {
            this->achievedSphereInfoList[i] = achievedSphereInfoList[i];
        }
    }
};

struct GameScoreNetworkPacket : public NetworkPacket
{
    int score;
    int blueCount;
    int yellowCount;
    int redCount;
    bool isTimeUp;
    bool isCollided;
    bool isCompleted;

    GameScoreNetworkPacket(
        int score,
        int blueCount,
        int yellowCount,
        int redCount,
        bool isTimeUp,
        bool isCollided,
        bool isCompleted
    ) :
        NetworkPacket(Type::GAME_SCORE),
        score(score),
        blueCount(blueCount),
        yellowCount(yellowCount),
        redCount(redCount),
        isTimeUp(isTimeUp),
        isCollided(isCollided),
        isCompleted(isCompleted)
    {
    }
};

#endif
