//
//  ResultScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#ifndef __Flight__ResultScene__
#define __Flight__ResultScene__

#include "cocos2d.h"
#include "cocosGUI.h"

class ResultScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(ResultScene);
    static cocos2d::Scene* createScene();

private:
    cocos2d::Node* rootNode;
    cocos2d::ui::Button* nextButton;

    bool init();
    void onEnter() override;
    void setupUI();
    void grabElements();
};

#endif /* defined(__Flight__ResultScene__) */
