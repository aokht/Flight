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

class ExSprite3D;

class LobbyScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(LobbyScene);
    static cocos2d::Scene* createScene();

private:
    cocos2d::Node* rootNode;
    cocos2d::Sprite* titleSprite;
    cocos2d::ui::Button* singlePlayerButton;
    cocos2d::ui::Button* multiplayerButton;
    cocos2d::Sprite* backgroundSprite;
    cocos2d::Sprite* backgroundSpriteBlur;
    cocos2d::Sprite* backgroundBox;
    Airplane* airplane;
    ExSprite3D* sphere1;
    ExSprite3D* sphere2;
    ExSprite3D* sphere3;

    bool init();
    void onEnter() override;
    void onExit() override;
    void grabElements();
    void setupUI();
    void setup3DModels();
    void setupOpeningAnimations();
};

#endif /* defined(__Flight__LobbyScene__) */
