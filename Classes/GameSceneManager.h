//
//  GameSceneManager.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#ifndef __Flight__GameSceneManager__
#define __Flight__GameSceneManager__

#include "GameScene.h"
#include "SceneData.h"

class ResultScene;
class ParameterScene;

class GameSceneManager
{
public:
    static GameSceneManager* getInstance();

    void showGameScene();
    void showResultScene(const GameScene::GameScore& score);
    void resetScene();

    void setSceneData(SceneData sceneData);
    SceneData getSceneData() const;
    void clearSceneData();

    void receivedData(const AirplaneInfoNetworkPacket& data);
    void receivedData(const GameScoreNetworkPacket& data);

    bool isSinglePlay() const {
        return sceneData.mode == SceneData::Mode::SINGLE;
    };
    bool isMultiplayMaster() const {
        return sceneData.mode == SceneData::Mode::MULTI_MASTER;
    };
    bool isMultiplaySlave() const {
        return sceneData.mode == SceneData::Mode::MULTI_SLAVE;
    }

private:
    GameScene* gameScene;
    int sceneCount;

    SceneData sceneData;

    GameSceneManager();
    ~GameSceneManager();

};

#endif /* defined(__Flight__GameSceneManager__) */
