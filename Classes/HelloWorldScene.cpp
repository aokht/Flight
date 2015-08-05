#include "cocostudio/CocoStudio.h"
#include "HelloWorldScene.h"
#include "SceneManager.h"
#include "GameSceneManager.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Node* rootNode = CSLoader::createNode("HelloWorldScene.csb");
    this->addChild(rootNode);

    return true;
}

void HelloWorld::onEnter()
{
    Layer::onEnter();

    // TMP: 強制的に LobbyScene に遷移
    SceneManager::getInstance()->showLobbyScene();
}