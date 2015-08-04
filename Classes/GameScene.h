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
#include "SceneData.h"

class Field;
class Airplane;
class Sphere;

class GameScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(GameScene);

    bool init();
    void onEnter() override;
    void update(float dt) override;

    void receivedData(const AirplaneInfoNetworkPacket& data);
    void receivedData(const GameScoreNetworkPacket& data);

    struct GameScore {
        bool isTimeUp;
        bool isCollided;
        bool isCompleted;
        float elapsedTime;
        std::vector<AchievedSphereInfo> sphereList;

        int otherAirplaneScore;
        bool isOtherAirplaneCollided;
        bool isOtherAirplaneCompleted;

        GameScore(
            bool isTimeUp = false,
            bool isCollided = false,
            bool isCompleted = false,
            float elapsedTime = 0.f,
            const std::vector<AchievedSphereInfo>& sphereList = std::vector<AchievedSphereInfo>()
        ) :
            isTimeUp(isTimeUp),
            isCollided(isCollided),
            isCompleted(isCompleted),
            elapsedTime(elapsedTime),
            sphereList(sphereList),
            otherAirplaneScore(-1),
            isOtherAirplaneCollided(false),
            isOtherAirplaneCompleted(false)
        {
        }
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
    cocos2d::Label* labelPosition;
    cocos2d::Label* labelRotation;
    cocos2d::Label* labelSpriteRotation;
    cocos2d::Label* labelRotationTarget;
    cocos2d::Label* labelTargetPosition;
    cocos2d::Label* labelTargetRotation;

    bool opening;
    bool running;
    float runningTime;
    bool onTouch;

    int coinCount;
    bool isCollided;
    bool isTimeUp;
    bool isCompleted;
    GameScore score;

    void setupField();
    void setupSpheres();
    void setupAirplane();
    void setupSkyDome();
    void setupCamera();
    void setupUI();
    void setupEventListeners();

    void updateRunningTime(float dt);
    void incrementCoinCount(int count);
    bool checkGameEnds();
    void endGame();
    bool isGameEnded() const;

    void sendAirplaneInfoWithSphereInfo(const std::vector<AchievedSphereInfo>& achievedSphereInfoList);
    void sendGameScore(const GameScore& score);

    void updateDebugInfo();

    GameScene();
    ~GameScene();

};

#endif /* defined(__Flight__GameScene__) */