//
//  GameCenterWrapper.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#ifndef Flight_GameCenterWrapper_h
#define Flight_GameCenterWrapper_h

class GameCenterManagerDelegate
{
public:
    struct LeaderboardData
    {
        long long score;
        long rank;
    };

    virtual void receivedLeaderboardInfo(std::vector<LeaderboardData> scores) = 0;

};

#ifdef __OBJC__
@class GameCenterManager;
#else
typedef struct objc_object GameCenterManager;
class NSArray;
#endif

class GameCenterWrapper
{
public:

    static GameCenterWrapper* getInstance();

    void setDelegate(GameCenterManagerDelegate* delegate);

    void loginGameCenter();
    bool isLoggedIn();
    void postHighScore(const std::string& key, int64_t score);
    void showLeaderboard(const std::string& key);
    void loadLeaderboardScores(const std::string& key);

    void receivedLeaderboardInfo(NSArray* scores);

protected:
    GameCenterManager* gamecenterManager;
    GameCenterManagerDelegate* delegate;

    GameCenterWrapper();
    ~GameCenterWrapper();

};

#endif
