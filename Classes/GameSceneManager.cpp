//
//  GameSceneManager.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#include "cocos2d.h"
#include "GameSceneManager.h"
#include "GameScene.h"
#include "ResultScene.h"
#include "ParameterScene.h"

using namespace std;
using namespace cocos2d;

GameSceneManager* GameSceneManager::getInstance()
{
    static GameSceneManager* instance = nullptr;

    if (! instance) {
        instance = new GameSceneManager();
    }

    return instance;
}

GameSceneManager::GameSceneManager()
{
    this->sceneCount = 0;
}

GameSceneManager::~GameSceneManager()
{
}

void GameSceneManager::showGameScene()
{
    this->resetScene();
    Scene* gameScene = GameScene::createScene();
    Director::getInstance()->pushScene(gameScene);
    this->sceneCount++;
}

void GameSceneManager::showResultScene(const GameScene::GameScore& score)
{
    Scene* resultScene = ResultScene::createScene(score);
    Director::getInstance()->pushScene(resultScene);
    this->sceneCount++;
}

void GameSceneManager::showParameterScene(const GameScene::GameScore& score)
{
    Scene* parameterScene = ParameterScene::createScene(score);
    Director::getInstance()->pushScene(parameterScene);
    this->sceneCount++;
}

void GameSceneManager::resetScene()
{
    while (this->sceneCount) {
        Director::getInstance()->popScene();
        this->sceneCount--;
    }
}

void GameSceneManager::setSceneData(SceneData sceneData)
{
    this->sceneData = sceneData;
}

SceneData GameSceneManager::getSceneData() const
{
    return sceneData;
}