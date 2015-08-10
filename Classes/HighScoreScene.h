//
//  HighScoreScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#ifndef __Flight__HighScoreScene__
#define __Flight__HighScoreScene__

#include "cocos2d.h"
#include "cocosGUI.h"
#include "GameCenterWrapper.h"

class HighScoreScene : public cocos2d::Layer, GameCenterManagerDelegate
{
public:
    static cocos2d::Scene* createSceneWithStageId(int stageId);
    static HighScoreScene* createWithStageId(int stageId);

    void receivedLeaderboardInfo(std::vector<LeaderboardData> leaderboardInfoList) override;

protected:
    cocos2d::Node* rootNode;
    cocos2d::ui::Text* stageNameLabel;
    cocos2d::ui::Text* highScoreLabel1;
    cocos2d::ui::Text* highScoreLabel2;
    cocos2d::ui::Text* highScoreLabel3;
    cocos2d::ui::Text* rankingScoreLabel1;
    cocos2d::ui::Text* rankingScoreLabel2;
    cocos2d::ui::Text* rankingScoreLabel3;
    cocos2d::ui::Button* gamecenterButton;
    cocos2d::ui::Button* backButton;

    int stageId;

    bool initWithStageId(int stageId);
    void onEnter() override;
    void onExit() override;

    void grabElements();
    void setupUI();
    void setupLocalScore();
    void setupGlobalScore();
};
#endif /* defined(__Flight__HighScoreScene__) */
