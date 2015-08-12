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
#include "LoadingScene.h"
#include "Global.h"
#include "SimpleAudioEngine.h"

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

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
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();

    Director::getInstance()->pushScene(LoadingScene::createScene(
        [this](){
            this->gameScene = GameScene::create();

            auto scene = Scene::create();
            scene->addChild(gameScene);
            Director::getInstance()->pushScene(scene);
            this->sceneCount++;
        },
        "Loading..."
    ));
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

map<Sphere::Type, int> GameSceneManager::calculateScore(const vector<AchievedSphereInfo>& achievedSphereInfoList)
{
    static const float rateMap[] = {
        1.00f,  // BLUE
        1.25f,  // YELLOW
        1.50f   // RED
    };

    map<Sphere::Type, int> sphereCount;
    sphereCount[Sphere::Type::BLUE] = 0;
    sphereCount[Sphere::Type::YELLOW] = 0;
    sphereCount[Sphere::Type::RED] = 0;
    for (const AchievedSphereInfo& info : achievedSphereInfoList) {
        sphereCount[info.color]++;
    }

    sphereCount[Sphere::Type::BLUE]   *= rateMap[Sphere::Type::BLUE   - 1];
    sphereCount[Sphere::Type::YELLOW] *= rateMap[Sphere::Type::YELLOW - 1];
    sphereCount[Sphere::Type::RED]    *= rateMap[Sphere::Type::RED    - 1];

    return sphereCount;
}

int GameSceneManager::calculateTotalSphereScore(const std::vector<AchievedSphereInfo>& achievedSphereInfoList)
{
    int totalScore = 0;

    for (auto sphereCount : calculateScore(achievedSphereInfoList)) {
        totalScore += sphereCount.second;
    }

    return totalScore;
}

int GameSceneManager::calculateTimeBonus(float elapsedTime)
{
    return (int)((PLAY_SECONDS - elapsedTime) * 10.f);
}

const Vec3 GameSceneManager::getCameraPosition() const
{
    CCASSERT(gameScene != nullptr, "gameScene is not set");
    return this->gameScene->getCameraPosition();
}

const Vec3 GameSceneManager::getCameraEye() const
{
    CCASSERT(gameScene != nullptr, "gameScene is not set");
    return this->gameScene->getCameraEye();
}