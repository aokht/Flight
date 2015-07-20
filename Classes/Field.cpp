//
//  Field.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#include "Field.h"
#include "ExSprite3D.h"

using namespace std;
using namespace cocos2d;

bool Field::init()
{
    if (! this->initWithFile("fields/field.obj")) {
        return false;
    }

    return true;
}