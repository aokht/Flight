//
//  SelectScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/25.
//
//

#include <iostream>
#include "SelectScene.h"
#include "SceneManager.h"
#include "Global.h"
#include "HighScoreDataSource.h"
#include "SimpleAudioEngine.h"

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

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

    this->rootNode = SceneManager::createCSNode("SelectScene.csb");
    this->addChild(rootNode);

    this->isPrepared = false;

    this->fieldLoading = false;
    this->airplaneLoading = false;

    return true;
}

void SelectScene::onEnter()
{
    Layer::onEnter();

    if (! this->isPrepared) {
        this->isPrepared = true;

        this->grabElements();
        this->setupUI();

        this->loadStages();
        this->loadAirplanes();

        // TODO: 前回のステージとか
        this->showStage(0);
        this->showAirplane(0);

        SimpleAudioEngine::getInstance()->playBackgroundMusic(BGM_LIST[BGM_SUBTITLE], true);
    }
}

void SelectScene::grabElements()
{
    this->nextButton = this->rootNode->getChildByName<ui::Button*>("NextButton");;
    CCASSERT(nextButton, "NextButton in SelectScene is not found");

    this->backButton = this->rootNode->getChildByName<ui::Button*>("BackButton");;
    CCASSERT(backButton, "BackButton in SelectScene is not found");

    this->airplaneNode = this->rootNode->getChildByName<Node*>("AirplaneNode");;
    CCASSERT(airplaneNode, "AirplaneNode in SelectScene is not found");

    this->airplaneLeftButton = this->rootNode->getChildByName<ui::Button*>("AirplaneLeftButton");;
    CCASSERT(airplaneLeftButton, "AirplaneLeftButton in SelectScene is not found");

    this->airplaneRightButton = this->rootNode->getChildByName<ui::Button*>("AirplaneRightButton");;
    CCASSERT(airplaneRightButton, "AirplaneRightButton in SelectScene is not found");

    this->airplaneNameLabel = this->rootNode->getChildByName<ui::Text*>("AirplaneNameLabel");;
    CCASSERT(airplaneNameLabel, "AirplaneNameLabel in SelectScene is not found");

    this->stageNameLabel = this->rootNode->getChildByName<ui::Text*>("StageNameLabel");;
    CCASSERT(stageNameLabel, "StageNameLabel in SelectScene is not found");

    this->stageNode = this->rootNode->getChildByName<Node*>("StageNode");;
    CCASSERT(stageNode, "StageNode in SelectScene is not found");

    this->stageLeftButton = this->rootNode->getChildByName<ui::Button*>("StageLeftButton");;
    CCASSERT(stageLeftButton, "StageLeftButton in SelectScene is not found");

    this->stageRightButton = this->rootNode->getChildByName<ui::Button*>("StageRightButton");
    CCASSERT(stageRightButton, "StageRightButton in SelectScene is not found");

    this->airplaneLoadingIndicator = this->rootNode->getChildByName<ui::Text*>("AirplaneLoadingLabel");
    CCASSERT(airplaneLoadingIndicator, "AirplaneLoadingLabel in SelectScene is not found");

    this->stageLoadingIndicator = this->rootNode->getChildByName<ui::Text*>("StageLoadingLabel");
    CCASSERT(stageLoadingIndicator, "StageLoadingLabel in SelectScene is not found");

    this->highScoreButton = this->rootNode->getChildByName<ui::Button*>("HighScoreButton");
    CCASSERT(highScoreButton, "HighScoreButton in SelectScene is not found");

    if (! canSelectStage()) {
        this->stageLeftButton->setVisible(false);
        this->stageNode->setVisible(false);
        this->stageRightButton->setVisible(false);
        this->stageNameLabel->setVisible(false);
        // TODO: 「相手が選択中」画像を出す
    }
}

void SelectScene::setupUI()
{
    this->backButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            SceneManager::getInstance()->showLobbyScene();
        }
    });

    this->nextButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_IMPORTANT]);

            SceneManager* sceneManager = SceneManager::getInstance();

            int stageId = fieldList.at(fieldIdList[currentField])->getFieldId();
            int airplaneId = airplaneList.at(airplaneIdList[currentAirplane])->getAirplaneId();
            sceneManager->setSelectSceneData(stageId, airplaneId);

            if (sceneManager->isSinglePlay()) {
                sceneManager->showGameScene();
            }
            else {
                // TODO: waiting... 的なサムシング
                sceneManager->sendSelectSceneDataToPeer();
                if (sceneManager->isMultiplayReady()) {
                    sceneManager->showGameScene();
                }
            }
        }
    });

    this->stageRightButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            this->showStage(currentField + 1);
        }
    });

    this->stageLeftButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            this->showStage(currentField - 1);
        }
    });

    this->airplaneRightButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            this->showAirplane(currentAirplane + 1);
        }
    });

    this->airplaneLeftButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            this->showAirplane(currentAirplane - 1);
        }
    });

    this->airplaneNameLabel->setString("");
    this->stageNameLabel->setString("");

    this->stageNode->setPosition3D(this->stageNode->getPosition3D() + Vec3(-125.f, 0.f, 250.f));
    this->airplaneNode->setPosition3D(this->airplaneNode->getPosition3D() + Vec3(125.f, 0.f, 250.f));
    this->stageNode->setRotation3D(Vec3(20.f, 0.f, -10.f));
    this->airplaneNode->setRotation3D(Vec3(20.f, 0.f, 10.f));

    this->stageLoadingIndicator->setVisible(false);
    this->airplaneLoadingIndicator->setVisible(false);

    this->highScoreButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            SceneManager::getInstance()->showHighScoreScene(this->fieldIdList[this->currentField]);
        }
    });
}

void SelectScene::loadStages()
{
    for (const FieldData& data : FieldDataSource::findAll()) {
        fieldIdList.push_back(data.id);
    }
}

void SelectScene::loadAirplanes()
{
    for (const AirplaneData& data : AirplaneDataSource::findAll()) {
        airplaneIdList.push_back(data.id);
    }
}

void SelectScene::showStage(int index)
{
    for (Node* prevField : this->stageNode->getChildren()) {
        prevField->setVisible(false);
    }
    // TODO: loading 的な何か

    if (index < 0) {
        index = (int)fieldIdList.size() - 1;
    } else {
        index = index % fieldIdList.size();
    }
    int fieldId = fieldIdList[index];

    Field* field = fieldList.at(fieldId);
    if (field == nullptr) {
        this->fieldLoading = true;
        this->checkButtonEnable();
        FieldData data = FieldDataSource::findById(fieldId);
        Field::createWithDataAsync(data, [this, fieldId](Field* field, void* param){
            if (field) {
                field->setScale(0.017);
                this->fieldList.insert(fieldId, field);
                this->stageNode->addChild(field, 0, "spriteField");
                this->stageNameLabel->setString(field->getFieldName());
                field->runAction(RepeatForever::create(Sequence::create(RotateBy::create(20, Vec3(0, -360, 0)), nullptr)));

                this->fieldLoading = false;
                this->checkButtonEnable();
            }
        }, nullptr);
        CCLOG("Loading stage: %s", data.filenameTerrain.data());
    }
    else {
        this->stageNameLabel->setString(field->getFieldName());
        field->setVisible(true);
    }

    this->currentField = index;
}

void SelectScene::showAirplane(int index)
{
    for (Node* prevAirplane : this->airplaneNode->getChildren()) {
        prevAirplane->setVisible(false);
    }
    // TODO: loading 的な何か

    if (index < 0) {
        index = (int)airplaneIdList.size() - 1;
    } else {
        index = index % airplaneIdList.size();
    }
    int airplaneId = airplaneIdList[index];

    Airplane* airplane = airplaneList.at(airplaneId);
    if (airplane == nullptr) {
        this->airplaneLoading = true;
        this->checkButtonEnable();
        AirplaneData data = AirplaneDataSource::findById(airplaneId);
        Airplane::createByDataAsync(data, [this, airplaneId](Airplane* airplane, void* param){
            if (airplane) {
                airplane->setScale(50);
                this->airplaneList.insert(airplaneId, airplane);
                this->airplaneNode->addChild(airplane, 0, "spriteAirplane");
                this->airplaneNameLabel->setString(airplane->getAirplaneName());
                airplane->runAction(RepeatForever::create(Sequence::create(RotateBy::create(20, Vec3(0, -360, 0)), nullptr)));

                this->airplaneLoading = false;
                this->checkButtonEnable();
            }
        }, nullptr);
        CCLOG("Loading airplane: %s", data.name.data());
    }
    else {
        this->airplaneNameLabel->setString(airplane->getAirplaneName());
        airplane->setVisible(true);
    }

    this->currentAirplane = index;
}

bool SelectScene::canSelectStage() const
{
    SceneManager* sceneManager = SceneManager::getInstance();
    return sceneManager->isSinglePlay() || sceneManager->isMultiplayMaster();
}

void SelectScene::checkButtonEnable()
{
    this->nextButton->setEnabled(!(airplaneLoading || fieldLoading));

    this->airplaneLoadingIndicator->setVisible(airplaneLoading);
    this->airplaneLeftButton->setEnabled(!airplaneLoading);
    this->airplaneRightButton->setEnabled(!airplaneLoading);

    this->stageLoadingIndicator->setVisible(fieldLoading);
    this->stageLeftButton->setEnabled(!fieldLoading);
    this->stageRightButton->setEnabled(!fieldLoading);
}