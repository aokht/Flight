//
//  LobbyScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include "cocostudio/CocoStudio.h"
#include "LobbyScene.h"
#include "Global.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* LobbyScene::createScene()
{
    Scene* scene = Scene::create();

    Layer* layer = LobbyScene::create();
    scene->addChild(layer);

    return scene;
}

bool LobbyScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = CSLoader::createNode("LobbyScene.csb");
    this->addChild(rootNode);

    return true;
}

void LobbyScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
}

void LobbyScene::grabElements()
{
    this->singlePlayerButton = this->rootNode->getChildByName<ui::Button*>("SinglePlayerButton");
    CCASSERT(singlePlayerButton, "SinglePlayerButton in LobbyScene is not found");

    this->multiplayerButton = this->rootNode->getChildByName<ui::Button*>("MultiplayerButton");
    CCASSERT(multiplayerButton, "MultiplayerButton in LobbyScene is not found");
}

void LobbyScene::setupUI()
{
    this->singlePlayerButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showAirplaneSelectScene();
        }
    });

    this->multiplayerButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            // TODO: multiplayer mode not implemented
            SceneManager::getInstance()->showAirplaneSelectScene();
        }
    });
}