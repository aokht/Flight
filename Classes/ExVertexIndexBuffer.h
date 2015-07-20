//
//  ExVertexIndexBuffer.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#ifndef __Flight__ExVertexIndexBuffer__
#define __Flight__ExVertexIndexBuffer__

#include "cocos2d.h"

class ExVertexBuffer : public cocos2d::VertexBuffer
{
public:
    const std::vector<unsigned char>& getShadowCopy() const { return _shadowCopy; };

};

class ExIndexBuffer : public cocos2d::IndexBuffer
{
public:
    const std::vector<unsigned char>& getShadowCopy() const { return _shadowCopy; };

};

#endif /* defined(__Flight__ExVertexIndexBuffer__) */
