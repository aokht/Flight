//
//  SceneManager.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#ifndef __Flight__SceneManager__
#define __Flight__SceneManager__

#include "cocos2d.h"

class SceneManager
{
public:
    static SceneManager* getInstance();

    void showLobbyScene();
    void showAirplaneSelectScene();
    void showStageSelectScene();
    void showGameScene();

private:
    SceneManager();
    ~SceneManager();
};


#endif /* defined(__Flight__SceneManager__) */
