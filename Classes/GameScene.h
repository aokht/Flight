//
//  GameScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#ifndef __Flight__GameScene__
#define __Flight__GameScene__

#include "cocos2d.h"
#include "cocosGUI.h"

class Field;
class Airplane;
class Sphere;

class GameScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(GameScene);
    static cocos2d::Scene* createScene();

    bool init();
    void onEnter() override;
    void update(float dt) override;

    struct GameScore {
        int coinsCount;
        float elapsedTime;
    };

private:
    Field* field;
    cocos2d::Camera* camera;
    Airplane* airplane;
    cocos2d::Sprite3D* skydome;
    cocos2d::Sprite* header;
    cocos2d::Label* labelCoins;
    cocos2d::Label* labelTime;
    cocos2d::ui::Button* startButton;
    cocos2d::ui::Button* stopButton;
    cocos2d::Label* labelPosition;
    cocos2d::Label* labelRotation;
    cocos2d::Label* labelSpriteRotation;
    cocos2d::Label* labelRotationTarget;

    bool running;
    float runningTime;
    bool onTouch;

    int coinCount;

    std::vector<Sphere*> sphereList;

    void setupField();
    void setupAirplane();
    void setupSkyDome();
    void setupCamera();
    void setupUI();
    void setupEventListeners();

    void updateRunningTime(float dt);
    void incrementCoinCount(int count);
    bool checkGameEnds();
    void endGame();

    void updateDebugInfo();
};
#endif /* defined(__Flight__GameScene__) */
