//
//  Global.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/12.
//
//

#include "Global.h"

using namespace std;
using namespace cocos2d;

ostream& operator<<(ostream& stream, const Vec2& vec)
{
    stream << "(" << vec.x << ", " << vec.y << ")";
    return stream;
}

ostream& operator<<(ostream& stream, const Vec3& vec)
{
    stream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return stream;
}