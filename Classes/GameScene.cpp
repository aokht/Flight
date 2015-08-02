//
//  GameScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#include <string>
#include <sstream>
#include <iostream>

#include "GameScene.h"
#include "Global.h"
#include "Airplane.h"
#include "Sphere.h"
#include "Field.h"
#include "HelloWorldScene.h"
#include "GameSceneManager.h"
#include "FieldDataSource.h"

using namespace std;
using namespace cocos2d;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();

    auto layer = GameScene::create();
    scene->addChild(layer);

    return scene;
}

bool GameScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    Director::getInstance()->setDepthTest(true);

    this->onTouch = false;
    this->running = false;
    this->runningTime = 0.f;
    this->coinCount = 0;

    return true;
}

void GameScene::onEnter()
{
    Layer::onEnter();

    this->setupField();
    this->setupAirplane();
    this->setupSkyDome();
    this->setupUI();
    this->setupCamera();
    this->setupEventListeners();

    this->scheduleUpdate();
}

void GameScene::update(float dt)
{
    Layer::update(dt);

    static float noTouchTime = 0.f;

    if (this->running) {
        if (! this->onTouch) {
            // タッチ中で無ければ進行方向をデフォルトに戻そうとする
            if (noTouchTime > 0.1f) {  // 適当な待ち時間
                this->airplane->setRotationToDefault(dt);
            }
            noTouchTime += dt;
        } else {
            noTouchTime = 0.f;
        }

        this->airplane->step(dt);
        this->field->step(dt);

        int sphereCount = this->field->getSphereCollisionCount();
        this->incrementCoinCount(sphereCount);


        if (this->checkGameEnds()) {
            this->endGame();
        }

        this->updateRunningTime(dt);

        if (Director::getInstance()->isDisplayStats()) {
            this->updateDebugInfo();
        }
    }
}

void GameScene::updateRunningTime(float dt)
{
    this->runningTime += dt;
    this->labelTime->setString(StringUtils::format("%.2f", 120.f - this->runningTime));
}

void GameScene::incrementCoinCount(int count)
{
    this->coinCount += count;
    this->labelCoins->setString(StringUtils::toString(this->coinCount));
}

bool GameScene::checkGameEnds()
{
    // 衝突検知
    if (this->field->collisionDetectionEnabled()) {
        // 前方 10
        if (this->field->isIntersect(this->field->getAirplanePosition(), Vec3(0, 0, 10))) {
            CCLOG("COLLISION!! forward 10");
            return true;
        }

        // 下方 10
        if (this->field->isIntersect(this->field->getAirplanePosition(), Vec3(0, -10, 0))) {
            CCLOG("COLLISION!! down 10");
            return true;
        }
    }

    return this->coinCount == this->sphereList.size() || this->runningTime > 120.f;
}

void GameScene::endGame()
{
    GameScore score({
        coinCount,
        runningTime,
    });
    GameSceneManager::getInstance()->showResultScene(score);
}

void GameScene::setupField()
{
    int fieldId = GameSceneManager::getInstance()->getSceneData().stageId;
    this->field = Field::createById(fieldId, true, true);
    this->addChild(field);
}

void GameScene::setupSkyDome()
{
    FieldData data = FieldDataSource::findById(this->field->getFieldId());

    this->skydome = Sprite3D::create(data.filenameSky);
    skydome->setPosition3D(Vec3(0, -2000, 0));
    this->addChild(skydome);
}

void GameScene::setupAirplane()
{
    int airplaneId = GameSceneManager::getInstance()->getSceneData().airplaneId;
    this->airplane = Airplane::createById(airplaneId);
    this->addChild(airplane);

    this->field->setAirplaneToField(airplane);
}

void GameScene::setupCamera()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Camera* camera = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 1, 80000);  // TODO: n/f 調整
    this->camera = camera;

    this->airplane->setCameraToAirplane(camera);

    Camera::getDefaultCamera()->setCameraFlag(CameraFlag::DEFAULT);
    this->header->setCameraMask((unsigned short)CameraFlag::DEFAULT);

    this->camera->setCameraFlag(CameraFlag::USER1);
    this->field->setCameraMask((unsigned short)CameraFlag::USER1);
    this->airplane->setCameraMask((unsigned short)CameraFlag::USER1);
    this->skydome->setCameraMask((unsigned short)CameraFlag::USER1);
}

void GameScene::setupUI()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // make header
    this->header = Sprite::create();
    header->setColor(Color3B::WHITE);
    header->setTextureRect(Rect(0, 0, visibleSize.width, 80.f));
    header->setAnchorPoint(Vec2(0.f, 1.f));
    header->setPosition(Vec2(0.f, visibleSize.height));
    header->setOpacity(100);
    this->addChild(header);

    Label* labelCoinsName = Label::createWithTTF("Coins: ", "fonts/arial.ttf", 50.f);
    labelCoinsName->setColor(Color3B::BLACK);
    labelCoinsName->setAnchorPoint(Vec2(0.f, 0.f));
    labelCoinsName->setPosition(Vec2(0.f, 0.f));
    header->addChild(labelCoinsName);

    Label* labelCoinsValue = Label::createWithTTF("0", "fonts/arial.ttf", 50);
    labelCoinsValue->setColor(Color3B::BLACK);
    labelCoinsValue->setAnchorPoint(Vec2(0.f, 0.f));
    labelCoinsValue->setPosition(Vec2(labelCoinsName->getContentSize().width + 10, 0.f));
    header->addChild(labelCoinsValue);
    this->labelCoins = labelCoinsValue;

    Label* labelTimeName = Label::createWithTTF("Time: ", "fonts/arial.ttf", 50.f);
    labelTimeName->setColor(Color3B::BLACK);
    labelTimeName->setAnchorPoint(Vec2(0.f, 0.f));
    labelTimeName->setPosition(Vec2(header->getContentSize().width * 0.35f, 0.f));
    header->addChild(labelTimeName);

    Label* labelTimeNum = Label::createWithTTF("00.00", "fonts/arial.ttf", 50);
    labelTimeNum->setColor(Color3B::BLACK);
    labelTimeNum->setAnchorPoint(Vec2(0.f, 0.f));
    labelTimeNum->setPosition(Vec2(labelTimeName->getPosition().x + labelTimeName->getContentSize().width + 10, 0.f));
    header->addChild(labelTimeNum);
    this->labelTime = labelTimeNum;

    // make start / stop button
    ui::Button* startButton = ui::Button::create("ui/startButton.png", "ui/startButtonPressed.png");
    startButton->setAnchorPoint(Vec2(1.0f, 1.0f));
    startButton->setPosition(header->getContentSize() - Size(20, 0));
    startButton->setScale(1.3);
    this->startButton = startButton;
    header->addChild(startButton);
    startButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            if (this->running) {
                this->startButton->loadTextures("ui/startButton.png", "ui/startButtonPressed.png");
            } else {
                this->startButton->loadTextures("ui/stopButton.png", "ui/stopButtonPressed.png");
            }
            this->running = !this->running;
        }
    });

    // reset button
    ui::Button* resetButton = ui::Button::create("ui/resetButton.png", "ui/resetButtonPressed.png");
    resetButton->setAnchorPoint(Vec2(1.f, 1.f));
    resetButton->setPosition(Vec2(startButton->getPosition().x - startButton->getContentSize().width - 30, header->getContentSize().height));
    resetButton->setScale(1.3);
    header->addChild(resetButton);
    resetButton->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            GameSceneManager::getInstance()->showGameScene();
        }
    });

    if (Director::getInstance()->isDisplayStats()) {
        this->labelPosition = Label::createWithTTF("Position: (00000.0, 00000.0, 00000.0)", "fonts/arial.ttf", 24);
        labelPosition->setAnchorPoint(Vec2(0.f, 0.f));
        labelPosition->setPosition(Vec2(0.f, 180));
        this->addChild(labelPosition);

        this->labelRotation = Label::createWithTTF("Rotation: (000.0, 000.0, 000.0)", "fonts/arial.ttf", 24);
        labelRotation->setAnchorPoint(Vec2(0.f, 0.f));
        labelRotation->setPosition(Vec2(0.f, 150));
        this->addChild(labelRotation);

        this->labelSpriteRotation = Label::createWithTTF("SpriteRotation: (000.0, 000.0, 000.0)", "fonts/arial.ttf", 24);
        labelSpriteRotation->setAnchorPoint(Vec2(0.f, 0.f));
        labelSpriteRotation->setPosition(Vec2(0.f, 120));
        this->addChild(labelSpriteRotation);

        this->labelRotationTarget = Label::createWithTTF("RotationTarget: (000.0, 000.0, 000.0)", "fonts/arial.ttf", 24);
        labelRotationTarget->setAnchorPoint(Vec2(0.f, 0.f));
        labelRotationTarget->setPosition(Vec2(0.f, 90));
        this->addChild(labelRotationTarget);
    }
}

void GameScene::setupEventListeners()
{
    auto touchEventListener = EventListenerTouchOneByOne::create();

    static Vec2 touchStart;
    static float ux_max_drag_px = 200.f;  // TODO: ドラッグ可能量

    touchEventListener->onTouchBegan = [this](Touch* touch, Event* event)
    {
        this->onTouch = true;
        touchStart = touch->getLocation();

        this->airplane->onInputBegan();

        return true;
    };
    touchEventListener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        Vec2 touchEnd = touch->getLocation();
        Vec2 diff = touchEnd - touchStart;

        // ドラッグ可能量を超えていた時に、超えた分だけ始点をずらす(反対側にドラッグした時にすぐ反応するために)
        if (abs(diff.x) > ux_max_drag_px) {
            touchStart.x += (abs(diff.x) - ux_max_drag_px) * (diff.x < 0.f ? -1.f : 1.f);
            diff.x = ux_max_drag_px * (diff.x < 0.f ? -1.f : 1.f);
        }
        if (abs(diff.y) > ux_max_drag_px) {
            touchStart.y += (abs(diff.y) - ux_max_drag_px) * (diff.y < 0.f ? -1.f : 1.f);
            diff.y = ux_max_drag_px * (diff.y < 0.f ? -1.f : 1.f);
        }

        // % に変換
        diff = diff * (100.f / ux_max_drag_px);
        this->airplane->onInputMoved(diff);
    };
    touchEventListener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        Vec2 touchEnd = touch->getLocation();
        Vec2 diff = touchEnd - touchStart;

        this->airplane->onInputEnded(diff);

        this->onTouch = false;
        touchStart.setZero();
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEventListener, this);

    auto keyEventListener = EventListenerKeyboard::create();
    static bool shiftKeyPressed = false;
    keyEventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){
        switch (keyCode) {
            case EventKeyboard::KeyCode::KEY_SPACE:
                this->running = true;
                break;

            case EventKeyboard::KeyCode::KEY_SHIFT:
                shiftKeyPressed = true;

            default:
                break;
        }
    };
    keyEventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event){
        switch(keyCode){
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            {
                if (shiftKeyPressed) {
                    this->airplane->setRotation3D(this->airplane->getRotation3D() + Vec3(0, 10, 0));
                } else {
                    this->airplane->setPosition3D(this->airplane->getPosition3D() + Vec3(100, 0, 0));
                }
            }
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            {
                if (shiftKeyPressed) {
                    this->airplane->setRotation3D(this->airplane->getRotation3D() + Vec3(0, -10, 0));
                } else {
                    this->airplane->setPosition3D(this->airplane->getPosition3D() + Vec3(-100, 0, 0));
                }
            }
                break;
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
            {
                if (shiftKeyPressed) {
                    this->airplane->setRotation3D(this->airplane->getRotation3D() + Vec3(-10, 0, 0));
                } else {
                    this->airplane->setPosition3D(this->airplane->getPosition3D() + Vec3(0, 0, -100));
                }
            }
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            {
                if (shiftKeyPressed) {
                    this->airplane->setRotation3D(this->airplane->getRotation3D() + Vec3(10, 0, 0));
                } else {
                    this->airplane->setPosition3D(this->airplane->getPosition3D() + Vec3(0, 0, 100));
                }
            }
                break;
            case EventKeyboard::KeyCode::KEY_U:
            {
                this->airplane->setPosition3D(this->airplane->getPosition3D() + Vec3(0, 100, 0));
            }
                break;
            case EventKeyboard::KeyCode::KEY_D:
            {
                this->airplane->setPosition3D(this->airplane->getPosition3D() + Vec3(0, -100, 0));
            }
                break;

            case EventKeyboard::KeyCode::KEY_SPACE:
                this->running = false;
                break;
            case EventKeyboard::KeyCode::KEY_SHIFT:
                shiftKeyPressed = false;
                break;

            default:
                log("default: %d", keyCode);
                break;
        }
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyEventListener, this);
}

void GameScene::updateDebugInfo()
{
    Vec3 position = -this->field->getPosition3D();
    Vec3 rotation = this->airplane->getRotation3D();
    Vec3 spriteRotation = this->airplane->getSpriteRotation();
    Vec3 rotationTarget = this->airplane->getRotationTarget();

    this->labelPosition->setString(StringUtils::format("Position: (%.1f, %.1f, %.1f)", position.x, position.y, position.z));
    this->labelRotation->setString(StringUtils::format("Rotation: (%.1f, %.1f, %.1f)", rotation.x, rotation.y, rotation.z));
    this->labelSpriteRotation->setString(StringUtils::format("SpriteRotation: (%.1f, %.1f, %.1f)", spriteRotation.x, spriteRotation.y, spriteRotation.z));
    this->labelRotationTarget->setString(StringUtils::format("RotationTarget: (%.1f, %.1f, %.1f)", rotationTarget.x, rotationTarget.y, rotationTarget.z));
}