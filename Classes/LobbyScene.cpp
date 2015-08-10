//
//  LobbyScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#include <iostream>
#include "LobbyScene.h"
#include "Global.h"
#include "SceneManager.h"
#include "SceneData.h"
#include "Airplane.h"
#include "Action3D.h"
#include "ExSprite3D.h"
#include "GameCenterWrapper.h"

using namespace std;
using namespace cocos2d;

Scene* LobbyScene::createScene()
{
    Scene* scene = Scene::create();

    Layer* layer = LobbyScene::create();
    scene->addChild(layer);

    return scene;
}

bool LobbyScene::init()
{
    if (! Layer::init()) {
        return false;
    }

    this->rootNode = SceneManager::createCSNode("LobbyScene.csb");
    this->addChild(rootNode);

    SceneManager::getInstance()->initSceneData();

    Director::getInstance()->setDepthTest(true);

    return true;
}

void LobbyScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
    this->setup3DModels();
    this->setupOpeningAnimations();

    SceneManager::getInstance()->startAdvertisingAvailability();
    GameCenterWrapper::getInstance()->loginGameCenter();
}

void LobbyScene::onExit()
{
    Layer::onExit();

    SceneManager::getInstance()->stopAdvertisingAvailability();
}

void LobbyScene::grabElements()
{
    this->titleSprite = this->rootNode->getChildByName<Sprite*>("TitleSprite");
    CCASSERT(titleSprite, "TitleSprite in LobbyScene is not found");

    this->singlePlayerButton = this->rootNode->getChildByName<ui::Button*>("SinglePlayerButton");
    CCASSERT(singlePlayerButton, "SinglePlayerButton in LobbyScene is not found");

    this->multiplayerButton = this->rootNode->getChildByName<ui::Button*>("MultiplayerButton");
    CCASSERT(multiplayerButton, "MultiplayerButton in LobbyScene is not found");

    this->backgroundSprite = this->rootNode->getChildByName<Sprite*>("BackgroundSprite");
    CCASSERT(backgroundSprite, "BackgroundSprite in LobbyScene is not found");

    this->backgroundSpriteBlur = this->rootNode->getChildByName<Sprite*>("BackgroundSpriteBlur");
    CCASSERT(backgroundSpriteBlur, "BackgroundSpriteBlur in LobbyScene is not found");

    this->backgroundBox = this->rootNode->getChildByName<Sprite*>("BackgroundBox");
    CCASSERT(backgroundBox, "BackgroundBox in LobbyScene is not found");

    this->creditsButtonBackground = this->rootNode->getChildByName<Sprite*>("InfoButtonBackground");
    CCASSERT(creditsButtonBackground, "InfoButtonBackground in LobbyScene is not found");
    this->creditsButton = creditsButtonBackground->getChildByName<ui::Button*>("InfoButton");
    CCASSERT(creditsButton, "InfoButton in LobbyScene is not found");
}

void LobbyScene::setupUI()
{
    this->singlePlayerButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager* sceneManager = SceneManager::getInstance();
            sceneManager->setSceneData({ SceneData::Mode::SINGLE });
            sceneManager->showSelectScene();
        }
    });

    this->multiplayerButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showPeerList();
        }
    });

    this->creditsButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->showCreditsScene();
        }
    });
}

void LobbyScene::setup3DModels()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    this->airplane = Airplane::createById(1);
    airplane->setRotation3D(Vec3(-15.f, -50.f, 0.f));
    this->addChild(airplane);

    this->sphere1 = ExSprite3D::create("objects/diamond.obj");
    this->sphere2 = ExSprite3D::create("objects/diamond.obj");
    this->sphere3 = ExSprite3D::create("objects/diamond.obj");
    sphere1->setPosition3D(Vec3(visibleSize.width * 0.65f, visibleSize.height * 0.55f, 300.f));
    sphere2->setPosition3D(Vec3(visibleSize.width * 0.55f, visibleSize.height * 0.35f, 300.f));
    sphere3->setPosition3D(Vec3(visibleSize.width * 0.70f, visibleSize.height * 0.30f, 300.f));
    sphere1->setRotation3D(Vec3(0.f,   0.f, 0.f));
    sphere2->setRotation3D(Vec3(0.f,  57.f, 0.f));
    sphere3->setRotation3D(Vec3(0.f, 123.f, 0.f));
    sphere1->setScale(2.f);
    sphere2->setScale(2.f);
    sphere3->setScale(2.f);
    this->addChild(sphere1);
    this->addChild(sphere2);
    this->addChild(sphere3);

    GLProgram* glProgram = GLProgram::createWithFilenames("SphereShaderForTitleScreen.vert", "SphereShaderForTitleScreen.frag");

    GLProgramState* state1 = GLProgramState::create(glProgram);
    GLProgramState* state2 = GLProgramState::create(glProgram);
    GLProgramState* state3 = GLProgramState::create(glProgram);
    sphere1->setGLProgramState(state1);
    sphere2->setGLProgramState(state2);
    sphere3->setGLProgramState(state3);

    // set vertex attributes
    const MeshVertexData* meshVertexData = sphere1->getMeshVertexData();
    for (long i = 0, offset = 0, last = (int)meshVertexData->getMeshVertexAttribCount(); i < last; ++i) {
        const MeshVertexAttrib& attrib = meshVertexData->getMeshVertexAttrib(i);
        state1->setVertexAttribPointer(
                                       s_attributeNames[attrib.vertexAttrib],
                                       attrib.size,
                                       attrib.type,
                                       GL_FALSE,
                                       meshVertexData->getVertexBuffer()->getSizePerVertex(),
                                       (GLvoid*)offset
                                       );
        state2->setVertexAttribPointer(
                                       s_attributeNames[attrib.vertexAttrib],
                                       attrib.size,
                                       attrib.type,
                                       GL_FALSE,
                                       meshVertexData->getVertexBuffer()->getSizePerVertex(),
                                       (GLvoid*)offset
                                       );
        state3->setVertexAttribPointer(
                                       s_attributeNames[attrib.vertexAttrib],
                                       attrib.size,
                                       attrib.type,
                                       GL_FALSE,
                                       meshVertexData->getVertexBuffer()->getSizePerVertex(),
                                       (GLvoid*)offset
                                       );

        offset += attrib.attribSizeBytes;
    }
    state1->setUniformVec4("u_sphereColor", Vec4(0.f, 0.f, 1.f, 1.f));
    state2->setUniformVec4("u_sphereColor", Vec4(0.f, 1.f, 0.f, 1.f));
    state3->setUniformVec4("u_sphereColor", Vec4(1.f, 0.f, 0.f, 1.f));
}

void LobbyScene::setupOpeningAnimations()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // タイトルの初期状態設定
    this->titleSprite->setPosition(Vec2(-titleSprite->getBoundingBox().size.width, titleSprite->getPosition().y));

    // 飛行機の初期状態設定
    this->airplane->setPosition3D(Vec3(visibleSize.width + 100.f, -100.f, 0.f));
    this->airplane->setScale(30.f);

    // もろもろ消しておく
    // ボタン類
    this->singlePlayerButton->setVisible(false);
    this->multiplayerButton->setVisible(false);
    this->creditsButtonBackground->setVisible(false);
    this->singlePlayerButton->setOpacity(0.f);
    this->multiplayerButton->setOpacity(0.f);
    this->creditsButtonBackground->setOpacity(0.f);
    // 背景
    this->backgroundSpriteBlur->setVisible(false);
    this->backgroundBox->setVisible(false);
    // スフィア
    this->sphere1->setVisible(false);
    this->sphere2->setVisible(false);
    this->sphere3->setVisible(false);
    this->sphere1->setOpacity(0.f);
    this->sphere2->setOpacity(0.f);
    this->sphere3->setOpacity(0.f);

    // アニメーションシーケンス
    this->runAction(Sequence::create(
        // タイトル & 飛行機アニメーション
        CallFunc::create([this](){
            Size visibleSize = Director::getInstance()->getVisibleSize();
            float rate = 3.0;
            this->airplane->runAction(
                Spawn::create(
                    EaseIn::create(MoveTo3D::create(0.75f, Vec3(visibleSize.width * 0.62f, visibleSize.height * 0.40f, 300.f)), rate),
                    EaseIn::create(ScaleTo::create(0.75f, 70.f), rate),
                    nullptr
                )

            );
            this->titleSprite->runAction(
                EaseIn::create(MoveTo::create(0.75f, Vec2(visibleSize.width * 0.5f, titleSprite->getPosition().y)), rate)
            );
        }),
        DelayTime::create(0.75f),
        // 画面を揺らす
        CallFunc::create([this](){
            this->backgroundSprite->runAction(Sequence::create(
                MoveBy::create(0.05f, Vec2(0.f, 30.f)),
                MoveBy::create(0.05f, Vec2(0.f, -60.f)),
                MoveBy::create(0.05f, Vec2(0.f, 30.f)),
                nullptr
            ));
        }),
        DelayTime::create(0.15f),
        // 背景をぼかしつつ(くっきり版を消す)、ボタン類を出す
        CallFunc::create([&]() {
            this->runAction(Sequence::create(
                CallFunc::create([this](){
                    this->backgroundSpriteBlur->setVisible(true);
                    this->backgroundBox->setVisible(true);
                    this->backgroundSprite->runAction(Sequence::create(
                        FadeOut::create(1.f),
                        CallFunc::create([this](){
                            this->backgroundSprite->setVisible(false);
                        }),
                        nullptr
                    ));

                    this->singlePlayerButton->setVisible(true);
                    this->singlePlayerButton->runAction(FadeIn::create(1.f));

                    this->multiplayerButton->setVisible(true);
                    this->multiplayerButton->runAction(FadeIn::create(1.f));

                    this->creditsButtonBackground->setVisible(true);
                    this->creditsButtonBackground->runAction(FadeIn::create(1.f));

                    this->sphere1->setVisible(true);
                    this->sphere2->setVisible(true);
                    this->sphere3->setVisible(true);
                    this->sphere1->runAction(FadeIn::create(1.f));
                    this->sphere2->runAction(FadeIn::create(1.f));
                    this->sphere3->runAction(FadeIn::create(1.f));
                }),
                nullptr
            ));
        }),
        // (UI出現演出を待たずに)無限ループアニメーション系 (飛行機を揺らす・(TODO)背景を動かす)
        CallFunc::create([&](){
            this->airplane->runAction(RepeatForever::create(Sequence::create(
                EaseInOut::create(MoveBy::create(1.f, Vec2(0.f, -5.f)), 1.5f),
                EaseInOut::create(MoveBy::create(1.f, Vec2(0.f,  5.f)), 1.5f),
                nullptr
            )));

            this->sphere1->runAction(RepeatForever::create(RotateBy3D::create(2.f, Vec3(0.f, 180.f, 0.f))));
            this->sphere2->runAction(RepeatForever::create(RotateBy3D::create(2.f, Vec3(0.f, 180.f, 0.f))));
            this->sphere3->runAction(RepeatForever::create(RotateBy3D::create(2.f, Vec3(0.f, 180.f, 0.f))));
        }),
        nullptr
    ));
}