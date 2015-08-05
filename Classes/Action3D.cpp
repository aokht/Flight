//
//  Action3D.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/04.
//
//

#include "Action3D.h"

using namespace std;
using namespace cocos2d;

#pragma mark -
#pragma mark MoveTo3D

MoveTo3D* MoveTo3D::create(float duration, const Vec3& position)
{
    MoveTo3D* moveTo3D = new MoveTo3D();

    if (moveTo3D && moveTo3D->init(duration, position)) {
        moveTo3D->autorelease();
        return moveTo3D;
    }

    CC_SAFE_DELETE(moveTo3D);
    return nullptr;
}

bool MoveTo3D::init(float duration, const Vec3& position)
{
    if (! ActionInterval::initWithDuration(duration)) {
        return false;
    }

    this->position = position;

    return true;
}

void MoveTo3D::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);

    if (target) {
        this->origin = target->getPosition3D();
        this->delta = (position - origin) / _duration;
    }
}

void MoveTo3D::update(float progress)
{
    if (_target) {
        _target->setPosition3D(origin + delta * (_duration * progress));
    }
}


#pragma mark -
#pragma mark MoveTo3D

RotateBy3D* RotateBy3D::create(float duration, const Vec3& angle)
{
    RotateBy3D* rotateBy3D = new RotateBy3D();

    if (rotateBy3D && rotateBy3D->init(duration, angle)) {
        rotateBy3D->autorelease();
        return rotateBy3D;
    }

    CC_SAFE_DELETE(rotateBy3D);
    return nullptr;
}

bool RotateBy3D::init(float duration, const Vec3& angle)
{
    if (! ActionInterval::initWithDuration(duration)) {
        return false;
    }

    this->angle = angle;

    return true;
}

void RotateBy3D::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);

    if (target) {
        this->origin = target->getRotation3D();
    }
}

void RotateBy3D::update(float progress)
{
    if (_target) {
        _target->setRotation3D(origin + angle * progress);
    }
}