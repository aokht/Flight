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

private:
    cocos2d::Sprite3D* field;
    cocos2d::Camera* camera;
    Airplane* airplane;
    cocos2d::Sprite* header;
    cocos2d::Label* labelCoins;
    cocos2d::Label* labelTime;
    cocos2d::ui::Button* startButton;
    cocos2d::ui::Button* stopButton;

    bool running;
    float runningTime;
    bool onTouch;

    int coinCount;

    std::vector<Sphere*> sphereList;

    void setupField();
    void setupAirplane();
    void setupCamera();
    void setupUI();
    void setupEventListeners();

    void updateRunningTime(float dt);
    void incrementCoinCount(int count);
    bool checkGameEnds();
    void endGame();

    static void resetScene();
};
#endif /* defined(__Flight__GameScene__) */
