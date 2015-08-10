//
//  GameCenterManager.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#ifndef Flight_GameCenterManager_h
#define Flight_GameCenterManager_h

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

class GameCenterWrapper;

@interface GameCenterManager : NSObject<GKGameCenterControllerDelegate>
- (void) loginGameCenter;
- (void) postHighScore:(NSString*)key score:(int64_t)score;

- (void) showLeaderboard:(NSString*)key;
- (void) loadLeaderboardScores:(NSString*)key;
- (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController;

- (void)setDelegate:(GameCenterWrapper*)wrapper;

- (BOOL)isLoggedIn;

@end

#endif
