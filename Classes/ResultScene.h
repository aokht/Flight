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
#include "GameScene.h"

class ResultScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(const GameScene::GameScore& score);
    static ResultScene* createWithScore(const GameScene::GameScore& score);

private:
    cocos2d::Node* rootNode;
    cocos2d::ui::Button* nextButton;
    cocos2d::ui::Button* lobbyButton;
    cocos2d::ui::Text* blueCountLabel;
    cocos2d::ui::Text* yellowCountLabel;
    cocos2d::ui::Text* redCountLabel;
    cocos2d::ui::Text* timeCountLabel;
    cocos2d::ui::Text* totalScoreLabel;
    cocos2d::Node* courseMapNode;

    bool init();
    void onEnter() override;
    void setupUI();
    void grabElements();

    void setupScores();

    ResultScene(const GameScene::GameScore& score);
    static cocos2d::Scene* create();
    const GameScene::GameScore score;
};

#endif /* defined(__Flight__ResultScene__) */