//
//  HighScoreDataSource.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#ifndef __Flight__HighScoreDataSource__
#define __Flight__HighScoreDataSource__

#include "GameCenterWrapper.h"

class HighScoreDataSource
{
public:
    static std::vector<GameCenterManagerDelegate::LeaderboardData> getLocalHighScoreList(int stageId);
    static void setLocalHighScoreIfBigger(int stageId, int score);

    static void setDelegate(GameCenterManagerDelegate* delegate);
    static void loadGlobalHighScoreList(int stageId);
    static void setGlobalHighScore(int stageId, int score);
    static void showLeaderboard(int stageId);

    static bool isGameCenterLoggedIn();

    // debug
    static void resetLocalHighScore();

protected:
    static const int localHighScoreMaxCount;
    static const std::string localHighScoreKeyPrefix;
    static std::string getLocalHighScoreKeyString(int stageId, int rank);
    static std::string getGlobalHighScoreKeyString(int stageId);
};


#endif /* defined(__Flight__HighScoreDataSource__) */
