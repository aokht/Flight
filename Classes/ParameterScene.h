//
//  ParameterScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#ifndef __Flight__ParameterScene__
#define __Flight__ParameterScene__

#include "cocos2d.h"
#include "cocosGUI.h"

class ParameterScene : public cocos2d::Layer
{
public:
    CREATE_FUNC(ParameterScene);
    static cocos2d::Scene* createScene();

private:
    cocos2d::Node* rootNode;
    cocos2d::ui::Button* nextButton;
    bool init();
    void onEnter() override;
    void grabElements();
    void setupUI();
};
#endif /* defined(__Flight__ParameterScene__) */
