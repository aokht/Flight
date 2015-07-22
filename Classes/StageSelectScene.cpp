//
//  StageSelectScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include "cocostudio/CocoStudio.h"
#include "StageSelectScene.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* StageSelectScene::createScene()
{
    Scene* scene = Scene::create();

    Layer* layer = StageSelectScene::create();
    scene->addChild(layer);

    return scene;
}

bool StageSelectScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = CSLoader::createNode("StageSelectScene.csb");
    this->addChild(rootNode);

    return true;
}

void StageSelectScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
}

void StageSelectScene::grabElements()
{
    this->stageNode = this->rootNode->getChildByName<Node*>("StageNode");;
    CCASSERT(stageNode, "StageNode in StageSelectScene is not found");

    this->backgroundSprite = this->rootNode->getChildByName<Sprite*>("BackgroundSprite");;
    CCASSERT(backgroundSprite, "BackgroundSprite in StageSelectScene is not found");

    this->stageNameLabel = this->rootNode->getChildByName<ui::Text*>("StageNameLabel");;
    CCASSERT(stageNameLabel, "StageNameLabel in StageSelectScene is not found");

    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");;
    CCASSERT(nextButton, "NextButton in StageSelectScene is not found");

    this->backButton = this->rootNode->getChildByName<ui::Button*>("BackButton");;
    CCASSERT(nextButton, "BackButton in StageSelectScene is not found");

    this->leftButton = this->rootNode->getChildByName<ui::Button*>("LeftButton");;
    CCASSERT(leftButton, "LeftButton in StageSelectScene is not found");

    this->rightButton = this->rootNode->getChildByName<ui::Button*>("RightButton");;
    CCASSERT(rightButton, "RightButton in StageSelectScene is not found");
}

void StageSelectScene::setupUI()
{
    this->backButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showAirplaneSelectScene();
        }
    });

    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showGameScene();
        }
    });
}