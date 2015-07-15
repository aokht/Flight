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
#include "GameScene.h"

class ParameterScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(const GameScene::GameScore& score);
    static ParameterScene* createWithScore(const GameScene::GameScore& score);

private:
    cocos2d::Node* rootNode;
    cocos2d::ui::Button* nextButton;
    bool init();
    void onEnter() override;
    void grabElements();
    void setupUI();

    static cocos2d::Scene* create();
    ParameterScene(const GameScene::GameScore& score);
    const GameScene::GameScore& score;
};
#endif /* defined(__Flight__ParameterScene__) */
