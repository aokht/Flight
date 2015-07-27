//
//  Global.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/12.
//
//

#ifndef __Flight__Global__
#define __Flight__Global__

#include <iostream>
#include "cocos2d.h"
#include "GameScene.h"

std::ostream& operator<<(std::ostream& stream, const cocos2d::Vec2& vec);
std::ostream& operator<<(std::ostream& stream, const cocos2d::Vec3& vec);

std::ostream& operator<<(std::ostream& stream, const GameScene::GameScore& score);


const float EPSILON = 0.00000011920928955078125f; //floatの限界精度(1/2^23)

#define SPRITE3DBATCHNODE_ENABLED 0

#endif /* defined(__Flight__Global__) */
