//
//  AirplaneSelectScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/22.
//
//

#ifndef __Flight__AirplaneSelectScene__
#define __Flight__AirplaneSelectScene__

#include "cocos2d.h"
#include "cocosGUI.h"

class AirplaneSelectScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(AirplaneSelectScene);
    static cocos2d::Scene* createScene();

private:
    cocos2d::Node* rootNode;
    cocos2d::Node* airplaneNode;
    cocos2d::Sprite* backgroundSprite;
    cocos2d::ui::Text* airplaneNameLabel;
    cocos2d::ui::Button* nextButton;
    cocos2d::ui::Button* backButton;
    cocos2d::ui::Button* leftButton;
    cocos2d::ui::Button* rightButton;

    bool init();
    void onEnter() override;
    void setupUI();
    void grabElements();
};

#endif /* defined(__Flight__AirplaneSelectScene__) */
