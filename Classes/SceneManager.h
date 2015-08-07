//
//  SceneManager.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#ifndef __Flight__SceneManager__
#define __Flight__SceneManager__

#include "cocos2d.h"
#include "SceneData.h"
#include "NetworkingWrapper.h"

class GameSceneManager;

class SceneManager : public NetworkingDelegate
{
public:
    friend GameSceneManager;

    static SceneManager* getInstance();

    void showLobbyScene();
    void showSelectScene();
    void showGameScene();
    void showLoadingScene(const std::function<void()>& callback, const std::string& label);
    bool isInGameScene() const;

    void initSceneData();
    void setSceneData(SceneData sceneData);
    SceneData getSceneData() const;
    SceneData::Mode getCurrentMode() const;

    bool isSinglePlay() const;
    bool isMultiplayMaster() const;
    bool isMultiplaySlave() const;

    void setSelectSceneData(int stageId, int airplaneId);

    // NetworkingDelegate
    void showPeerList();
    void startAdvertisingAvailability();
    void stopAdvertisingAvailability();
    void receivedData(const void* data, unsigned long length) override;
    void stateChanged(ConnectionState state) override;
    void setPeerState();
    bool isMasterPeer() const;
    bool isMultiplayReady() const;

    void sendSelectSceneDataToPeer();
    void sendGameSceneAirplaneInfoToPeer(const AirplaneInfoNetworkPacket& packet);
    void sendGameSceneScoreToPeer(const GameScoreNetworkPacket& packet);

protected:
    SceneManager();
    ~SceneManager();

    void stopGameScene();

    GameSceneManager* gameSceneManager;
    SceneData sceneData;

    NetworkingWrapper* networkingWrapper;

    bool networking;
    void setNetworking(bool networking);
    void sendData(const void* data, int length, SendDataMode mode = SendDataMode::Reliable);
    void receivedData(const SelectSceneNetworkPacket& data);
    void receivedData(const AirplaneInfoNetworkPacket& data);
    void receivedData(const GameScoreNetworkPacket& data);
};


#endif /* defined(__Flight__SceneManager__) */
