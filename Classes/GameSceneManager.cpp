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
#include "ResultSceneMulti.h"
#include "SceneManager.h"

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

GameSceneManager::GameSceneManager() :
    gameScene(nullptr),
    sceneCount(0)
{
}

GameSceneManager::~GameSceneManager()
{
}

void GameSceneManager::showGameScene()
{
    this->resetScene();

    this->gameScene = GameScene::create();

    auto scene = Scene::create();
    scene->addChild(gameScene);
    Director::getInstance()->pushScene(scene);

    this->sceneCount++;
}

void GameSceneManager::showResultScene(const GameScene::GameScore& score)
{
    // 結果を表示する時点でコネクションを切断フラグを立てる
    SceneManager::getInstance()->setNetworking(false);

    Scene* resultScene = isSinglePlay() ? ResultScene::createScene(score) : ResultSceneMulti::createScene(score);

    Director::getInstance()->pushScene(resultScene);
    this->sceneCount++;
}

void GameSceneManager::resetScene()
{
    while (this->sceneCount) {
        Director::getInstance()->popScene();
        this->sceneCount--;
    }
    this->gameScene = nullptr;
}

void GameSceneManager::setSceneData(SceneData sceneData)
{
    this->sceneData = sceneData;
}

SceneData GameSceneManager::getSceneData() const
{
    return sceneData;
}

void GameSceneManager::clearSceneData()
{
    this->sceneData = {};
}

void GameSceneManager::receivedData(const AirplaneInfoNetworkPacket& data)
{
    if (gameScene) {
        gameScene->receivedData(data);
    }
}

void GameSceneManager::receivedData(const GameScoreNetworkPacket& data)
{
    if (gameScene) {
        gameScene->receivedData(data);
    }
}