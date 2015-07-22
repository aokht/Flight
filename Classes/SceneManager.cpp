//
//  SceneManager.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include "SceneManager.h"

#include "LobbyScene.h"
#include "AirplaneSelectScene.h"
#include "StageSelectScene.h"
#include "GameSceneManager.h"


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

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::showLobbyScene()
{
    Scene* lobbyScene = LobbyScene::createScene();
    Director::getInstance()->pushScene(lobbyScene);
}

void SceneManager::showAirplaneSelectScene()
{
    Scene* airplaneSelectScene = AirplaneSelectScene::createScene();
    Director::getInstance()->pushScene(airplaneSelectScene);
}

void SceneManager::showStageSelectScene()
{
    Scene* stageSelectScene = StageSelectScene::createScene();
    Director::getInstance()->pushScene(stageSelectScene);
}

void SceneManager::showGameScene()
{
    GameSceneManager::getInstance()->showGameScene();
}