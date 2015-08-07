//
//  CreditsScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/07.
//
//

#ifndef __Flight__CreditsScene__
#define __Flight__CreditsScene__

#include "cocos2d.h"

class CreditsScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(CreditsScene);

private:
    bool init();
};

#endif /* defined(__Flight__CreditsScene__) */
