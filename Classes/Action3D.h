//
//  Action3D.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/04.
//
//

#ifndef __Flight__MoveTo3D__
#define __Flight__MoveTo3D__

#include "cocos2d.h"

class MoveTo3D : public cocos2d::ActionInterval
{
public:
    static MoveTo3D* create(float duration, const cocos2d::Vec3& position);

    void update(float progress) override;
    void startWithTarget(cocos2d::Node *target) override;

protected:
    cocos2d::Vec3 delta;
    cocos2d::Vec3 origin;
    cocos2d::Vec3 position;
    bool init(float duration, const cocos2d::Vec3& position);

};

class RotateBy3D : public cocos2d::ActionInterval
{
public:
    static RotateBy3D* create(float duration, const cocos2d::Vec3& angle);

    void update(float progress) override;
    void startWithTarget(cocos2d::Node *target) override;

protected:
    cocos2d::Vec3 origin;
    cocos2d::Vec3 angle;
    bool init(float duration, const cocos2d::Vec3& angle);
    
};

#endif /* defined(__Flight__MoveTo3D__) */
