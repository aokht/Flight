//
//  ResultScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/15.
//
//

#ifndef __Flight__ResultSceneMulti__
#define __Flight__ResultSceneMulti__

#include "cocos2d.h"
#include "cocosGUI.h"
#include "GameScene.h"

class ResultSceneMulti : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(const GameScene::GameScore& score);

private:
    cocos2d::Node* rootNode;
    cocos2d::ui::Button* lobbyButton;
    cocos2d::ui::Text* playerScoreLabel;
    cocos2d::ui::Text* otherScoreLabel;
    cocos2d::Node* fieldNode;
    cocos2d::Node* playerAirplaneNode;
    cocos2d::Node* otherAirplaneNode;

    static ResultSceneMulti* createWithScore(const GameScene::GameScore& score);
    bool init();
    void onEnter() override;
    void setupUI();
    void grabElements();

    void setupScores();

    ResultSceneMulti(const GameScene::GameScore& score);
    static cocos2d::Scene* create();
    const GameScene::GameScore score;
};

#endif /* defined(__Flight__ResultScene__) */