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

#include "cocostudio/CocoStudio.h"
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
#include "ExSprite3D.h"
#include "Action3D.h"
#include "HighScoreDataSource.h"
#include "MiniMap.h"
#include "SimpleAudioEngine.h"

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

const float GameScene::circleScale = 0.25f;

bool GameScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    CSLoader* csLoader = CSLoader::getInstance();
    csLoader->registReaderObject("MiniMapReader", (ObjectFactory::Instance)MiniMapReader::getInstance);

    this->rootNode = SceneManager::createCSNode("GameScene/GameScene.csb");
    this->addChild(rootNode);

    Director::getInstance()->setDepthTest(true);

    this->onTouch = false;
    this->running = true;
    this->runningTime = 0.f;
    this->coinCount = 0;

    this->isTimeUp = false;
    this->isCollided = false;
    this->isCompleted = false;

    this->sphereEffectId = -1;

    return true;
}

void GameScene::onEnter()
{
    Layer::onEnter();

    this->grabElemets();
    this->setupField();
    this->setupSpheres();
    this->setupAirplane();
    this->setupSkyDome();
    this->setupMiniMap();
    this->setupUI();
    this->setupCamera();
    this->setupEventListeners();

    SimpleAudioEngine::getInstance()->playBackgroundMusic(BGM_LIST[BGM_STAGE1], true);

    this->startGame();
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

        vector<AchievedSphereInfo> achivedSphereInfoList;
        this->field->checkSphereCollision(&achivedSphereInfoList);
        this->updateSphereCount(achivedSphereInfoList);

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

float GameScene::getGameTime() const
{
    return GameSceneManager::getInstance()->isSinglePlay() ? PLAY_SECONDS : PLAY_SECONDS_MULTI;
}

void GameScene::updateRunningTime(float dt)
{
    this->runningTime += dt;
    float remainingTime = this->getGameTime() - this->runningTime;
    if (remainingTime < EPSILON) {
        remainingTime = 0.f;
    }
    this->labelTime->setString(StringUtils::format("%.2f", remainingTime));
}

void GameScene::updateSphereCount(const std::vector<AchievedSphereInfo>& achievedSphereInfoList)
{
    // スフィア獲得SEは、ループ再生を pause / resume して使いまわす(毎回再生すると飛行機のエンジン音が止まる)
    if (achievedSphereInfoList.empty()) {
        if (sphereEffectId > 0) {
            SimpleAudioEngine::getInstance()->pauseEffect(sphereEffectId);
        }
        return;
    }

    int countList[] = { 0, 0, 0 };
    for (const AchievedSphereInfo& info : achievedSphereInfoList) {
        if (Sphere::Type::NONE < info.color && info.color < Sphere::Type::LAST) {
            countList[info.color - 1]++;
        }
    }

    ui::Text* labelList[] = { blueSphereCount, yellowSphereCount, redSphereCount };
    Sprite* circleList[] = { blueCircle, yellowCircle, redCircle };
    for (int type = Sphere::Type::NONE + 1; type < Sphere::Type::LAST; ++type) {
        int i = type - 1;
        if (countList[i]) {
            labelList[i]->setString(StringUtils::toString(atoi(labelList[i]->getString().data()) + countList[i]));
            labelList[i]->stopAllActions();
            labelList[i]->setScale(1.5f);
            labelList[i]->runAction(ScaleTo::create(0.1f, 1.f));
            circleList[i]->stopAllActions();
            circleList[i]->setScale(circleScale * 1.5f);
            circleList[i]->runAction(ScaleTo::create(0.1f, circleScale));
        }
    }

    if (sphereEffectId < 0) {
        this->playSphereSE();
    } else {
        SimpleAudioEngine::getInstance()->resumeEffect(sphereEffectId);
    }
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

    if (this->runningTime > this->getGameTime()) {
        this->runningTime = this->getGameTime();
        this->isTimeUp = true;
        return true;
    }

    return false;
}

void GameScene::startGame()
{
    // オープニング演出
    Sprite* count3Sprite = Sprite::create("ui/countDown.png", Rect(  0.f, 0.f, 128.f, 128.f));
    Sprite* count2Sprite = Sprite::create("ui/countDown.png", Rect(128.f, 0.f, 128.f, 128.f));
    Sprite* count1Sprite = Sprite::create("ui/countDown.png", Rect(256.f, 0.f, 128.f, 128.f));
    Sprite* startSprite  = Sprite::create("ui/status.png", Rect(0.f, 0.f, 512.f, 128.f));
    count3Sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    count2Sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    count1Sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    startSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    Size visibleSize = Director::getInstance()->getVisibleSize();
    count3Sprite->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    count2Sprite->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    count1Sprite->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    startSprite->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    count3Sprite->setRotation3D(Vec3(0.f, -90.f, 0.f));
    count2Sprite->setRotation3D(Vec3(0.f, -90.f, 0.f));
    count1Sprite->setRotation3D(Vec3(0.f, -90.f, 0.f));
    count3Sprite->setVisible(false);
    count2Sprite->setVisible(false);
    count1Sprite->setVisible(false);
    startSprite->setVisible(false);
    this->addChild(count3Sprite);
    this->addChild(count2Sprite);
    this->addChild(count1Sprite);
    this->addChild(startSprite);

    count3Sprite->setVisible(true);
    count3Sprite->runAction(Sequence::create(
        CallFunc::create([this](){
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[COUNTDOWN_1], false, 1.25f);
            this->playAirplaneSE();
        }),
        RotateBy3D::create(1.f, Vec3(0.f, 180.f, 0.f)),
        CallFunc::create([=](){
            count3Sprite->setVisible(false);
            count2Sprite->setVisible(true);
            count2Sprite->runAction(Sequence::create(
                CallFunc::create([](){ SimpleAudioEngine::getInstance()->playEffect(SE_LIST[COUNTDOWN_1], false, 1.25f); }),
                RotateBy3D::create(1.f, Vec3(0.f, 180.f, 0.f)),
                CallFunc::create([=](){
                    count2Sprite->setVisible(false);
                    count1Sprite->setVisible(true);
                    count1Sprite->runAction(Sequence::create(
                        CallFunc::create([](){ SimpleAudioEngine::getInstance()->playEffect(SE_LIST[COUNTDOWN_1], false, 1.25f); }),
                        RotateBy3D::create(1.f, Vec3(0.f, 180.f, 0.f)),
                        CallFunc::create([=](){
                            count1Sprite->setVisible(false);
                            startSprite->setVisible(true);
                            startSprite->runAction(Sequence::create(
                                CallFunc::create([](){ SimpleAudioEngine::getInstance()->playEffect(SE_LIST[COUNTDOWN_2], false, 2.5f); }),
                                FadeOut::create(2.f),
                                CallFunc::create([=](){
                                    startSprite->setVisible(false);
                                    startSprite->removeFromParent();
                                    count1Sprite->removeFromParent();
                                    count2Sprite->removeFromParent();
                                    count3Sprite->removeFromParent();
                                }),
                                nullptr
                            ));

                            this->running = true;
                            this->scheduleUpdate();
                        }),
                        nullptr
                    ));
                }),
                nullptr
            ));
        }),
        nullptr
    ));
}

void GameScene::playAirplaneSE()
{
    SimpleAudioEngine::getInstance()->playEffect(SE_LIST[AIRPLANE_1], true, 1.f, 0.f, 10.f);
}

void GameScene::playSphereSE()
{
    this->sphereEffectId = SimpleAudioEngine::getInstance()->playEffect(SE_LIST[GET_SPHERE], true);
}

void GameScene::stopGame(bool strict)
{
    this->running = false;
    if (strict) {
        this->unscheduleUpdate();
    }
}

void GameScene::endGame()
{
    this->stopGame();

    SimpleAudioEngine* audio = SimpleAudioEngine::getInstance();
    audio->stopBackgroundMusic();
    audio->stopAllEffects();

    // エンディング
    Sprite* message = nullptr;
    if (isTimeUp) {
        message = Sprite::create("ui/status.png", Rect(0.f, 128.f, 512.f, 128.f));  // Time's Up!
    } else if (isCompleted) {
        message = Sprite::create("ui/status.png", Rect(0.f, 256.f, 512.f, 128.f));  // Completed!
    }

    function<void()> callback = [this](){
        GameScore score({
            this->isTimeUp,
            this->isCollided,
            this->isCompleted,
            this->runningTime,
            this->field->getAchievedSphereInfoList()
        });
        
        // シングルプレイ
        if (GameSceneManager::getInstance()->isSinglePlay()) {
            int stageId = GameSceneManager::getInstance()->getSceneData().stageId;
            // TODO: 得点計算をまとめる
            int sphereScore = GameSceneManager::calculateTotalSphereScore(score.sphereList);
            int timeBonus = 0;
            if (! score.isCollided) {
                timeBonus = GameSceneManager::calculateTimeBonus(score.elapsedTime);
            }
            int totalScore = sphereScore + timeBonus;
            HighScoreDataSource::setGlobalHighScore(stageId, totalScore);
            HighScoreDataSource::setLocalHighScoreIfBigger(stageId, totalScore);

            GameSceneManager::getInstance()->showResultScene(score);
        }
        // マルチプレイ
        else {
            this->sendGameScore(score);

            // 相手が既に終わっていた場合は結果を表示
            if (this->score.otherAirplaneTotalScore != -1) {
                score.otherAirplaneTotalScore = this->score.otherAirplaneTotalScore;
                score.otherAirplaneScoreMap = this->score.otherAirplaneScoreMap;
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
    };

    this->airplane->runAction(FadeOut::create(0.5f));

    if (message) {
        // 時間切れ or 全部取り終わった
        Size size = Director::getInstance()->getVisibleSize();
        message->setAnchorPoint(Vec2(0.5f, 0.5f));
        message->setPosition(Vec2(size.width + message->getContentSize().width * 0.5, size.height * 0.5f));
        this->addChild(message);

        message->runAction(Sequence::create(
            EaseOut::create(MoveTo::create(0.8f, Vec2(size.width * 0.5f, size.height * 0.5f)), 10.f),
            EaseIn::create(MoveTo::create(0.8f, Vec2(-message->getContentSize().width * 0.5f, size.height * 0.5f)), 10.f),
            CallFunc::create([callback](){
                callback();
            }),
            nullptr
        ));

        audio->playEffect(SE_LIST[GAMEEND_FLYBY]);
    } else if (isCollided) {
        // 爆発演出
        ParticleSystemQuad* particleExplosion = ParticleSystemQuad::create("explosion.plist");
        Size size = Director::getInstance()->getVisibleSize();
        particleExplosion->setPosition3D(Vec3(size.width * 0.5f, size.height * 0.45f, 400.f));
        this->addChild(particleExplosion);

        float duration = particleExplosion->getDuration() + particleExplosion->getLife() + particleExplosion->getLifeVar();
        this->runAction(Sequence::create(
            DelayTime::create(duration),
            CallFunc::create([callback](){
                callback();
            }),
            nullptr
        ));

        audio->playEffect(SE_LIST[EXPLOSION]);
    } else {
        // 念のため
        callback();
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
    skydome->setPosition3D(Vec3(0, -FIELD_HEIGHT, 0));
    this->addChild(skydome);
}

void GameScene::setupAirplane()
{
    GameSceneManager* gameSceneManager = GameSceneManager::getInstance();
    SceneData sceneData = gameSceneManager->getSceneData();

    int airplaneId = sceneData.airplaneId;
    this->airplane = Airplane::createById(airplaneId);
    this->addChild(airplane);

    airplane->setPosition3D(this->field->getAirplaneStartPosition());
    airplane->setRotation3D(this->field->getAirplaneStartRotation());

    if (! gameSceneManager->isSinglePlay()) {
        // TODO: peerID で複数人対応
        int peerId = DUMMY_PEER_ID;
        int targetAirplaneId = sceneData.targetAirplaneId;
        Airplane* targetAirplane = Airplane::createById(targetAirplaneId);
        targetAirplane->setScale(25);

        if (gameSceneManager->isMultiplayMaster()) {
            targetAirplane->setPosition3D(this->field->getOtherAirplaneStartPosition());
            targetAirplane->setRotation3D(this->field->getOtherAirplaneStartRotation());
        }
        else {
            targetAirplane->setPosition3D(this->field->getAirplaneStartPosition());
            targetAirplane->setRotation3D(this->field->getAirplaneStartRotation());
            airplane->setPosition3D(this->field->getOtherAirplaneStartPosition());
            airplane->setRotation3D(this->field->getOtherAirplaneStartRotation());
        }
        this->field->setOtherAirplane(peerId, targetAirplane);
    }

    this->field->setAirplane(airplane);
}

void GameScene::setupMiniMap()
{
    this->field->setMiniMap(miniMap);
}

void GameScene::setupCamera()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Camera* camera = Camera::createPerspective(60.f, visibleSize.width / visibleSize.height, 0.1f, 20000.f);
    this->camera = camera;

    this->airplane->setCameraToAirplane(camera);

    Camera::getDefaultCamera()->setCameraFlag(CameraFlag::DEFAULT);
    this->header->setCameraMask((unsigned short)CameraFlag::DEFAULT);

    this->camera->setCameraFlag(CameraFlag::USER1);
    this->field->setCameraMask((unsigned short)CameraFlag::USER1);
    this->airplane->setCameraMask((unsigned short)CameraFlag::USER1);
    this->skydome->setCameraMask((unsigned short)CameraFlag::USER1);
}

void GameScene::grabElemets()
{
    this->header = this->rootNode->getChildByName<Sprite*>("Header");
    CCASSERT(header, "Header in GameScene is not found");

    this->blueCircle =this->header->getChildByName<Sprite*>("BlueCircle");
    this->yellowCircle =this->header->getChildByName<Sprite*>("YellowCircle");
    this->redCircle =this->header->getChildByName<Sprite*>("RedCircle");
    CCASSERT(blueCircle, "BlueCircle in GameScene is not found");
    CCASSERT(yellowCircle, "YellowCircle in GameScene is not found");
    CCASSERT(redCircle, "RedCircle in GameScene is not found");


    this->blueSphereCount = this->header->getChildByName<ui::Text*>("BlueSphereCount");
    this->yellowSphereCount = this->header->getChildByName<ui::Text*>("YellowSphereCount");
    this->redSphereCount = this->header->getChildByName<ui::Text*>("RedSphereCount");
    CCASSERT(blueSphereCount, "BlueSphereCount in GameScene is not found");
    CCASSERT(yellowSphereCount, "YellowSphereCount in GameScene is not found");
    CCASSERT(redSphereCount, "RedSphereCount in GameScene is not found");

    this->blueSphereTotalCount = this->header->getChildByName<ui::Text*>("BlueSphereTotalCount");
    this->yellowSphereTotalCount = this->header->getChildByName<ui::Text*>("YellowSphereTotalCount");
    this->redSphereTotalCount = this->header->getChildByName<ui::Text*>("RedSphereTotalCount");
    CCASSERT(blueSphereTotalCount, "BlueSphereTotalCount in GameScene is not found");
    CCASSERT(yellowSphereTotalCount, "YellowSphereTotalCount in GameScene is not found");
    CCASSERT(redSphereTotalCount, "RedSphereTotalCount in GameScene is not found");

    this->labelTime = this->header->getChildByName<ui::Text*>("TimerLabel");
    CCASSERT(labelTime, "TimerLabel in GameScene is not found");

    this->startButton = this->header->getChildByName<ui::Button*>("StartButton");
    this->quitButton = this->header->getChildByName<ui::Button*>("QuitButton");
    CCASSERT(startButton, "StartButton in GameScene is not found");
    CCASSERT(quitButton, "QuitButton in GameScene is not found");

    this->miniMap = this->rootNode->getChildByName<MiniMap*>("MiniMapNode");
    CCASSERT(miniMap, "MiniMapNode in GameScene is not found");
}

void GameScene::setupUI()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const map<Sphere::Type, int>& sphereCountPerColor = this->field->getSphereCountPerColor();
    this->blueSphereTotalCount->setString(StringUtils::toString(sphereCountPerColor.at(Sphere::Type::BLUE)));
    this->yellowSphereTotalCount->setString(StringUtils::toString(sphereCountPerColor.at(Sphere::Type::YELLOW)));
    this->redSphereTotalCount->setString(StringUtils::toString(sphereCountPerColor.at(Sphere::Type::RED)));

    // タイマーをセット
    this->updateRunningTime(0.f);

    this->startButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine* audio = SimpleAudioEngine::getInstance();
            audio->playEffect(SE_LIST[TAP_NORMAL]);
            if (this->running) {
                // stop
                this->startButton->setTitleText("Start");
                this->quitButton->setVisible(true);
                this->quitButton->setEnabled(true);
                this->running = false;
                audio->pauseBackgroundMusic();
                audio->stopAllEffects();
            } else {
                // start
                this->startButton->setTitleText("Stop");
                this->quitButton->setVisible(false);
                this->quitButton->setEnabled(false);
                this->running = true;
                audio->resumeBackgroundMusic();
                this->playAirplaneSE();
                this->playSphereSE();
            }
        }
    });

    // quit button
    quitButton->setVisible(false);
    this->quitButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect(SE_LIST[TAP_NORMAL]);
            this->isCollided = true;
            this->endGame();
        }
    });

    if (! GameSceneManager::getInstance()->isSinglePlay()) {
        // hide start / stop / quit buttons
        this->startButton->setVisible(false);
        this->quitButton->setVisible(false);
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
    static float ux_max_drag_px = 300.f;  // ドラッグ可能量

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
            touchStart.x += (abs(diff.x) - ux_max_drag_px) * sign(diff.x);
            diff.x = ux_max_drag_px * sign(diff.x);
        }
        if (abs(diff.y) > ux_max_drag_px) {
            touchStart.y += (abs(diff.y) - ux_max_drag_px) * sign(diff.y);
            diff.y = ux_max_drag_px * sign(diff.y);
        }

        // % に変換
        diff = diff / ux_max_drag_px;
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
    map<Sphere::Type, int> scoreMap = GameSceneManager::calculateScore(score.sphereList);

    GameScoreNetworkPacket packet({
        (int)score.sphereList.size(),
        scoreMap[Sphere::Type::BLUE],
        scoreMap[Sphere::Type::YELLOW],
        scoreMap[Sphere::Type::RED],
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

    this->score.otherAirplaneTotalScore = data.score;
    this->score.otherAirplaneScoreMap[Sphere::Type::BLUE] = data.blueCount;
    this->score.otherAirplaneScoreMap[Sphere::Type::YELLOW] = data.yellowCount;
    this->score.otherAirplaneScoreMap[Sphere::Type::RED] = data.redCount;
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

const Vec3 GameScene::getCameraPosition() const
{
    return this->field->getAirplanePosition() + this->camera->getPosition3D();
}

const Vec3 GameScene::getCameraEye() const
{
    return this->field->getAirplanePosition();
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