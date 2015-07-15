//
//  ParameterScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#include "cocostudio/CocoStudio.h"
#include "ParameterScene.h"
#include "GameSceneManager.h"

using namespace cocos2d;

Scene* ParameterScene::createScene()
{
    Scene* scene = Scene::create();

    Layer* layer = ParameterScene::create();
    scene->addChild(layer);

    return scene;
}

bool ParameterScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = CSLoader::createNode("GameScene/ParameterScene.csb");
    this->addChild(rootNode);

    return true;
}

void ParameterScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
}

void ParameterScene::grabElements()
{
    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");
    CCASSERT(nextButton, "NextButton in ParameterScene is not found");
}

void ParameterScene::setupUI()
{
    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            GameSceneManager::getInstance()->showGameScene();
        }
    });
}