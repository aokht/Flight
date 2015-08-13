//
//  GameCenterWrapper.mm
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#include "GameCenterWrapper.h"
#include "GameCenterManager.h"

using namespace std;

typedef GameCenterManagerDelegate::LeaderboardData LeaderboardData;

GameCenterWrapper* GameCenterWrapper::getInstance()
{
    static GameCenterWrapper* instance = nullptr;

    if (! instance) {
        instance = new GameCenterWrapper();
    }

    return instance;
}

GameCenterWrapper::GameCenterWrapper()
{
    this->gamecenterManager = [[GameCenterManager alloc] init];
    [gamecenterManager setDelegate:this];
    [gamecenterManager retain];
}

GameCenterWrapper::~GameCenterWrapper()
{
    [gamecenterManager release];
    gamecenterManager = nil;
}

void GameCenterWrapper::setDelegate(GameCenterManagerDelegate* delegate)
{
    this->delegate = delegate;
}

void GameCenterWrapper::loginGameCenter()
{
    [gamecenterManager loginGameCenter];
}

bool GameCenterWrapper::isLoggedIn()
{
    return [gamecenterManager isLoggedIn];
}

void GameCenterWrapper::postHighScore(const std::string& key, int64_t score)
{
    [gamecenterManager postHighScore:[[[NSString alloc] initWithCString:key.c_str() encoding:NSUTF8StringEncoding] autorelease] score:score];
}

void GameCenterWrapper::loadLeaderboardScores(const std::string& key)
{
    [gamecenterManager loadLeaderboardScores:[[[NSString alloc] initWithCString:key.c_str() encoding:NSUTF8StringEncoding] autorelease]];
}

void GameCenterWrapper::showLeaderboard(const std::string& key)
{
    [gamecenterManager showLeaderboard:[[[NSString alloc] initWithCString:key.c_str() encoding:NSUTF8StringEncoding] autorelease]];
}

void GameCenterWrapper::receivedLeaderboardInfo(NSArray *scores)
{
    vector<LeaderboardData> leaderboardDataList;
    if (delegate) {
        for (long i = 0, count = [scores count]; i < count; i++) {
            GKScore* score = scores[i];
            leaderboardDataList.push_back({
                score.value,
                score.rank
            });
        }
        std::sort(leaderboardDataList.begin(), leaderboardDataList.end(),
            [](const LeaderboardData& a, const LeaderboardData& b){
                return a.rank < b.rank;
            }
        );

        delegate->receivedLeaderboardInfo(leaderboardDataList);
    }
}