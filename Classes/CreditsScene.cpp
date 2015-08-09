//
//  CreditsScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/07.
//
//

#include "cocosGUI.h"
#include "CreditsScene.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* CreditsScene::createScene()
{
    auto scene = Scene::create();

    auto layer = CreditsScene::create();
    scene->addChild(layer);

    return scene;
}

bool CreditsScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    Node* rootNode = SceneManager::createCSNode("CreditsScene.csb");
    this->addChild(rootNode);

    rootNode->getChildByName<ui::Button*>("BackButton")->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showLobbyScene();
        }
    });

    return true;
}