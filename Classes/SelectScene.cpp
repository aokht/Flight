//
//  SelectScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/25.
//
//

#include <iostream>
#include "cocostudio/CocoStudio.h"
#include "SelectScene.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

Scene* SelectScene::createScene()
{
    Scene* scene = Scene::create();

    Layer* layer = SelectScene::create();
    scene->addChild(layer);

    return scene;
}

bool SelectScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = CSLoader::createNode("SelectScene.csb");
    this->addChild(rootNode);

    return true;
}

void SelectScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();

    this->loadStages();
    this->loadAirplanes();

    // TODO: 前回のステージとか
    this->showStage(0);
    this->showAirplane(0);
}

void SelectScene::grabElements()
{
    this->stageNode = this->rootNode->getChildByName<Node*>("StageNode");;
    CCASSERT(stageNode, "StageNode in SelectScene is not found");

    this->airplaneNode = this->rootNode->getChildByName<Node*>("AirplaneNode");;
    CCASSERT(airplaneNode, "AirplaneNode in SelectScene is not found");

    this->backgroundSprite = this->rootNode->getChildByName<Sprite*>("BackgroundSprite");;
    CCASSERT(backgroundSprite, "BackgroundSprite in SelectScene is not found");

    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");;
    CCASSERT(nextButton, "NextButton in SelectScene is not found");

    this->backButton = this->rootNode->getChildByName<ui::Button*>("BackButton");;
    CCASSERT(backButton, "BackButton in SelectScene is not found");

    this->stageLeftButton = this->rootNode->getChildByName<ui::Button*>("StageLeftButton");;
    CCASSERT(stageLeftButton, "StageLeftButton in SelectScene is not found");

    this->stageRightButton = this->rootNode->getChildByName<ui::Button*>("StageRightButton");;
    CCASSERT(stageRightButton, "StageRightButton in SelectScene is not found");

    this->airplaneLeftButton = this->rootNode->getChildByName<ui::Button*>("AirplaneLeftButton");;
    CCASSERT(airplaneLeftButton, "AirplaneLeftButton in SelectScene is not found");

    this->airplaneRightButton = this->rootNode->getChildByName<ui::Button*>("AirplaneRightButton");;
    CCASSERT(airplaneRightButton, "AirplaneRightButton in SelectScene is not found");

    this->stageNameLabel = this->rootNode->getChildByName<ui::Text*>("StageNameLabel");;
    CCASSERT(stageNameLabel, "StageNameLabel in SelectScene is not found");

    this->airplaneNameLabel = this->rootNode->getChildByName<ui::Text*>("AirplaneNameLabel");;
    CCASSERT(airplaneNameLabel, "AirplaneNameLabel in SelectScene is not found");
}

void SelectScene::setupUI()
{
    this->backButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showLobbyScene();
        }
    });

    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager* sceneManager = SceneManager::getInstance();

            SceneData data = sceneManager->getSceneData();
            data.stageId = fieldList.at(currentField)->getFieldId();
            data.airplaneId = airplaneList.at(currentAirplane)->getAirplaneId();
            sceneManager->setSceneData(data);

            sceneManager->showGameScene();
        }
    });

    this->stageRightButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            this->showStage(currentField + 1);
        }
    });

    this->stageLeftButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            this->showStage(currentField - 1);
        }
    });

    this->airplaneRightButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            this->showAirplane(currentAirplane + 1);
        }
    });

    this->airplaneLeftButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            this->showAirplane(currentAirplane - 1);
        }
    });
}

void SelectScene::loadStages()
{
    for (FieldData data : FieldDataSource::findAll()) {
        Field* field = Field::createWithData(data);
        field->setScale(0.03);
        field->setRotation3D(Vec3(5, 0, 0));
        this->fieldList.pushBack(field);
    }
}

void SelectScene::loadAirplanes()
{
    for (AirplaneData data : AirplaneDataSource::findAll()) {
        Airplane* airplane = Airplane::createByData(data);
        airplane->setScale(100);
        airplane->setRotation3D(Vec3(15, 0, 0));
        this->airplaneList.pushBack(airplane);
    }
}

void SelectScene::showStage(int index)
{
    if (index < 0) {
        index = fieldList.size() - 1;
    } else {
        index = index % fieldList.size();
    }

    Field* prevField = this->stageNode->getChildByName<Field*>("spriteField");
    if (prevField) {
        prevField->removeFromParent();
    }

    Field* field = fieldList.at(index);
    this->stageNameLabel->setString(field->getFieldName());
    this->stageNode->addChild(field, 0, "spriteField");
    this->currentField = index;

    field->runAction(RepeatForever::create(Sequence::create(RotateBy::create(20, Vec3(0, -360, 0)), nullptr)));
}

void SelectScene::showAirplane(int index)
{
    if (index < 0) {
        index = airplaneList.size() - 1;
    } else {
        index = index % airplaneList.size();
    }

    Airplane* prevAirplane = this->airplaneNode->getChildByName<Airplane*>("spriteAirplane");
    if (prevAirplane) {
        prevAirplane->removeFromParent();
    }

    Airplane* airplane = airplaneList.at(index);
    this->airplaneNameLabel->setString(airplane->getAirplaneName());
    this->airplaneNode->addChild(airplane, 0, "spriteAirplane");
    this->currentAirplane = index;

    airplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(20, Vec3(0, -360, 0)), nullptr)));
}