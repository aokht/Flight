//
//  ExMesh.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/26.
//
//

#ifndef __Flight__ExMesh__
#define __Flight__ExMesh__

#include "cocos2d.h"

class ExMesh : public cocos2d::Mesh
{
public:
    cocos2d::MeshCommand& getMeshCommand()
    {
        return cocos2d::Mesh::getMeshCommand();
    };

    void setGLProgramState(cocos2d::GLProgramState* glProgramState)
    {
        Mesh::setGLProgramState(glProgramState);
    }

    void setAABB(cocos2d::AABB aabb)
    {
        this->_aabb = aabb;
    }

};
#endif /* defined(__Flight__ExMesh__) */
