//
//  AirplaneSelectScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include "cocostudio/CocoStudio.h"
#include "AirplaneSelectScene.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* AirplaneSelectScene::createScene()
{
    Scene* scene = Scene::create();

    Layer* layer = AirplaneSelectScene::create();
    scene->addChild(layer);

    return scene;
}

bool AirplaneSelectScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = CSLoader::createNode("AirplaneSelectScene.csb");
    this->addChild(rootNode);

    return true;
}

void AirplaneSelectScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
}

void AirplaneSelectScene::grabElements()
{
    this->airplaneNode = this->rootNode->getChildByName<Node*>("AirplaneNode");;
    CCASSERT(airplaneNode, "AirplaneNode in AirplaneSelectScene is not found");

    this->backgroundSprite = this->rootNode->getChildByName<Sprite*>("BackgroundSprite");;
    CCASSERT(backgroundSprite, "BackgroundSprite in AirplaneSelectScene is not found");

    this->airplaneNameLabel = this->rootNode->getChildByName<ui::Text*>("AirplaneNameLabel");;
    CCASSERT(airplaneNameLabel, "AirplaneNameLabel in AirplaneSelectScene is not found");

    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");;
    CCASSERT(nextButton, "NextButton in AirplaneSelectScene is not found");

    this->backButton = this->rootNode->getChildByName<ui::Button*>("BackButton");;
    CCASSERT(nextButton, "BackButton in AirplaneSelectScene is not found");

    this->leftButton = this->rootNode->getChildByName<ui::Button*>("LeftButton");;
    CCASSERT(leftButton, "LeftButton in AirplaneSelectScene is not found");

    this->rightButton = this->rootNode->getChildByName<ui::Button*>("RightButton");;
    CCASSERT(rightButton, "RightButton in AirplaneSelectScene is not found");
}

void AirplaneSelectScene::setupUI()
{
    this->backButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showLobbyScene();
        }
    });

    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showStageSelectScene();
        }
    });
}