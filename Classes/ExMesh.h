//
//  ExMesh.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/10.
//
//

#ifndef Flight_ExMesh_h
#define Flight_ExMesh_h

#include "cocos2d.h"

class ExMesh : public cocos2d::Mesh
{
public:
    void setAABB(const cocos2d::AABB& aabb)
    {
        this->_aabb = aabb;
    }

};

#endif
