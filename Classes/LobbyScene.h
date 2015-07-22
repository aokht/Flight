//
//  LobbyScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#ifndef __Flight__LobbyScene__
#define __Flight__LobbyScene__

#include "cocos2d.h"
#include "cocosGUI.h"
#include "GameScene.h"

class LobbyScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(LobbyScene);
    static cocos2d::Scene* createScene();

private:
    cocos2d::Node* rootNode;
    cocos2d::ui::Button* singlePlayerButton;
    cocos2d::ui::Button* multiplayerButton;

    bool init();
    void onEnter() override;
    void setupUI();
    void grabElements();
};

#endif /* defined(__Flight__LobbyScene__) */
