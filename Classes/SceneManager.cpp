//
//  SceneManager.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include "SceneManager.h"

#include "LobbyScene.h"
#include "SelectScene.h"
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

void SceneManager::showSelectScene()
{
    Scene* selectScene = SelectScene::createScene();
    Director::getInstance()->pushScene(selectScene);
}

void SceneManager::showGameScene()
{
    GameSceneManager* gameSceneManager = GameSceneManager::getInstance();
    gameSceneManager->setSceneData(sceneData);
    gameSceneManager->showGameScene();
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