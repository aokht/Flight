//
//  LoadingScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/05.
//
//

#include "cocostudio/CocoStudio.h"
#include "LoadingScene.h"

using namespace std;
using namespace cocos2d;

Scene* LoadingScene::createScene(const function<void()>& callback, const string& label)
{
    auto scene = Scene::create();

    auto layer = LoadingScene::create(callback, label);
    scene->addChild(layer);

    return scene;
}

LoadingScene* LoadingScene::create(const std::function<void ()>& callback, const string& label)
{
    LoadingScene* scene = new LoadingScene();

    if (scene && scene->init(callback, label)) {
        scene->autorelease();
        return scene;
    }

    CC_SAFE_DELETE(scene);
    return nullptr;
}

bool LoadingScene::init(const function<void()>& callback, const string& label)
{
    if (! Layer::init()) {
        return false;
    }

    Node* rootNode = CSLoader::createNode("LoadingScene.csb");
    this->addChild(rootNode);

    this->textLabel = rootNode->getChildByName<ui::Text*>("LoadingLabel");
    CCASSERT(textLabel, "LoadingLabel in LoadingScene is not found");
    this->textLabel->setString(label);

    this->callback = callback;

    return true;
}

void LoadingScene::onEnter()
{
    Layer::onEnter();

    callback();
}