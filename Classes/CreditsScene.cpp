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
#include "Global.h"
#include "SimpleAudioEngine.h"

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

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
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            SceneManager::getInstance()->backScene();
        }
    });

    return true;
}