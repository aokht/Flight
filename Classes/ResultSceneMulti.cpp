//
//  ResultSceneMulti.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/03.
//
//

#include "cocostudio/CocoStudio.h"
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

    this->rootNode = CSLoader::createNode("GameScene/ResultSceneMulti.csb");
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

    this->playerScoreLabel = this->rootNode->getChildByName<ui::Text*>("PlayerScoreLabel");
    CCASSERT(playerScoreLabel, "PlayerScoreLabel in ResultSceneMulti is not found");

    this->otherScoreLabel = this->rootNode->getChildByName<ui::Text*>("OtherScoreLabel");
    CCASSERT(otherScoreLabel, "OtherScoreLabel in ResultSceneMulti is not found");

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

    // tmp: フィールドを回転させてみる
    Field* field = Field::createById(sceneData.stageId);
    field->setScale(0.04);
    field->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, 360, 0)), nullptr)));
    this->fieldNode->addChild(field);
    this->fieldNode->setRotation3D(Vec3(10, 0.f, 0.f));  // 目測

    // tmp: 飛行機を回転させてみる
    Airplane* playerAirplane = Airplane::createById(sceneData.airplaneId);
    playerAirplane->setScale(70);
    playerAirplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, 360, 0)), nullptr)));
    this->playerAirplaneNode->addChild(playerAirplane);
    this->playerAirplaneNode->setRotation3D(Vec3(20, -5.25, 5.25));  // 目測

    Airplane* otherAirplane = Airplane::createById(sceneData.targetAirplaneId);
    otherAirplane->setScale(70);
    otherAirplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, 360, 0)), nullptr)));
    this->otherAirplaneNode->addChild(otherAirplane);
    this->otherAirplaneNode->setRotation3D(Vec3(20, -5.25, -5.25));  // 目測
}

void ResultSceneMulti::setupScores()
{
    this->playerScoreLabel->setString(StringUtils::toString(score.sphereList.size()));
    this->otherScoreLabel->setString(StringUtils::toString(score.otherAirplaneScore));
}