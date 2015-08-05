//
//  ResultScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#include "cocostudio/CocoStudio.h"
#include "ResultScene.h"
#include "GameSceneManager.h"
#include "Global.h"
#include "Field.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* ResultScene::createScene(const GameScene::GameScore& score)
{
    Scene* scene = Scene::create();

    Layer* layer = ResultScene::createWithScore(score);
    scene->addChild(layer);

    return scene;
}

ResultScene* ResultScene::createWithScore(const GameScene::GameScore& score)
{
    ResultScene* pRet = new(std::nothrow) ResultScene(score);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

ResultScene::ResultScene(const GameScene::GameScore& score) :
    score(score)
{
}

bool ResultScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = CSLoader::createNode("GameScene/ResultScene.csb");
    this->addChild(rootNode);

    return true;
}

void ResultScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
    this->setupScores();
}

void ResultScene::grabElements()
{
    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");
    CCASSERT(nextButton, "NextButton in ResultScene is not found");

    this->lobbyButton = this->rootNode->getChildByName<ui::Button*>("LobbyButton");
    CCASSERT(lobbyButton, "LobbyButton in ResultScene is not found");

    this->coinCountLabel = this->rootNode->getChildByName<ui::Text*>("CoinsValueLabel");
    CCASSERT(coinCountLabel, "CoinsValueLabel in ResultScene is not found");

    this->timeCountLabel = this->rootNode->getChildByName<ui::Text*>("TimeValueLabel");
    CCASSERT(timeCountLabel, "TimeValueLabel in ResultScene is not found");

    this->totalScoreLabel = this->rootNode->getChildByName<ui::Text*>("TotalValueLabel");
    CCASSERT(totalScoreLabel, "TotalValueLabel in ResultScene is not found");

    this->courseMapNode = this->rootNode->getChildByName<Node*>("CourseMap");
    CCASSERT(courseMapNode, "CourseMap in ResultScene is not found");
}

void ResultScene::setupUI()
{
    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            GameSceneManager::getInstance()->showGameScene();
        }
    });

    this->lobbyButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showLobbyScene();
        }
    });

    // フィールド
    int stageId = GameSceneManager::getInstance()->getSceneData().stageId;
    Field* field = Field::createById(stageId);  // キャッシュが効くはず
    field->setScale(0.025);
    field->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, -360, 0)), nullptr)));
    this->courseMapNode->addChild(field);
    this->courseMapNode->setRotation3D(Vec3(10, -5.25, -5.25));  // 目測
    this->courseMapNode->setPosition3D(this->courseMapNode->getPosition3D() + Vec3(-100.f, 60.f, 250.f));
}

void ResultScene::setupScores()
{
    this->coinCountLabel->setString(StringUtils::toString(score.sphereList.size()));
    this->timeCountLabel->setString(StringUtils::format("%.2f", PLAY_SECONDS - score.elapsedTime));
}