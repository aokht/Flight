//
//  HighScoreDataSource.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#include "HighScoreDataSource.h"
#include "GameCenterWrapper.h"
#include "FieldDataSource.h"

using namespace std;
using namespace cocos2d;

typedef GameCenterManagerDelegate::LeaderboardData LeaderboardData;

const int HighScoreDataSource::localHighScoreMaxCount = 3;
const string HighScoreDataSource::localHighScoreKeyPrefix = "highscore";

vector<LeaderboardData> HighScoreDataSource::getLocalHighScoreList(int stageId)
{
    vector<LeaderboardData> highScoreList;
    for (int rank = 1; rank <= localHighScoreMaxCount; ++rank) {
        int score = UserDefault::getInstance()->getIntegerForKey(getLocalHighScoreKeyString(stageId, rank).data(), -1);
        highScoreList.push_back({ score, rank });
    }

    return highScoreList;
}

void HighScoreDataSource::setLocalHighScoreIfBigger(int stageId, int score)
{
    vector<LeaderboardData> highScoreList = getLocalHighScoreList(stageId);
    highScoreList.push_back({
        score,
        -1
    });
    std::sort(highScoreList.begin(), highScoreList.end(), [](const LeaderboardData& a, const LeaderboardData& b) { return a.score > b.score; });

    UserDefault* data = UserDefault::getInstance();
    for (int i = 0; i < 3; ++i) {
        int rank = i + 1;
        data->setIntegerForKey(getLocalHighScoreKeyString(stageId, rank).data(), static_cast<int>(highScoreList[i].score));
    }
    data->flush();
}

bool HighScoreDataSource::isGameCenterLoggedIn()
{
    return GameCenterWrapper::getInstance()->isLoggedIn();
}

void HighScoreDataSource::setDelegate(GameCenterManagerDelegate *delegate)
{
    GameCenterWrapper::getInstance()->setDelegate(delegate);
}

void HighScoreDataSource::loadGlobalHighScoreList(int stageId)
{
    const string& key = getGlobalHighScoreKeyString(stageId);
    GameCenterWrapper::getInstance()->loadLeaderboardScores(key);
}

void HighScoreDataSource::setGlobalHighScore(int stageId, int score)
{
    const string& key = getGlobalHighScoreKeyString(stageId);
    GameCenterWrapper::getInstance()->postHighScore(key, score);
}


void HighScoreDataSource::showLeaderboard(int stageId)
{
    const string& key = getGlobalHighScoreKeyString(stageId);
    GameCenterWrapper::getInstance()->showLeaderboard(key);
}

string HighScoreDataSource::getGlobalHighScoreKeyString(int stageId)
{
    return FieldDataSource::findById(stageId).leaderboardHighScoreKey;
}

string HighScoreDataSource::getLocalHighScoreKeyString(int stageId, int rank)
{
    return StringUtils::format("%s_%d_%d", localHighScoreKeyPrefix.data(), stageId, rank);
}

void HighScoreDataSource::resetLocalHighScore()
{
#if COCOS2D_DEBUG
    UserDefault* data = UserDefault::getInstance();
    for (const FieldData& field : FieldDataSource::findAll()) {
        for (int rank = 1; rank <= localHighScoreMaxCount; ++rank) {
            data->setIntegerForKey(getLocalHighScoreKeyString(field.id, rank).data(), -1);
        }
    }
    data->flush();
#endif
}