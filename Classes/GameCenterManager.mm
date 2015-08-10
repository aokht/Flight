//
//  GameCenterManager.m
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/09.
//
//

#import "GameCenterManager.h"
#import "GameCenterWrapper.h"


@implementation GameCenterManager {
    GameCenterWrapper* gameCenterWrapper;
}

- (void)setDelegate:(GameCenterWrapper*)wrapper
{
    gameCenterWrapper = wrapper;
}

-(void) loginGameCenter
{
    GKLocalPlayer* player = [GKLocalPlayer localPlayer];
    UIViewController* rootController = [UIApplication sharedApplication].keyWindow.rootViewController;
    player.authenticateHandler = ^(UIViewController* ui, NSError* error )
    {
        if( nil != ui ) {
            NSLog(@"Need to login");
            [rootController presentViewController:ui animated:YES completion:nil];
        }
        else if( player.isAuthenticated ) {
            NSLog(@"Authenticated");
        }
        else {
            NSLog(@"Failed");
        }
    };
}

- (BOOL)isLoggedIn
{
    return [GKLocalPlayer localPlayer].isAuthenticated;
}

-(void) postHighScore:(NSString*)key score:(int64_t)score{
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    if([localPlayer isAuthenticated])
    {
        GKScore *gkScore = [[GKScore alloc] initWithLeaderboardIdentifier:key];
        gkScore.value = score;
        gkScore.context = 0;
        [GKScore reportScores:[NSArray arrayWithObject:gkScore] withCompletionHandler:^(NSError *error) {
            if(error) {
                NSLog(@"Error : %@",error);
            }
            else {
                NSLog(@"Sent highscore. : %lld", score);
            }
        }];
    }
    else {
        NSLog(@"Gamecenter not authenticated.");
    }
}

-(void) showLeaderboard:(NSString *)key {
    GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
    if(localPlayer.authenticated){
        GKGameCenterViewController* gamecenterController = [GKGameCenterViewController new];
        if (gamecenterController != nil) {
            gamecenterController.gameCenterDelegate = self;
            gamecenterController.viewState = GKGameCenterViewControllerStateLeaderboards;
            gamecenterController.leaderboardIdentifier = key;
            [gamecenterController.leaderboardIdentifier retain];  // これでいいのか・・・？
            [[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:gamecenterController animated:YES completion:nil];
        }
    }
    else{
        UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"" message:@"GameCenterへのログインが必要です。" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
        [alertView show];
        [alertView release];
    }
}

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    [[UIApplication sharedApplication].keyWindow.rootViewController dismissViewControllerAnimated:YES completion:nil];
}

- (void)loadLeaderboardScores:(NSString *)key
{
    GKLeaderboard *leaderboardRequest = [[GKLeaderboard alloc] init];
    leaderboardRequest.playerScope = GKLeaderboardPlayerScopeGlobal;
    leaderboardRequest.timeScope = GKLeaderboardTimeScopeAllTime;
    leaderboardRequest.identifier = key;
    [leaderboardRequest.identifier retain];
    leaderboardRequest.range = NSMakeRange(1, 3);

    [leaderboardRequest loadScoresWithCompletionHandler: ^(NSArray *scores, NSError *error) {
        if (error == nil) {
            if (gameCenterWrapper) {
                gameCenterWrapper->receivedLeaderboardInfo(scores);
            }
        }
    }];
}

@end