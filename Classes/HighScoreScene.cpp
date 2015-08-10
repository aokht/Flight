//
//  HighScoreScene.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#include "HighScoreScene.h"
#include "SceneManager.h"
#include "HighScoreDataSource.h"
#include "FieldDataSource.h"

using namespace std;
using namespace cocos2d;

Scene* HighScoreScene::createSceneWithStageId(int stageId)
{
    auto scene = Scene::create();

    auto layer = HighScoreScene::createWithStageId(stageId);
    scene->addChild(layer);

    return scene;
}

HighScoreScene* HighScoreScene::createWithStageId(int stageId)
{
    HighScoreScene* pRet = new(std::nothrow) HighScoreScene();
    if (pRet && pRet->initWithStageId(stageId)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        return nullptr;
    }
}

bool HighScoreScene::initWithStageId(int stageId)
{
    if (! Layer::init()) {
        return false;
    }

    this->stageId = stageId;

    this->rootNode = SceneManager::createCSNode("HighScoreScene.csb");
    this->addChild(rootNode);

    return true;
}

void HighScoreScene::onEnter()
{
    Layer::onEnter();

    this->grabElements();
    this->setupUI();
    this->setupGlobalScore();
    this->setupLocalScore();
}

void HighScoreScene::onExit()
{
    Layer::onExit();

    // 後からコールバックが呼ばれないようにする
    HighScoreDataSource::setDelegate(nullptr);
}

void HighScoreScene::grabElements()
{
    this->stageNameLabel = this->rootNode->getChildByName<ui::Text*>("StageNameLabel");
    CCASSERT(stageNameLabel, "StageNameLabel in HighScoreScene is not found");

    this->highScoreLabel1 = this->rootNode->getChildByName<ui::Text*>("HighScoreValue_1");
    CCASSERT(highScoreLabel1, "HighScoreValue_1 in HighScoreScene is not found");
    this->highScoreLabel2 = this->rootNode->getChildByName<ui::Text*>("HighScoreValue_2");
    CCASSERT(highScoreLabel2, "HighScoreValue_2 in HighScoreScene is not found");
    this->highScoreLabel3 = this->rootNode->getChildByName<ui::Text*>("HighScoreValue_3");
    CCASSERT(highScoreLabel3, "HighScoreValue_3 in HighScoreScene is not found");

    this->rankingScoreLabel1 = this->rootNode->getChildByName<ui::Text*>("RankingValue_1");
    CCASSERT(rankingScoreLabel1, "RankingValue_1 in HighScoreScene is not found");
    this->rankingScoreLabel2 = this->rootNode->getChildByName<ui::Text*>("RankingValue_2");
    CCASSERT(rankingScoreLabel2, "RankingValue_2 in HighScoreScene is not found");
    this->rankingScoreLabel3 = this->rootNode->getChildByName<ui::Text*>("RankingValue_3");
    CCASSERT(rankingScoreLabel3, "RankingValue_3 in HighScoreScene is not found");

    this->gamecenterButton = this->rootNode->getChildByName<ui::Button*>("GameCenterButton");
    CCASSERT(gamecenterButton, "GameCenterButton in HighScoreScene is not found");

    this->backButton = this->rootNode->getChildByName<ui::Button*>("BackButton");
    CCASSERT(backButton, "BackButton in HighScoreScene is not found");
}

void HighScoreScene::setupUI()
{
    this->gamecenterButton->setEnabled(HighScoreDataSource::isGameCenterLoggedIn());
    this->gamecenterButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            HighScoreDataSource::showLeaderboard(this->stageId);
        }
    });

    this->backButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            SceneManager::getInstance()->backScene();
        }
    });

    const FieldData& data = FieldDataSource::findById(stageId);
    this->stageNameLabel->setString(data.name);

#if COCOS2D_DEBUG
    ui::Button* resetButton = this->rootNode->getChildByName<ui::Button*>("ResetButton");
    resetButton->setVisible(true);
    resetButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType eEventType) {
        if (eEventType == ui::Widget::TouchEventType::ENDED) {
            HighScoreDataSource::resetLocalHighScore();
            this->setupLocalScore();
        }
    });
#else
    ui::Button* resetButton = this->rootNode->getChildByName<ui::Button*>("ResetButton");
    if (resetButton) {
        resetButton->removeFromParent();
    }
#endif
}

void HighScoreScene::setupLocalScore()
{
    this->highScoreLabel1->setString("-");
    this->highScoreLabel2->setString("-");
    this->highScoreLabel3->setString("-");

    vector<GameCenterManagerDelegate::LeaderboardData> highScoreList = HighScoreDataSource::getLocalHighScoreList(stageId);
    for (size_t i = 0, last = highScoreList.size(); i < last; ++i) {
        ui::Text* label = nullptr;
        switch (i) {
            case 0: label = this->highScoreLabel1; break;
            case 1: label = this->highScoreLabel2; break;
            case 2: label = this->highScoreLabel3; break;
            default:
                break;
        }
        if (label && highScoreList[i].score > 0) {
            label->setString(StringUtils::toString(highScoreList[i].score));
        }
    }
}

void HighScoreScene::setupGlobalScore()
{
    HighScoreDataSource::setDelegate(this);
    HighScoreDataSource::loadGlobalHighScoreList(this->stageId);
}

void HighScoreScene::receivedLeaderboardInfo(vector<LeaderboardData> leaderboardDataList)
{
    this->rankingScoreLabel1->setString("-");
    this->rankingScoreLabel2->setString("-");
    this->rankingScoreLabel3->setString("-");

    for (size_t i = 0, last = leaderboardDataList.size(); i < last; ++i) {
        ui::Text* label = nullptr;
        switch (i) {
            case 0: label = this->rankingScoreLabel1; break;
            case 1: label = this->rankingScoreLabel2; break;
            case 2: label = this->rankingScoreLabel3; break;
            default:
                break;
        }
        if (label) {
            label->setString(StringUtils::toString(leaderboardDataList[i].score));
        }
    }
}