//
//  SceneManager.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include <iostream>
#include "cocostudio/CocoStudio.h"
#include "SceneManager.h"
#include "LobbyScene.h"
#include "SelectScene.h"
#include "LoadingScene.h"
#include "CreditsScene.h"
#include "HighScoreScene.h"
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
    networking(false),
    sceneCount(0)
{
    networkingWrapper->setServiceName("Flight");
    networkingWrapper->setDelegate(this);
}

SceneManager::~SceneManager()
{
}

Node* SceneManager::createCSNode(const string& path)
{
    Node* rootNode = CSLoader::createNode(path);

    rootNode->setContentSize(Director::getInstance()->getVisibleSize());
    ui::Helper::doLayout(rootNode);

    return rootNode;
}

void SceneManager::showLobbyScene()
{
    stopGameScene();
    networkingWrapper->disconnect();
    resetScene();
    Scene* lobbyScene = LobbyScene::createScene();
    Director::getInstance()->pushScene(lobbyScene);
    this->sceneCount++;
}

void SceneManager::showSelectScene()
{
    stopGameScene();
    Scene* selectScene = SelectScene::createScene();
    Director::getInstance()->pushScene(selectScene);
    this->sceneCount++;
}

void SceneManager::showGameScene()
{
    gameSceneManager = GameSceneManager::getInstance();
    gameSceneManager->setSceneData(sceneData);
    gameSceneManager->showGameScene();
    this->sceneCount++;
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
    this->sceneCount++;
}

void SceneManager::showCreditsScene()
{
    Scene* creditsScene = CreditsScene::createScene();
    Director::getInstance()->pushScene(creditsScene);
    this->sceneCount++;
}

void SceneManager::showHighScoreScene(int stageId)
{
    Scene* highScoreScene = HighScoreScene::createSceneWithStageId(stageId);
    Director::getInstance()->pushScene(highScoreScene);
    this->sceneCount++;
}

void SceneManager::backScene()
{
    Director::getInstance()->popScene();
    this->sceneCount--;
}

void SceneManager::resetScene()
{
    while (this->sceneCount--) {
        Director::getInstance()->popScene();
    }
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

void SceneManager::stopAdvertisingAvailability()
{
    networkingWrapper->stopAdvertisingAvailability();
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

void SceneManager::sendData(const void *data, int length, SendDataMode mode)
{
    networkingWrapper->sendData(data, length, mode);
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
    static chrono::high_resolution_clock::time_point lastSent = chrono::high_resolution_clock::now();

    // スフィアを取っていないなら 100ms に 1回しか送らない  TODO: 敵の動きもシミュレートしないとカクカクする
    if (
        packet.achievedSphereInfoCount == 0 &&
        chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - lastSent).count() < 100
    ) {
        return;
    }
    lastSent = chrono::high_resolution_clock::now();

    this->sendData(&packet, sizeof(AirplaneInfoNetworkPacket), SendDataMode::Unreliable);
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