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
#include "HelloWorldScene.h"

using namespace std;
using namespace cocos2d;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();

    auto layer = GameScene::create();
    scene->addChild(layer);

    return scene;
}

void GameScene::resetScene()
{
    Director::getInstance()->popScene();

    Scene* scene = GameScene::createScene();
    Director::getInstance()->pushScene(scene);
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
    this->setupCamera();
    this->setupUI();
    this->setupEventListeners();

    this->scheduleUpdate();
}

void GameScene::update(float dt)
{
    Layer::update(dt);

    if (this->running) {
        if (! this->onTouch) {
            // タッチ中で無ければ進行方向をデフォルトに戻そうとする
            this->airplane->setRotationToDefault(dt);
        }

        this->airplane->step(dt);

        const Vec3& airplanePosition = this->airplane->getPosition3D();
        float distance = 1000;  // TODO: 当たり判定距離
        for (auto s = sphereList.begin(), last = sphereList.end(); s != last; ++s) {
            const Vec3 diff = (*s)->getPosition3D() -  airplanePosition;
            if (abs(diff.x) < distance && abs(diff.y) < distance && abs(diff.z) < distance &&  (*s)->isVisible()) {
                (*s)->setVisible(false);
                this->incrementCoinCount(1);
            }
        }

        if (this->checkGameEnds()) {
            this->endGame();
        }

        this->updateRunningTime(dt);
    }
}

void GameScene::updateRunningTime(float dt)
{
    this->runningTime += dt;
    this->labelTime->setString(StringUtils::format("%.2f", this->runningTime));
}

void GameScene::incrementCoinCount(int count)
{
    this->coinCount += count;
    this->labelCoins->setString(StringUtils::toString(this->coinCount));
}

bool GameScene::checkGameEnds()
{
    return this->coinCount == this->sphereList.size();
}

void GameScene::endGame()
{
    string message = StringUtils::format("Your Score is\n  Coins: %d\n  Time: %.2f", this->coinCount, this->runningTime);
    MessageBox(message.c_str(), "Complete!!");
    GameScene::resetScene();
}

void GameScene::setupField()
{
    this->field = Sprite3D::create("fields/field.obj");
    this->field->setPosition3D(Vec3(0, 0, 0));
    this->addChild(this->field);

    vector<Vec3> spherePositionList = Sphere::getSpherePositionList();
    for (auto s = spherePositionList.begin(), last = spherePositionList.end(); s != last; ++s) {
        Sphere* sphere = Sphere::create();
        sphere->setPosition3D(*s * 100);
        sphere->setScale(100);
        field->addChild(sphere);
        sphereList.push_back(sphere);
    }
}

void GameScene::setupAirplane()
{
    // create airplane
    this->airplane = Airplane::create();
    this->airplane->setPosition3D(Vec3(0, 5000, 0));
    this->addChild(this->airplane);
}

void GameScene::setupCamera()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Camera* camera = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 1, 80000);  // TODO: n/f 調整
    this->camera = camera;

    this->airplane->setCameraToAirplane(camera);
}

void GameScene::setupUI()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // make header
    this->header = Sprite::create();
    header->setColor(Color3B::WHITE);
    header->setTextureRect(Rect(0, 0, visibleSize.width, 60.f));
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
    startButton->setPosition(header->getContentSize());
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
    resetButton->setPosition(Vec2(startButton->getPosition().x - startButton->getContentSize().width, header->getContentSize().height));
    header->addChild(resetButton);
    resetButton->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            GameScene::resetScene();
        }
    });

}

void GameScene::setupEventListeners()
{
    auto touchEventListener = EventListenerTouchOneByOne::create();

    static Vec2 touchStart;
    static float ux_max_drag_px = 100.f;  // TODO: ドラッグ可能量

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