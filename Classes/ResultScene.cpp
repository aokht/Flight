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

    // tmp: カウントアップアニメーション系
    this->time = 0.f;
    this->onStart = 0.f;
    this->onCoinCountEnded = 0.f;
    this->onTimeCountEnded = 0.f;

    return true;
}

void ResultScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
}

void ResultScene::onEnterTransitionDidFinish()
{
    this->scheduleUpdate();
}

void ResultScene::update(float dt)
{
    this->animateCountUp(dt);
}

void ResultScene::animateCountUp(float dt)
{

    if (onStart <= 0.f) {
        onStart = time;
    }
    else if (onCoinCountEnded <= 0.f && onStart < time) {
        float dtScore = (time - onStart) * this->score.coinsCount;
        if (time - onStart >= 1.f) {
            onCoinCountEnded = time;
        }

        string scoreString = StringUtils::format("%.0f", min((float)this->score.coinsCount, dtScore));
        this->coinCountLabel->setString(scoreString);
    }
    else if (onCoinCountEnded > 0.f && onTimeCountEnded <= 0 && onCoinCountEnded < time) {
        float dtTime = (time - onCoinCountEnded) * this->score.elapsedTime;
        if (time - onCoinCountEnded >= 1.f) {
            onTimeCountEnded = time;
        }

        string scoreString = StringUtils::format("%.2f", min((float)this->score.elapsedTime, dtTime));
        this->timeCountLabel->setString(scoreString);
    }

    time += dt;
}

void ResultScene::grabElements()
{
    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");
    CCASSERT(nextButton, "NextButton in ResultScene is not found");

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
            GameSceneManager::getInstance()->showParameterScene(score);
        }
    });

    // tmp: フィールドを回転させてみる
    Sprite3D* field = Sprite3D::create("fields/field.obj");
    field->setScale(0.007);
    field->runAction(RepeatForever::create(Sequence::create(RotateBy::create(30, Vec3(0, 360, 0)), nullptr)));
    this->courseMapNode->addChild(field);
    this->courseMapNode->setRotation3D(Vec3(10, -5.25, -5.25));  // 目測
}