//
//  ResultSceneMulti.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/03.
//
//

#include "ResultSceneMulti.h"
#include "GameSceneManager.h"
#include "Global.h"
#include "Field.h"
#include "Airplane.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* ResultSceneMulti::createScene(const GameScene::GameScore& score)
{
    Scene* scene = Scene::create();

    Layer* layer = ResultSceneMulti::createWithScore(score);
    scene->addChild(layer);

    return scene;
}

ResultSceneMulti* ResultSceneMulti::createWithScore(const GameScene::GameScore& score)
{
    ResultSceneMulti* pRet = new(std::nothrow) ResultSceneMulti(score);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

ResultSceneMulti::ResultSceneMulti(const GameScene::GameScore& score) :
    score(score)
{
}

bool ResultSceneMulti::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = SceneManager::createCSNode("GameScene/ResultSceneMulti.csb");
    this->addChild(rootNode);

    return true;
}

void ResultSceneMulti::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
    this->setupScores();
}

void ResultSceneMulti::grabElements()
{
    this->lobbyButton = this->rootNode->getChildByName<ui::Button*>("LobbyButton");
    CCASSERT(lobbyButton, "LobbyButton in ResultSceneMulti is not found");

    this->playerTotalScoreLabel = this->rootNode->getChildByName<ui::Text*>("PlayerScoreLabel");
    CCASSERT(playerTotalScoreLabel, "PlayerScoreLabel in ResultSceneMulti is not found");

    this->playerBlueScoreLabel = this->rootNode->getChildByName<ui::Text*>("PlayerBlueScore");
    CCASSERT(playerBlueScoreLabel, "PlayerBlueScore in ResultSceneMulti is not found");

    this->playerYellowScoreLabel = this->rootNode->getChildByName<ui::Text*>("PlayerYellowScore");
    CCASSERT(playerYellowScoreLabel, "PlayerYellowScore in ResultSceneMulti is not found");

    this->playerRedScoreLabel = this->rootNode->getChildByName<ui::Text*>("PlayerRedScore");
    CCASSERT(playerRedScoreLabel, "PlayerRedScore in ResultSceneMulti is not found");

    this->otherTotalScoreLabel = this->rootNode->getChildByName<ui::Text*>("OtherScoreLabel");
    CCASSERT(otherTotalScoreLabel, "OtherScoreLabel in ResultSceneMulti is not found");

    this->otherBlueScoreLabel = this->rootNode->getChildByName<ui::Text*>("OtherBlueScore");
    CCASSERT(otherBlueScoreLabel, "OtherBlueScore in ResultSceneMulti is not found");

    this->otherYellowScoreLabel = this->rootNode->getChildByName<ui::Text*>("OtherYellowScore");
    CCASSERT(otherYellowScoreLabel, "OtherYellowScore in ResultSceneMulti is not found");

    this->otherRedScoreLabel = this->rootNode->getChildByName<ui::Text*>("OtherRedScore");
    CCASSERT(otherRedScoreLabel, "OtherRedScore in ResultSceneMulti is not found");

    this->fieldNode = this->rootNode->getChildByName<Node*>("FieldNode");
    CCASSERT(fieldNode, "FieldNode in ResultSceneMulti is not found");

    this->playerAirplaneNode = this->rootNode->getChildByName<Node*>("PlayerAirplaneNode");
    CCASSERT(playerAirplaneNode, "PlayerAirplaneNode in ResultSceneMulti is not found");

    this->otherAirplaneNode = this->rootNode->getChildByName<Node*>("OtherAirplaneNode");
    CCASSERT(otherAirplaneNode, "OtherAirplaneNode in ResultSceneMulti is not found");
}

void ResultSceneMulti::setupUI()
{
    this->lobbyButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showLobbyScene();
        }
    });

    const SceneData& sceneData = GameSceneManager::getInstance()->getSceneData();

    // フィールド
    Field* field = Field::createById(sceneData.stageId);  // キャッシュが効くはず
    field->setScale(0.02);
    this->fieldNode->addChild(field);
    this->fieldNode->setRotation3D(Vec3(10, 0.f, 0.f));  // 目測
    this->fieldNode->setPosition3D(this->fieldNode->getPosition3D() + Vec3(0.f, 0.f, 250.f));
    field->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, -360, 0)), nullptr)));

    // 飛行機
    Airplane* playerAirplane = Airplane::createById(sceneData.airplaneId);  // キャッシュが効くはず
    playerAirplane->setScale(50);
    this->playerAirplaneNode->setRotation3D(Vec3(20.f, 0.f, 10.f));
    this->playerAirplaneNode->setPosition3D(this->playerAirplaneNode->getPosition3D() + Vec3(140.f, 0.f, 250.f));
    this->playerAirplaneNode->addChild(playerAirplane);
    playerAirplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, -360, 0)), nullptr)));

    Airplane* otherAirplane = Airplane::createById(sceneData.targetAirplaneId);  // キャッシュが効くはず
    otherAirplane->setScale(50);
    this->otherAirplaneNode->setPosition3D(this->otherAirplaneNode->getPosition3D() + Vec3(-140.f, 0.f, 250.f));
    this->otherAirplaneNode->setRotation3D(Vec3(20.f, 0.f, -10.f));
    this->otherAirplaneNode->addChild(otherAirplane);
    otherAirplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, -360, 0)), nullptr)));
}

void ResultSceneMulti::setupScores()
{
    map<Sphere::Type, int> playerScoreMap = GameSceneManager::calculateScore(score.sphereList);

    // 衝突したら無効

    if (score.isCollided) {
        this->playerBlueScoreLabel->setString("-");
        this->playerYellowScoreLabel->setString("-");
        this->playerRedScoreLabel->setString("-");
        this->playerTotalScoreLabel->setString("0");
    } else {
        this->playerBlueScoreLabel->setString(StringUtils::toString(playerScoreMap[Sphere::Type::BLUE]));
        this->playerYellowScoreLabel->setString(StringUtils::toString(playerScoreMap[Sphere::Type::YELLOW]));
        this->playerRedScoreLabel->setString(StringUtils::toString(playerScoreMap[Sphere::Type::RED]));
        int totalScore = playerScoreMap[Sphere::Type::BLUE] + playerScoreMap[Sphere::Type::YELLOW] + playerScoreMap[Sphere::Type::RED];
        this->playerTotalScoreLabel->setString(StringUtils::toString(totalScore));
    }

    if (score.isOtherAirplaneCollided) {
        this->otherBlueScoreLabel->setString("-");
        this->otherYellowScoreLabel->setString("-");
        this->otherRedScoreLabel->setString("-");
        this->otherTotalScoreLabel->setString("0");
    } else {
        this->otherBlueScoreLabel->setString(StringUtils::toString(score.otherAirplaneScoreMap.at(Sphere::Type::BLUE)));
        this->otherYellowScoreLabel->setString(StringUtils::toString(score.otherAirplaneScoreMap.at(Sphere::Type::YELLOW)));
        this->otherRedScoreLabel->setString(StringUtils::toString(score.otherAirplaneScoreMap.at(Sphere::Type::RED)));
        int totalScore = score.otherAirplaneScoreMap.at(Sphere::Type::BLUE) + score.otherAirplaneScoreMap.at(Sphere::Type::YELLOW) + score.otherAirplaneScoreMap.at(Sphere::Type::RED);
        this->otherTotalScoreLabel->setString(StringUtils::toString(totalScore));
    }
}