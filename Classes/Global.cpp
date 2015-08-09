//
//  Global.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/12.
//
//

#include "Global.h"
#include "SceneData.h"

using namespace std;
using namespace cocos2d;

ostream& operator<<(ostream& stream, const Vec2& vec)
{
    stream << "(" << vec.x << ", " << vec.y << ")";
    return stream;
}

ostream& operator<<(ostream& stream, const Vec3& vec)
{
    stream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return stream;
}

ostream& operator<<(ostream& stream, const Rect& rec)
{
    stream << rec.origin << ", " << rec.size;
    return stream;
}

ostream& operator<<(ostream& stream, const AABB& aabb)
{
    stream << aabb._min << ", " << aabb._max;
    return stream;
}

ostream& operator<<(ostream& stream, const SceneData& data)
{
    stream << "SceneData {" <<
        "mode: " << data.mode << ", " <<
        "stageId: " << data.stageId << ", " <<
        "airplaneId: " << data.airplaneId << ", " <<
        "targetAirplaneId: " << data.targetAirplaneId <<
        " }";
    return stream;
}

ostream& operator<<(ostream& stream, const GameScene::GameScore& score)
{
    stream << "score: { coinsCount: " << score.sphereList.size() << ", " << "elapsedTime: " << score.elapsedTime << "}";
    return stream;
}

ostream& operator<<(ostream& stream, const NetworkPacket::Type& type)
{
    switch (type) {
        case NetworkPacket::Type::NONE:
            stream << "None";
            break;
        case NetworkPacket::Type::SELECTSCENE:
            stream << "SelectScene";
            break;
        case NetworkPacket::Type::AIRPLANE_INFO:
            stream << "AirplaneInfo";
            break;
        case NetworkPacket::Type::LAST:
        default:
            stream << "Unknown";
            break;
    }
    return stream;
}

ostream& operator<<(ostream& stream, const SceneData::Mode& mode)
{
    switch (mode) {
        case SceneData::Mode::NONE:
            stream << "None";
            break;
        case SceneData::Mode::SINGLE:
            stream << "Single";
            break;
        case SceneData::Mode::MULTI_MASTER:
            stream << "Multi(Master)";
            break;
        case SceneData::Mode::MULTI_SLAVE:
            stream << "Multi(Slave)";
            break;
        case SceneData::Mode::LAST:
        default:
            stream << "Unknown";
            break;
    }
    return stream;
}

ostream& operator<<(ostream& stream, const NetworkPacket& packet)
{
    stream << "NetworkPacket { type = " << packet.type << " }";
    return stream;
}

ostream& operator<<(ostream& stream, const SelectSceneNetworkPacket& packet)
{
    stream << "SelectSceneNetworkPacket { " <<
        "mode: " << packet.mode << ", " <<
        "stageId: " << packet.stageId << ", " <<
        "airplaneId: " << packet.airplaneId <<
        " }";
    return stream;
}

ostream& operator<<(ostream& stream, const AchievedSphereInfo& sphereInfo)
{
    stream << "AchievedSphereInfo { " <<
        "batchId: " << sphereInfo.batchId << ", " <<
        "sphereId: " << sphereInfo.sphereId << ", " <<
        "achiever: " << sphereInfo.achiever <<
        " }";
    return stream;
}

ostream& operator<<(ostream& stream, const AirplaneInfoNetworkPacket& packet)
{
    stream << "AirplaneInfoNetworkPacket { " <<
        "peerId: " << packet.peerId << ", " <<
        "position: (" << packet.position.x << ", " << packet.position.y << ", " << packet.position.z << "), " <<
        "rotation: (" << packet.rotation.x << ", " << packet.rotation.y << ", " << packet.rotation.z << "), ";
    stream << "AchievedSphereInfo(" << packet.achievedSphereInfoCount << ") { ";
    for (int i = 0; i < packet.achievedSphereInfoCount; ++i) {
        stream << packet.achievedSphereInfoList[i] << ", ";
    }
    stream << "}";
    stream << " }";
    return stream;
}

float sign(float num)
{
    return num < 0.f ? -1.f : 1.f;
}