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

class ResultScene;
class ParameterScene;

class GameSceneManager
{
public:
    static GameSceneManager* getInstance();

    void showGameScene();
    void showResultScene(const GameScene::GameScore& score);
    void showParameterScene(const GameScene::GameScore& score);
    void resetScene();

private:
    int sceneCount;

    GameSceneManager();
    ~GameSceneManager();

};

#endif /* defined(__Flight__GameSceneManager__) */
