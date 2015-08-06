//
//  SceneManager.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include <iostream>
#include "SceneManager.h"
#include "LobbyScene.h"
#include "SelectScene.h"
#include "LoadingScene.h"
#include "GameSceneManager.h"
#include "Global.h"

using namespace std;
using namespace cocos2d;

SceneManager* SceneManager::getInstance()
{
    static SceneManager* instance = nullptr;

    if (! instance) {
        instance = new SceneManager();
    }

    return instance;
}

SceneManager::SceneManager() :
    networkingWrapper(new NetworkingWrapper()),
    gameSceneManager(nullptr),
    networking(false)
{
    networkingWrapper->setServiceName("Flight");
    networkingWrapper->setDelegate(this);
}

SceneManager::~SceneManager()
{
}

void SceneManager::showLobbyScene()
{
    stopGameScene();
    networkingWrapper->disconnect();
    Scene* lobbyScene = LobbyScene::createScene();
    Director::getInstance()->pushScene(lobbyScene);
}

void SceneManager::showSelectScene()
{
    stopGameScene();
    Scene* selectScene = SelectScene::createScene();
    Director::getInstance()->pushScene(selectScene);
}

void SceneManager::showGameScene()
{
    gameSceneManager = GameSceneManager::getInstance();
    gameSceneManager->setSceneData(sceneData);
    gameSceneManager->showGameScene();
}

void SceneManager::stopGameScene()
{
    if (gameSceneManager) {
        gameSceneManager->clearSceneData();
        gameSceneManager = nullptr;
    }
}

void SceneManager::showLoadingScene(const std::function<void ()>& callback, const std::string& label)
{
    Scene* loadingScene = LoadingScene::createScene(callback, label);
    Director::getInstance()->pushScene(loadingScene);
}

bool SceneManager::isInGameScene() const
{
    return gameSceneManager != nullptr;
}

void SceneManager::initSceneData()
{
    this->sceneData = {};
}

void SceneManager::setSceneData(SceneData sceneData)
{
    this->sceneData = sceneData;
}

SceneData SceneManager::getSceneData() const
{
    return sceneData;
}

SceneData::Mode SceneManager::getCurrentMode() const
{
    return sceneData.mode;
}

bool SceneManager::isSinglePlay() const
{
    return getCurrentMode() == SceneData::Mode::SINGLE;
}

bool SceneManager::isMultiplayMaster() const
{
    return getCurrentMode() == SceneData::Mode::MULTI_MASTER;
}

bool SceneManager::isMultiplaySlave() const
{
    return getCurrentMode() == SceneData::Mode::MULTI_SLAVE;
}

void SceneManager::setSelectSceneData(int stageId, int airplaneId)
{
    SceneData sceneData = getSceneData();
    if (isSinglePlay()) {
        sceneData.stageId = stageId;
        sceneData.airplaneId = airplaneId;
        this->setSceneData(sceneData);
    }
    else if (isMultiplayMaster()) {
        sceneData.stageId = stageId;
        sceneData.airplaneId = airplaneId;
        this->setSceneData(sceneData);
    }
    else if (isMultiplaySlave()) {
        sceneData.airplaneId = airplaneId;
        this->setSceneData(sceneData);
    }
}

#pragma mark -
#pragma mark Networking

void SceneManager::showPeerList()
{
    networkingWrapper->showPeerList();
}

void SceneManager::startAdvertisingAvailability()
{
    networkingWrapper->startAdvertisingAvailability();
}

void SceneManager::receivedData(const void* data, unsigned long length)
{
    const NetworkPacket* packet = static_cast<const NetworkPacket*>(data);
    switch (packet->type) {
        case NetworkPacket::Type::SELECTSCENE:
        {
            if (length != sizeof(SelectSceneNetworkPacket)) {
                return;
            }
            const SelectSceneNetworkPacket* p = static_cast<const SelectSceneNetworkPacket*>(packet);
            this->receivedData(*p);
        }
            break;

        case NetworkPacket::Type::AIRPLANE_INFO:
        {
            if (length != sizeof(AirplaneInfoNetworkPacket)) {
                return;
            }
            const AirplaneInfoNetworkPacket* p = static_cast<const AirplaneInfoNetworkPacket*>(data);
            this->receivedData(*p);
        }
            break;

        case NetworkPacket::Type::GAME_SCORE:
        {
            if (length != sizeof(GameScoreNetworkPacket)) {
                return;
            }
            const GameScoreNetworkPacket* p = static_cast<const GameScoreNetworkPacket*>(data);
            this->receivedData(*p);
        }
            break;

        case NetworkPacket::Type::NONE:
        case NetworkPacket::Type::LAST:
        default:
            // ignore
            break;
    }
}

void SceneManager::sendData(const void *data, int length)
{
    networkingWrapper->sendData(data, length);
}

void SceneManager::stateChanged(ConnectionState state)
{
    switch (state) {
        case ConnectionState::NOT_CONNECTED:
            CCLOG("Not connected");
            if (networking) {
                MessageBox("Disconnected...", "Error");
                SceneManager::showLobbyScene();
            }
            break;
        case ConnectionState::CONNECTING:
            CCLOG("Connecting...");
            break;
        case ConnectionState::CONNECTED:
            CCLOG("Connected!");
            if (! isInGameScene()) {
                networkingWrapper->stopAdvertisingAvailability();
                this->setPeerState();
                this->setNetworking(true);
                this->showSelectScene();
            }
            break;
    }
}

void SceneManager::setPeerState()
{
    this->setSceneData({
        isMasterPeer() ? SceneData::Mode::MULTI_MASTER : SceneData::Mode::MULTI_SLAVE
    });
}

bool SceneManager::isMasterPeer() const
{
    vector<string> peerList = networkingWrapper->getPeerList();
    string deviceName = networkingWrapper->getDeviceName();
    peerList.push_back(deviceName);

    sort(peerList.begin(), peerList.end());

    return deviceName.compare(*peerList.begin());
}

bool SceneManager::isMultiplayReady() const
{
    CCASSERT(!isSinglePlay(), "Invalid game mode");

    SceneData data = getSceneData();
    cout << data << endl;
    return data.stageId && data.airplaneId && data.targetAirplaneId;
}

void SceneManager::sendSelectSceneDataToPeer()
{
    SceneData data = getSceneData();
    SelectSceneNetworkPacket packet(
        data.mode,
        data.stageId,
        data.airplaneId
    );
    this->sendData(&packet, sizeof(SelectSceneNetworkPacket));
}

void SceneManager::sendGameSceneAirplaneInfoToPeer(const AirplaneInfoNetworkPacket& packet)
{
    this->sendData(&packet, sizeof(AirplaneInfoNetworkPacket));
}

void SceneManager::sendGameSceneScoreToPeer(const GameScoreNetworkPacket& packet)
{
    this->sendData(&packet, sizeof(GameScoreNetworkPacket));
}

void SceneManager::receivedData(const SelectSceneNetworkPacket&data)
{
    SceneData sceneData = getSceneData();

    // validation
    if (! (SceneData::Mode::NONE < data.mode && data.mode < SceneData::Mode::LAST)) {
        return;
    }
    if (! (data.airplaneId > 0)) {
        return;
    }
    if (data.mode == SceneData::Mode::MULTI_MASTER) {
        if (! (data.stageId > 0)) {
            return;
        }
    }
    if (getCurrentMode() == data.mode) {
        // 両者とも master / 両者とも slave
        return;
    }

    switch (data.mode) {
        // 相手が master
        case SceneData::Mode::MULTI_MASTER:
            sceneData.stageId = data.stageId;
            sceneData.targetAirplaneId = data.airplaneId;
            break;

        // 相手が slave
        case SceneData::Mode::MULTI_SLAVE:
            sceneData.targetAirplaneId = data.airplaneId;
            break;

        // ignore
        default:
            break;
    }

    // isMultiplayReady が true になるようにする
    this->setSceneData(sceneData);
    if (isMultiplayReady()) {
        this->showGameScene();
    }
}

void SceneManager::receivedData(const AirplaneInfoNetworkPacket& data)
{
    if (gameSceneManager) {
        gameSceneManager->receivedData(data);
    }
}

void SceneManager::receivedData(const GameScoreNetworkPacket& data)
{
    if (gameSceneManager) {
        gameSceneManager->receivedData(data);
    }
}

void SceneManager::setNetworking(bool networking)
{
    this->networking = networking;
}