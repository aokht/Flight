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
class MiniMap;

class GameScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(GameScene);

    bool init();
    void onEnter() override;
    void update(float dt) override;

    void receivedData(const AirplaneInfoNetworkPacket& data);
    void receivedData(const GameScoreNetworkPacket& data);

    const cocos2d::Vec3 getCameraPosition() const;
    const cocos2d::Vec3 getCameraEye() const;

    struct GameScore {
        bool isTimeUp;
        bool isCollided;
        bool isCompleted;
        float elapsedTime;
        std::vector<AchievedSphereInfo> sphereList;

        int otherAirplaneTotalScore;
        std::map<Sphere::Type, int> otherAirplaneScoreMap;
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
            otherAirplaneTotalScore(-1),
            otherAirplaneScoreMap(),
            isOtherAirplaneCollided(false),
            isOtherAirplaneCompleted(false)
        {
        }
    };

private:
    cocos2d::Node* rootNode;
    Field* field;
    cocos2d::Camera* camera;
    Airplane* airplane;
    cocos2d::Sprite3D* skydome;
    cocos2d::Sprite* header;
    cocos2d::Sprite* blueCircle;
    cocos2d::Sprite* yellowCircle;
    cocos2d::Sprite* redCircle;
    static const float circleScale;
    cocos2d::ui::Text* blueSphereCount;
    cocos2d::ui::Text* yellowSphereCount;
    cocos2d::ui::Text* redSphereCount;
    cocos2d::ui::Text* blueSphereTotalCount;
    cocos2d::ui::Text* yellowSphereTotalCount;
    cocos2d::ui::Text* redSphereTotalCount;
    cocos2d::ui::Text* labelTime;
    cocos2d::ui::Button* startButton;
    cocos2d::ui::Button* quitButton;
    cocos2d::Label* labelPosition;
    cocos2d::Label* labelRotation;
    cocos2d::Label* labelSpriteRotation;
    cocos2d::Label* labelRotationTarget;
    cocos2d::Label* labelTargetPosition;
    cocos2d::Label* labelTargetRotation;
    MiniMap* miniMap;

    bool isOpening;
    bool running;
    float runningTime;
    bool onTouch;

    int coinCount;
    bool isCollided;
    bool isTimeUp;
    bool isCompleted;
    GameScore score;

    void grabElemets();
    void setupField();
    void setupSpheres();
    void setupAirplane();
    void setupSkyDome();
    void setupCamera();
    void setupUI();
    void setupEventListeners();
    void setupMiniMap();

    int sphereEffectId;
    void playAirplaneSE();
    void playSphereSE();

    float getGameTime() const;
    void updateRunningTime(float dt);
    void updateSphereCount(const std::vector<AchievedSphereInfo>& achievedSphereInfoList);
    bool checkGameEnds();
    void startGame();
    void endGame();
    bool isGameEnded() const;
    void stopGame(bool strict = false);

    void sendAirplaneInfoWithSphereInfo(const std::vector<AchievedSphereInfo>& achievedSphereInfoList);
    void sendGameScore(const GameScore& score);

    void updateDebugInfo();

    GameScene();
    ~GameScene();

};

#endif /* defined(__Flight__GameScene__) */