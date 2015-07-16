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

Scene* ParameterScene::createScene(const GameScene::GameScore& score)
{
    Scene* scene = Scene::create();

    Layer* layer = ParameterScene::createWithScore(score);
    scene->addChild(layer);

    return scene;
}

ParameterScene* ParameterScene::createWithScore(const GameScene::GameScore& score)
{
    ParameterScene* pRet = new(std::nothrow) ParameterScene(score);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

ParameterScene::ParameterScene(const GameScene::GameScore& score) :
    score(score)
{
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

    this->airplaneNode = this->rootNode->getChildByName<Node*>("Airplane");
    CCASSERT(airplaneNode, "Airplane in ParameterScene is not found");
}

void ParameterScene::setupUI()
{
    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            GameSceneManager::getInstance()->showGameScene();
        }
    });

    // tmp: 飛行機を回転させてみる
    Sprite3D* airplane = Sprite3D::create("airplanes/boss.obj");
    airplane->setTexture("airplanes/boss.png");
    airplane->setScale(20);
    airplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, 360, 0)), nullptr)));
    this->airplaneNode->addChild(airplane);
    this->airplaneNode->setRotation3D(Vec3(20, -5.25, -5.25));  // 目測
}