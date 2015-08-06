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
#include "Sprite3DBatchNode.h"
#include "SceneManager.h"

using namespace std;
using namespace cocos2d;

bool GameScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    Director::getInstance()->setDepthTest(true);

    this->onTouch = false;
    this->opening = true;
    this->running = true;
    this->runningTime = 0.f;
    this->coinCount = 0;

    this->isTimeUp = false;
    this->isCollided = false;
    this->isCompleted = false;

    return true;
}

void GameScene::onEnter()
{
    Layer::onEnter();

    this->setupField();
    this->setupSpheres();
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

    // オープニング演出(適当
    static float waitingTime = 3.f;
    if (this->running && this->opening) {
        this->labelTime->setString(StringUtils::format("%.0f", waitingTime == 3.f ? 3.f : waitingTime + 1.f));
        waitingTime -= dt;

        if (waitingTime < 0.f) {
            this->opening = false;
            this->running = true;
        }
    }

    static float noTouchTime = 0.f;

    if (this->running && !this->opening) {
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

        vector<AchievedSphereInfo> achivedSphereInfoList;
        this->field->checkSphereCollision(&achivedSphereInfoList);
        this->incrementCoinCount((int)achivedSphereInfoList.size());

        if (! GameSceneManager::getInstance()->isSinglePlay()) {
            this->sendAirplaneInfoWithSphereInfo(achivedSphereInfoList);
        }

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
    this->labelTime->setString(StringUtils::format("%.2f", PLAY_SECONDS - this->runningTime));
}

void GameScene::incrementCoinCount(int count)
{
    this->coinCount += count;
    this->labelCoins->setString(StringUtils::toString(this->coinCount));
}

bool GameScene::checkGameEnds()
{
    // 全部取り終わったら終わり
    if (this->field->getRemainingSphereCount() == 0) {
        this->isCompleted = true;
        return true;
    }

    // 衝突検知
    if (this->field->collisionDetectionEnabled()) {
        // 前方 10
        if (this->field->isIntersect(this->field->getAirplanePosition(), Vec3(0, 0, 10))) {
            this->isCollided = true;
            return true;
        }

        // 下方 10
        if (this->field->isIntersect(this->field->getAirplanePosition(), Vec3(0, -10, 0))) {
            this->isCollided = true;
            return true;
        }
    }

    if (this->runningTime > PLAY_SECONDS) {
        this->runningTime = PLAY_SECONDS;
        this->isTimeUp = true;
        return true;
    }

    return false;
}

void GameScene::endGame()
{
    // TODO: 終了演出
    this->running = false;
    this->unscheduleUpdate();

    GameScore score({
        this->isTimeUp,
        this->isCollided,
        this->isCompleted,
        this->runningTime,
        this->field->getAchievedSphereInfoList()
    });

    // シングルプレイ
    if (GameSceneManager::getInstance()->isSinglePlay()) {
        GameSceneManager::getInstance()->showResultScene(score);
    }
    // マルチプレイ
    else {
        this->sendGameScore(score);

        // 相手が既に終わっていた場合は結果を表示
        if (this->score.otherAirplaneScore != -1) {
            score.otherAirplaneScore = this->score.otherAirplaneScore;
            score.isOtherAirplaneCollided = this->score.isOtherAirplaneCollided;
            score.isOtherAirplaneCompleted = this->score.isOtherAirplaneCompleted;

            GameSceneManager::getInstance()->showResultScene(score);
        }
        // 相手のスコアが来るまで待機
        else {
            this->score = score;
            SceneManager::getInstance()->showLoadingScene([](){}, "Waiting...");
        }
    }
}

bool GameScene::isGameEnded() const
{
    return !running && (isCompleted || isTimeUp || isCollided);
}

void GameScene::setupField()
{
    int fieldId = GameSceneManager::getInstance()->getSceneData().stageId;
    this->field = Field::createById(fieldId, true, true);
    this->addChild(field);
}

void GameScene::setupSpheres()
{
    this->field->setupSpheres();
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
    GameSceneManager* gameSceneManager = GameSceneManager::getInstance();
    SceneData sceneData = gameSceneManager->getSceneData();

    int airplaneId = sceneData.airplaneId;
    this->airplane = Airplane::createById(airplaneId);
    this->addChild(airplane);

    airplane->setRotation3D(Vec3(0, -180, 0));          // TODO
    airplane->setPosition3D(Vec3(4500, 400, 4500));  // TODO

    if (! gameSceneManager->isSinglePlay()) {
        // TODO: peerID で複数人対応
        int peerId = DUMMY_PEER_ID;
        int targetAirplaneId = sceneData.targetAirplaneId;
        Airplane* targetAirplane = Airplane::createById(targetAirplaneId);
        targetAirplane->setScale(25);

        if (gameSceneManager->isMultiplayMaster()) {
            // TODO
            targetAirplane->setPosition3D(Vec3(-4500, 400, -4500));
        }
        else {
            // TODO
            targetAirplane->setPosition3D(Vec3(4500, 400, 4500));
            airplane->setPosition3D(Vec3(-4500, 400, -4500));
        }
        this->field->setOtherAirplane(peerId, targetAirplane);
    }

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
    ui::Button* startButton = ui::Button::create("ui/stopButton.png", "ui/stopButtonPressed.png");
    startButton->setAnchorPoint(Vec2(1.0f, 1.0f));
    startButton->setPosition(header->getContentSize() - Size(20, 0));
    startButton->setScale(1.3);
    this->startButton = startButton;
    header->addChild(startButton);
    startButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            if (this->running) {
                // stop
                this->startButton->loadTextures("ui/startButton.png", "ui/startButtonPressed.png");
                this->running = false;
            } else {
                // start
                this->startButton->loadTextures("ui/stopButton.png", "ui/stopButtonPressed.png");
                this->running = true;
            }
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

    if (! GameSceneManager::getInstance()->isSinglePlay()) {
        // hide start / stop / reset buttons
        startButton->setVisible(false);
        resetButton->setVisible(false);
    }

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

        this->labelTargetPosition = Label::createWithTTF("TargetPosition: (000.0, 000.0, 000.0)", "fonts/arial.ttf", 24);
        labelTargetPosition->setAnchorPoint(Vec2(1.f, 0.f));
        labelTargetPosition->setPosition(Vec2(visibleSize.width, 10));
        this->addChild(labelTargetPosition);

        this->labelTargetRotation = Label::createWithTTF("TargetRotation: (000.0, 000.0, 000.0)", "fonts/arial.ttf", 24);
        labelTargetRotation->setAnchorPoint(Vec2(1.f, 0.f));
        labelTargetRotation->setPosition(Vec2(visibleSize.width, 40));
        this->addChild(labelTargetRotation);
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

void GameScene::sendAirplaneInfoWithSphereInfo(const vector<AchievedSphereInfo>& achievedSphereInfoList)
{
    AirplaneInfoNetworkPacket packet(
        this->field->getAirplanePosition(),
        this->airplane->getRotation3D(),
        achievedSphereInfoList
    );
    SceneManager::getInstance()->sendGameSceneAirplaneInfoToPeer(packet);
}

void GameScene::sendGameScore(const GameScene::GameScore& score)
{
    GameScoreNetworkPacket packet({
        (int)score.sphereList.size(),
        score.isTimeUp,
        score.isCollided,
        score.isCompleted
    });
    SceneManager::getInstance()->sendGameSceneScoreToPeer(packet);
}

void GameScene::receivedData(const AirplaneInfoNetworkPacket& data)
{
    int peerId = data.peerId;
    Vec3 position(data.position.x, data.position.y, data.position.z);
    Vec3 rotation(data.rotation.x, data.rotation.y, data.rotation.z);

    this->field->setOtherAirplaneInfo(peerId, position, rotation);
    this->field->setOtherAirplaneAchievedSphere(peerId, data.achievedSphereInfoCount, data.achievedSphereInfoList);
}

void GameScene::receivedData(const GameScoreNetworkPacket& data)
{
    int peerId = data.peerId;  // TODO: 使っていない

    this->score.otherAirplaneScore = data.score;
    this->score.isOtherAirplaneCollided = data.isCollided;
    this->score.isOtherAirplaneCompleted = data.isCompleted;

    // まだプレイ中の場合
    if (! isGameEnded()) {
        // 相手が衝突していた場合はすぐに終わらせる
        if (score.isOtherAirplaneCollided) {
            this->endGame();
        }
        // 時間切れの場合は、こちらが終わるまで待つ
        else {
            ;
        }
    }
    // 既に終わっていた場合
    else {
        GameSceneManager::getInstance()->showResultScene(score);
    }
}

void GameScene::updateDebugInfo()
{
    const Vec3& position = -this->field->getPosition3D();
    const Vec3& rotation = this->airplane->getRotation3D();
    const Vec3& spriteRotation = this->airplane->getSpriteRotation();
    const Vec3& rotationTarget = this->airplane->getRotationTarget();

    this->labelPosition->setString(StringUtils::format("Position: (%.1f, %.1f, %.1f)", position.x, position.y, position.z));
    this->labelRotation->setString(StringUtils::format("Rotation: (%.1f, %.1f, %.1f)", rotation.x, rotation.y, rotation.z));
    this->labelSpriteRotation->setString(StringUtils::format("SpriteRotation: (%.1f, %.1f, %.1f)", spriteRotation.x, spriteRotation.y, spriteRotation.z));
    this->labelRotationTarget->setString(StringUtils::format("RotationTarget: (%.1f, %.1f, %.1f)", rotationTarget.x, rotationTarget.y, rotationTarget.z));

    const map<int, Airplane*>& otherAirplaneList = this->field->getOtherAirplaneList();
    if (! otherAirplaneList.empty()) {
        Airplane* otherAirplane = otherAirplaneList.at(DUMMY_PEER_ID);
        const Vec3& p = otherAirplane->getPosition3D();
        const Vec3& r = otherAirplane->getRotation3D();

        this->labelTargetPosition->setString(StringUtils::format("TargetPosition: (%.1f, %.1f, %.1f)", p.x, p.y, p.z));
        this->labelTargetRotation->setString(StringUtils::format("TargetPosition: (%.1f, %.1f, %.1f)", r.x, r.y, r.z));
    }
}

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}